
//#define METAMETHOD_2_ARGS 2
//#define METAMETHOD_1_ARGS 1

// general

#define IDX_CALL_STACK_MAIN 1

#define PRINT_DUAL_ERROR_OP(c)\
std::string var_type1=stk_result_op1->typeStr(),\
	   var_type2=stk_result_op2->typeStr();\
\
	VM_ERROR("cannot perform operator \"%s\" %s \"%s\". Check whether op1 and op2 are same type, or class implements the metamethod",\
		var_type1.c_str(),\
		c,\
		var_type2.c_str());\

#define PRINT_ERROR_OP(c)\
	std::string var_type1=stk_result_op1->typeStr();\
\
VM_ERROR("cannot perform preoperator %s\"%s\". Check whether op1 implements the metamethod",\
	c,\
	var_type1.c_str());\
	return NULL;

#define COPY_FLOAT(d,s)  memcpy((d),(s),sizeof(float))

namespace zetscript{

	inline bool  VirtualMachine::insertShareNode(InfoSharedList * list, InfoSharedPointerNode *_node){
		if(_node->next != NULL || _node->previous!=NULL) {
			VM_SET_USER_ERROR(this," Internal error expected node not in list");
			return false;
		}

		if(list->first == NULL){ /*one  node: trivial ?*/
			_node->previous=_node->next= list->last = list->first =_node;
		}
		else{ /* >1 node add to the end */
			// attach last-previous
			_node->previous=list->last;
			list->last->next=_node;
			list->last=_node;

			// attach next
			_node->next=list->first;
			list->first->previous=_node;
		}
		return true;
	}

	inline bool VirtualMachine::deattachShareNode(InfoSharedList * list, InfoSharedPointerNode *_node){

		if(list == NULL) return true;

		if(_node->next == NULL || _node->previous == NULL){
			VM_SET_USER_ERROR(this," An already deattached node");
			return false;
		}

		if((_node->previous == _node) && (_node->next == _node)){ // 1 single node...
			list->last=list->first=NULL;
		}
		else{ // dettach and attach next...
			// [1]<->[2]<-> ...[P]<->[C]<->[N]...[M-1]<->[M]
			//InfoSharedPointerNode *aux_node=_node;
			if(_node == list->first){
				list->first=_node->next;
			}
			else if(_node == list->last){
				list->last=_node->previous;
			}
			_node->previous->next=_node->next;
			_node->next->previous=_node->previous;

		}
		_node->previous = _node->next = NULL;
		return true;
	}


	inline void VirtualMachine::removeEmptySharedPointers(int idx_call_stack){

		if(idx_call_stack<=0){
			return;
		}

		InfoSharedList *list=&zero_shares[idx_call_stack];
		InfoSharedPointerNode *next_node=NULL,*current=list->first;


		if(current != NULL){
			bool finish=false;
			do{
				next_node=current->next;
				finish=next_node ==list->first;

				if(!deattachShareNode(list,current)){
					return;
				}

				delete current->data.shared_ptr;
				current->data.shared_ptr=NULL;
				free(current);

				current=next_node;

			}while(!finish);
		}
	}

	// defer all local vars
	inline void VirtualMachine::popVmScope(bool check_empty_shared_pointers) {

		if(vm_scope<vm_current_scope) // pop 1 scope
		{
			ScriptFunction *scope_info_function=(vm_current_scope-1)->script_function;
			Scope *scope         = (vm_current_scope-1)->scope;
			StackElement         * stk_local_vars=(vm_current_scope-1)->stk_local_vars;
			zs_vector			 * scope_symbols=scope->registered_symbols;
			//ScopeBlockVars   *scope_block_vars=&scope_info_function->scope_block_vars[index];

			for(uint8_t i = 0; i < scope_symbols->count; i++){
				Symbol *scope_symbol=(Symbol *)scope_symbols->items[i];
				StackElement *stk_local_var =&stk_local_vars[scope_symbol->idx_position]; // position where symbol is located on stack

				if(stk_local_var->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
					stk_local_var=(StackElement *)stk_local_var->var_ref;
				}

				ScriptObject *var = NULL;
				switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_local_var->properties)){
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT:
					var =((ScriptObject *)(stk_local_var->var_ref));
					if(var !=NULL){
						if(var->shared_pointer != NULL){
							if(!var->unrefSharedPtr(vm_idx_call)){
								return;
							}
						}
					}
				}
				*stk_local_var={
					0,
					0,
					MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED
				};
			}
			
			// remove deferred shared pointers except for return value...
			if(check_empty_shared_pointers){
				removeEmptySharedPointers(vm_idx_call);
			}

			// pop current var
			--vm_current_scope;
		}else{
			VM_SET_USER_ERROR(this,"internal error: trying to pop at the bottom");
		}

	}

	/*inline bool VirtualMachine::applySetter(
		 ScriptFunction *calling_function
		,Instruction *instruction
		,StackElement *stk_result_op1
		,StackElement *stk_result_op2

	) {

	}*/

	inline bool VirtualMachine::applyMetamethod(
		 ScriptFunction *calling_function
		,Instruction *instruction
		,ByteCodeMetamethod byte_code_metamethod
		,StackElement *stk_result_op1
		,StackElement *stk_result_op2
	
	) {
		//std::string str_symbol_to_find="";
		ScriptObject *calling_object=NULL;
		ScriptClass *script_class_aux=NULL;
		StackElement *stk_vm_current_backup,*stk_args;
		int stk_element_len=0;
		ScriptFunction *ptr_function_found=NULL;
		StackElement ret_obj;
		const char *byte_code_metamethod_operator_str=ByteCodeMetamethodToOperatorStr(byte_code_metamethod);
		const char *str_symbol_metamethod=ByteCodeMetamethodToSymbolStr(byte_code_metamethod);
		//zs_vector *stk_elements=NULL;
		std::string error_found="";
		zs_vector * list_props=NULL;
		ScriptObject *script_object=NULL;
		ScriptObject *one_param = NULL;
		int n_stk_args=getNumArgumentsStaticMetamethod(byte_code_metamethod);

		// init stk
		stk_vm_current_backup=stk_args=stk_vm_current;

		if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
			stk_result_op1 = (StackElement *)(stk_result_op1->var_ref);
		}

		if(stk_result_op1->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT){
			script_object = (ScriptObject *)(stk_result_op1->var_ref);
		}

		// only C refs can check 2nd param
		if(script_object == NULL) { // script null

			if(((stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK))){
				stk_result_op2 = (StackElement *)(stk_result_op2->var_ref);
			}

			if(stk_result_op2->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT){
				script_object = (ScriptObject *)(stk_result_op2->var_ref);
			}
		}


		if(script_object == NULL){ // cannot perform operation
			error_found="";
			goto apply_metamethod_error;
		}





		//stk_elements=script_class_aux->metamethod_operator[byte_code_metamethod];

		/*if(stk_elements == NULL){ // operator is not implemented
			error_found=zs_strutils::format("Operator metamethod \"%s (aka %s)\" is not implemented",str_symbol_metamethod,byte_code_metamethod_operator_str);
			goto apply_metamethod_error;
		}*/

		if(script_object->isNativeObject()){
			list_props=script_object->getAllProperties();//getFunctions();

			ptr_function_found = findFunction(
				 calling_object
				,calling_function
				,instruction
				,false
				,(void *)list_props->items
				,list_props->count
				,str_symbol_metamethod
				,stk_args
				,n_stk_args
			);

			if(ptr_function_found != NULL){
				if((ptr_function_found->symbol.properties & SYMBOL_PROPERTY_STATIC) == 0){
					error_found="Expected native %s to be static";
					goto apply_metamethod_error;
				}
			}

			else{ // try find non-static
				// search for non-static
				//stk_elements=script_object->getAllProperties();

				if((ptr_function_found = findFunction(
					 calling_object
					,calling_function
					,instruction
					,false
					,(void *)list_props->items
					,list_props->count
					,str_symbol_metamethod
					,stk_args+1
					,n_stk_args-1
				)) == NULL){
					error_found=zs_strutils::format("Operator metamethod \"%s (aka %s)\" it's not implemented or it cannot find appropriate arguments for calling function",str_symbol_metamethod,byte_code_metamethod_operator_str);
					goto apply_metamethod_error;
				}

				if((ptr_function_found->symbol.properties & SYMBOL_PROPERTY_STATIC) != 0){
					error_found="Expected native %s to be non-static";
					goto apply_metamethod_error;
				}
			}
		}else{ // get first item...
			StackElement * stk = script_object->getProperty(str_symbol_metamethod,NULL);

			if(stk == NULL){
				error_found=zs_strutils::format("Operator metamethod \"%s (aka %s)\" is not implemented",str_symbol_metamethod,byte_code_metamethod_operator_str);
				goto apply_metamethod_error;
			}

			if((stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION)==0){
				error_found=zs_strutils::format("Operator metamethod \"%s (aka %s)\" is not a function",str_symbol_metamethod,byte_code_metamethod_operator_str);
				goto apply_metamethod_error;
			}

			ptr_function_found=(ScriptFunction *)stk->var_ref;

		}

		// non static ignores first parameter and set calling object to allow this
		if((ptr_function_found->symbol.properties & SYMBOL_PROPERTY_STATIC) == 0){
			calling_object=script_object;
			stk_args++;
			n_stk_args--;
		}

		if((ptr_function_found->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF) == 0){
			ret_obj=callFunctionScript(
				calling_object
				,ptr_function_found
				,stk_args
				,n_stk_args
			);
		}else{ //
			ret_obj= callFunctionNative(
					ptr_function_found
					,stk_args
					,n_stk_args
					,instruction
					,calling_object
			);
		}

		if(ret_obj.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT){ //

			if(!((ScriptObject *)(ret_obj.var_ref))->initSharedPtr()){
				return false;
			}

			if(byte_code_metamethod != BYTE_CODE_METAMETHOD_SET){ /* Auto destroy C when ref == 0 */
				((ScriptObject *)(ret_obj.var_ref))->deleteNativeObjectOnDestroy(true);
			}
		}

		// reset stack...
		stk_vm_current=stk_vm_current_backup;

		if(byte_code_metamethod != BYTE_CODE_METAMETHOD_SET){ /* Auto destroy C when ref == 0 */
			*stk_vm_current++ = ret_obj;
		}

		return true;

apply_metamethod_error:

		if(n_stk_args==1){
			VM_ERROR("cannot perform operator \"%s %s\". %s"
				,byte_code_metamethod_operator_str
				,stk_result_op1->typeStr()
				,error_found.c_str()
				);
		}else{
			VM_ERROR("cannot perform operator \"%s %s %s\". %s"
				,stk_result_op1->typeStr()
				,byte_code_metamethod_operator_str
				,stk_result_op2->typeStr()
				,error_found.c_str()
				);
		}

		return false;


	}

	inline ScriptFunction * VirtualMachine::findFunction(
			ScriptObject *calling_object
			,ScriptFunction *calling_function
			,Instruction * instruction // call instruction
			,bool is_constructor
			,void *stk_elements_ptr // vector of properties
			,int stk_elements_len // vector of properties
			,const std::string & symbol_to_find
			,StackElement *stk_arg
			,unsigned char n_args
		) {

		// by default search over global functions...
		ScriptObject *var_object = NULL;
		ScriptFunction * ptr_function_found=NULL;
		std::string aux_string;
		bool stk_element_are_ptr=stk_elements_ptr!=vm_stack;

		for(int i = stk_elements_len-1; i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
			StackElement *stk_element=NULL;

			if(stk_element_are_ptr){
				stk_element=((StackElement **)stk_elements_ptr)[i];//(StackElement *)list_symbols->items[i];
			}else{
				stk_element=&((StackElement *)stk_elements_ptr)[i];
			}

			if((stk_element->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION) == 0){
				continue;
			}


			ScriptFunction *irfs = (ScriptFunction *)stk_element->var_ref;
			aux_string=irfs->symbol.name;

			if((aux_string == symbol_to_find && irfs->params->count == n_args)){
				if((irfs->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF)){ /* C! Must match all args...*/
					bool all_check=true; /*  check arguments types ... */
					int idx_type=-1;
					int arg_idx_type=-1;
					for( unsigned k = 0; k < n_args && all_check;k++){
						StackElement *current_arg=&stk_arg[k];
						arg_idx_type=((FunctionParam *)irfs->params->items[k])->idx_type;

						if(arg_idx_type!=IDX_BUILTIN_TYPE_STACK_ELEMENT){

							if(current_arg->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
								current_arg = (StackElement *)current_arg->var_ref;
							}

							unsigned short var_type = GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(current_arg->properties);

							switch(var_type){
								default:
									aux_string="unknow";
									all_check=false;
									break;
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT:
									idx_type=IDX_BUILTIN_TYPE_ZS_INT_PTR_C;
									all_check=
											arg_idx_type==IDX_BUILTIN_TYPE_ZS_INT_PTR_C
										  ||arg_idx_type==IDX_BUILTIN_TYPE_ZS_INT_C
										  ||arg_idx_type==IDX_BUILTIN_TYPE_FLOAT_PTR_C;
									break;
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
									idx_type=IDX_BUILTIN_TYPE_FLOAT_PTR_C;
									all_check=arg_idx_type==IDX_BUILTIN_TYPE_FLOAT_PTR_C
											||arg_idx_type==IDX_BUILTIN_TYPE_FLOAT_C
											||arg_idx_type==IDX_BUILTIN_TYPE_ZS_INT_PTR_C
										    ||arg_idx_type==IDX_BUILTIN_TYPE_ZS_INT_C;
									break;
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL:
									idx_type=IDX_BUILTIN_TYPE_BOOL_PTR_C;
									all_check=
											arg_idx_type==IDX_BUILTIN_TYPE_BOOL_PTR_C
										  ||arg_idx_type==IDX_BUILTIN_TYPE_BOOL_C;

									break;
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
									idx_type=IDX_BUILTIN_TYPE_STRING_PTR_C;

									all_check =
											(	arg_idx_type==IDX_BUILTIN_TYPE_STRING_PTR_C && current_arg->var_ref!=0)
										  ||	arg_idx_type==IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C;
									break;
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL:
									all_check=false;
									break;
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED:
									all_check=false;
									break;
								case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT:
									var_object=((ScriptObject *)current_arg->var_ref);
									aux_string=var_object->getNativePointerClassName();

									if(arg_idx_type==idx_type){
										all_check=true;
									}
									break;
							}
						}
					}

					if(all_check){ /* we found the right function (set it up!) ... */
						ptr_function_found = irfs;
					}
				}else{ /* type script function  ... */
					ptr_function_found = irfs;
				}
			}
		}

		if(ptr_function_found == NULL){
			int n_candidates=0;
			std::string str_candidates="";
			std::string args_str = "";
			/* get arguments... */

			for( unsigned k = 0; k < n_args;k++){
				StackElement *current_arg=&stk_arg[k];
				if(current_arg->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
					current_arg = (StackElement *)current_arg->var_ref;
				}

				if(k>0){
					args_str+=",";
				}
				unsigned short var_type = GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(current_arg->properties);

				switch(var_type){

				default:
					aux_string="unknow";
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT:
					aux_string=k_str_zs_int_type;
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
					aux_string=k_str_float_type;
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL:
					aux_string=k_str_bool_type;
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
					aux_string=k_str_string_type_ptr;
					if(current_arg->var_ref==0){ /* is constant char */
						aux_string=	k_str_const_char_type_ptr;
					}
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL:
					aux_string="NULL";
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED:
					aux_string="undefined";
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT:
					aux_string = ((ScriptObject *)current_arg->var_ref)->getNativePointerClassName();
					break;
				}
				args_str+=zs_rtti::demangle(aux_string);

				if(var_type == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT
				||var_type == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT
				||var_type == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL
				){
					args_str+=" [*] ";
				}
			}

			for(int i = stk_elements_len-1; i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
				StackElement *stk_element=NULL;
				if(stk_element_are_ptr){
					stk_element=((StackElement **)stk_elements_ptr)[i];//(StackElement *)list_symbols->items[i];
				}else{
					stk_element=&((StackElement *)stk_elements_ptr)[i];
				}
				if((stk_element->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION)== 0){
					continue;
				}

				ScriptFunction *irfs = (ScriptFunction *)stk_element->var_ref;


				if(irfs->symbol.name == symbol_to_find){

					if(n_candidates == 0){
						str_candidates+="\tPossible candidates are:\n\n";
					}
					str_candidates+="\t\t-"+(calling_object==NULL?""
							:calling_object->idx_class!=IDX_BUILTIN_TYPE_CLASS_MAIN?(calling_object->getClassName()+"::")
							:"")+irfs->symbol.name+"(";

					for(unsigned a = 0; a < irfs->params->count; a++){
						if(a>0){
							str_candidates+=",";
						}

						if(irfs->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF){
							str_candidates+=zs_rtti::demangle(
									GET_IDX_2_CLASS_C_STR(this,((FunctionParam *)irfs->params->items[a])->idx_type
							));
						}else{ /* typic var ... */
							str_candidates+="arg"+zs_strutils::int_to_str(a+1);
						}
					}
					str_candidates+=");\n";
					n_candidates++;
				}
			}

			if(n_candidates == 0){
				VM_ERROR("Cannot find %s \"%s%s(%s)\".\n\n",
						is_constructor ? "constructor":"function",
						calling_object==NULL?"":calling_object->idx_class!=IDX_BUILTIN_TYPE_CLASS_MAIN?(calling_object->getClassName()+"::").c_str():"",
								calling_function->getInstructionSymbolName(instruction),
						args_str.c_str()
				);

				return NULL;
			}
			else{
				VM_ERROR("Cannot match %s \"%s%s(%s)\" .\n\n%s",
					is_constructor ? "constructor":"function",
					calling_object==NULL?"":calling_object->idx_class!=IDX_BUILTIN_TYPE_CLASS_MAIN?(calling_object->getClassName()+"::").c_str():"",
							calling_function->getInstructionSymbolName(instruction),
					args_str.c_str(),
					str_candidates.c_str());
				return NULL;
			}
		}

		return ptr_function_found;
	}

	inline StackElement VirtualMachine::performAddString(StackElement *stk_result_op1,StackElement *stk_result_op2){
		// we have to create an new string variable

		//std::string *str;
		ScriptObjectString *script_var_string = NEW_STRING_VAR;
		StackElement stk_element={(void *)script_var_string->str_value.c_str(),script_var_string, MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING};
		script_var_string->initSharedPtr();

		std::string str1;
		std::string str2;

		std::string * str_dst[]={
			   &str1,
			   &str2
		};

		StackElement * stk_src[]={
			   stk_result_op1,
			   stk_result_op2
		};

		std::string  ** str_dst_it=str_dst;
		StackElement ** stk_src_it=stk_src;

		// str1
		for(unsigned i=0; i < 2; i++){
			StackElement *stk_src_item=(*stk_src_it);
			if(stk_src_item->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
				stk_src_item=(StackElement *)stk_src_item->var_ref;
			}

			switch(GET_STACK_ELEMENT_PROPERTY_PRIMITIVE_TYPES(stk_src_item->properties)){
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED:
				*(*str_dst_it)="undefined";
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL:
				*(*str_dst_it)="null";
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT:
				*(*str_dst_it)=zs_strutils::int_to_str((zs_int)(stk_src_item)->stk_value);
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
				*(*str_dst_it)=zs_strutils::float_to_str(*((float *)&((stk_src_item)->stk_value)));
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL:
				*(*str_dst_it)=(stk_src_item)->stk_value == 0?"false":"true";
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
				*(*str_dst_it)=(const char *)(stk_src_item)->stk_value;
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION:
				*(*str_dst_it)="function";
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_CLASS:
				*(*str_dst_it)="class";
				break;
			default:
				if(stk_src_item->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT){
					*(*str_dst_it)=((ScriptObject *)(stk_src_item)->var_ref)->toString();
				}
				else{
					*(*str_dst_it)="unknow";
				}

				break;
			}

			str_dst_it++;
			stk_src_it++;

		}

		// save result
		script_var_string->str_value=str1+str2;
		stk_element.stk_value=(void *)script_var_string->str_value.c_str();

		return stk_element;
	}

	inline StackElement VirtualMachine::performSubString(StackElement *stk_result_op1,StackElement *stk_result_op2){
		// we have to create an new string variable
		if((stk_result_op1->properties & stk_result_op1->properties) != MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){
			THROW_RUNTIME_ERROR("Expected both operants as string var");
		}

		//std::string *str;
		ScriptObjectString *script_var_string = NEW_STRING_VAR;
		StackElement stk_element={(void *)script_var_string->str_value.c_str(),script_var_string, MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING};
		script_var_string->initSharedPtr();


		script_var_string->str_value=(const char *)stk_result_op1->stk_value;
		script_var_string->str_value=zs_strutils::replace(script_var_string->str_value,(const char *)stk_result_op2->stk_value,"");

		stk_element.stk_value=(void *)script_var_string->str_value.c_str();

		return stk_element;
	}

}
