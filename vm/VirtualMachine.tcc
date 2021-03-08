
//#define METAMETHOD_2_ARGS 2
//#define METAMETHOD_1_ARGS 1

// general

#define IDX_CALL_STACK_MAIN 1

#define PUSH_UNDEFINED \
STK_SET_UNDEFINED(stk_vm_current++); \

#define PUSH_BOOLEAN(init_value) \
stk_vm_current->stk_value=(void *)((zs_int)(init_value)); \
stk_vm_current->properties=MSK_STK_PROPERTY_BOOL; \
stk_vm_current++;


#define PUSH_INTEGER(init_value) \
stk_vm_current->stk_value=(void *)((zs_int)(init_value)); \
stk_vm_current->properties=MSK_STK_PROPERTY_ZS_INT; \
stk_vm_current++;

/*#define PUSH_ARRAY_STK(n_stk) \
stk_vm_current->stk_value=(void *)((zs_int)(n_stk)); \
stk_vm_current->properties=MSK_STK_PROPERTY_ARRAY_STK; \
stk_vm_current++;

#define PUSH_CONST_CHAR(const_char) \
stk_vm_current->stk_value=(void *)((zs_int)(const_char)); \
stk_vm_current->properties=MSK_STK_PROPERTY_CONST_CHAR; \
stk_vm_current++;*/

#define PUSH_OBJECT(obj_value) \
stk_vm_current->stk_value=(void *)((zs_int)(obj_value)); \
stk_vm_current->properties=MSK_STK_PROPERTY_SCRIPT_OBJECT; \
stk_vm_current++;


#define PUSH_FLOAT(init_value) \
{\
	zs_float aux=(zs_float)(init_value); \
	ZS_FLOAT_COPY(&stk_vm_current->stk_value,&aux); \
	stk_vm_current->properties=MSK_STK_PROPERTY_ZS_FLOAT; \
	stk_vm_current++; \
}

#define PUSH_FUNCTION(ref) \
stk_vm_current->stk_value=(void *)ref; \
stk_vm_current->properties=MSK_STK_PROPERTY_FUNCTION; \
stk_vm_current++;

#define PUSH_CLASS(ref) \
stk_vm_current->stk_value=(void *)ref; \
stk_vm_current->properties=MSK_STK_PROPERTY_CLASS; \
stk_vm_current++;

// explains whether stk is this or not. Warning should be given as value and not as ptr
#define STK_IS_THIS(stk) (this_object != NULL && (stk)->stk_value == this_object)

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


// defer all local vars
#define POP_VM_SCOPE()\
 if(vm_scope>=vm_current_scope){\
	VM_SET_USER_ERROR(this,"internal error: trying to pop at the bottom");\
}\
{\
	VM_Scope *vm_check_scope=(vm_current_scope-1);\
	StackElement         * stk_local_vars	=vm_check_scope->stk_local_vars;\
	zs_vector *scope_symbols=vm_check_scope->scope->registered_symbols;\
	zs_int *symbols					=scope_symbols->items;\
	for(int i = scope_symbols->count-1; i >=0 ; --i){\
		StackElement *stk_local_var =stk_local_vars+((Symbol *)(symbols[i]))->idx_position;\
		if((stk_local_var->properties & MSK_STK_PROPERTY_PTR_STK)==MSK_STK_PROPERTY_PTR_STK){\
			stk_local_var=(StackElement *)stk_local_var->stk_value;\
		}\
		if((stk_local_var->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT)==MSK_STK_PROPERTY_SCRIPT_OBJECT){\
			ScriptObject *so=(ScriptObject *)(stk_local_var->stk_value);\
			if(so != NULL){\
				if(unrefSharedScriptObject(so,vm_idx_call)==false){\
					return;\
				}\
			}\
		}\
		STK_SET_UNDEFINED(stk_local_var);\
	}\
	--vm_current_scope;\
}
	


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
			VM_SET_USER_ERROR(this," Internal error: An already deattached node");
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

				delete current->data.ptr_script_object_shared;
				current->data.ptr_script_object_shared=NULL;
				free(current);

				current=next_node;

			}while(!finish);
		}
	}

	inline bool VirtualMachine::createSharedPointer(ScriptObject *_obj){
		if(_obj->shared_pointer == NULL){
			InfoSharedPointerNode *_node = (InfoSharedPointerNode *)malloc(sizeof(InfoSharedPointerNode));
			// init
			_node->previous=NULL;
			_node->next=NULL;
			_node->data.n_shares=0;
			_node->data.ptr_script_object_shared=_obj;
			_node->data.zero_shares=&zero_shares[vm_idx_call]; // it saves the zeros nodes where was set

			// insert node into shared nodes ...
			if(!insertShareNode(_node->data.zero_shares,_node)){
				return false;
			}
			_obj->shared_pointer=_node;
			return true;
		}else{
			VM_SET_USER_ERROR(this," shared ptr already has a shared pointer data");
		}

		return false;

	}

	inline bool VirtualMachine::sharePointer(ScriptObject *_obj){

		InfoSharedPointerNode *_node=_obj->shared_pointer;

		unsigned short *n_shares = &_node->data.n_shares;

		bool move_to_shared_list=*n_shares==0;

		if(*n_shares >= MAX_SHARES_VARIABLE){
			VM_SET_USER_ERROR(this,"MAX SHARED VARIABLES (Max. %i)",MAX_SHARES_VARIABLE);
			return false;
		}

		(*n_shares)++;

		if(move_to_shared_list){

			// Mov to shared pointer...
			if(!deattachShareNode(_node->data.zero_shares,_node)){
				return false;
			}
			// update current stack due different levels from functions!
			//_node->currentStack=idx_current_call;
			if(!insertShareNode(&shared_vars,_node)){
				return false;
			}

			// node is not in list of zero refs anymore...
			_node->data.zero_shares=NULL;
		}
		return true;
	}

	inline bool VirtualMachine::decrementShareNodesAndDettachIfZero(InfoSharedPointerNode *_node, bool & is_dettached){

		is_dettached=false;
		unsigned short *n_shares = &_node->data.n_shares;
		if(*n_shares > 0){ // not zero
			if(--(*n_shares)==0){ // mov back to 0s shares (candidate to be deleted on GC check)

				if(!deattachShareNode(&shared_vars,_node)){
					return false;
				}

				is_dettached=true;
			}
		}

		return true;
	}

	inline bool VirtualMachine::unrefSharedScriptObjectAndRemoveIfZero(ScriptObject **so){

		InfoSharedPointerNode *_node=(*so)->shared_pointer;
		bool is_dettached=false;

		if(decrementShareNodesAndDettachIfZero(_node,is_dettached)){

			if(is_dettached == true){
				delete _node->data.ptr_script_object_shared; // it deletes shared_script_object
				free(_node);
			}
			return true;
		}

		return false;
	}


	inline bool VirtualMachine::unrefSharedScriptObject(ScriptObject *_obj, int idx_current_call){

		InfoSharedPointerNode *shared_pointer=_obj->shared_pointer;
		if(shared_pointer!=NULL){
			bool is_dettached=false;
			if(decrementShareNodesAndDettachIfZero(shared_pointer,is_dettached)){
				if(is_dettached){
					shared_pointer->data.zero_shares=&zero_shares[idx_current_call];
					if(!insertShareNode(shared_pointer->data.zero_shares,shared_pointer)){ // insert to zero shares vector to remove automatically on ending scope
						return false;
					}
				}
				return true;
			}
		}else{
			VM_SET_USER_ERROR(this->zs->getVirtualMachine(),"shared ptr not registered");
		}

		return false;
	}

	inline bool VirtualMachine::applyMetamethodPrimitive(
			 ScriptFunction *calling_function
			,Instruction *instruction
			,ByteCodeMetamethod byte_code_metamethod
			,StackElement *stk_result_op1
			,StackElement *stk_result_op2
			, bool & error
	){
		//error=true;
		switch(byte_code_metamethod){
		case ByteCodeMetamethod::BYTE_CODE_METAMETHOD_ADD:
			if(
				STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1)
					||
				STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)
			){
				ScriptObjectString *so_string=ScriptObjectString::newScriptObjectStringAddStk(this->zs,stk_result_op1,stk_result_op2);
				createSharedPointer(so_string);
				PUSH_OBJECT(so_string);
				return true;
			}
			else if(
				STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op1)
					&&
				STK_IS_SCRIPT_OBJECT_VECTOR(stk_result_op2)
			){
				ScriptObjectVector *so_vector=ScriptObjectVector::newScriptObjectVectorAdd(
						this->zs
						,(ScriptObjectVector *)stk_result_op1->stk_value
						,(ScriptObjectVector *)stk_result_op2->stk_value
				);
				createSharedPointer(so_vector);
				PUSH_OBJECT(so_vector);

				return true;
			}
			else if(
				STK_IS_SCRIPT_OBJECT_OBJECT(stk_result_op1)
					&&
				STK_IS_SCRIPT_OBJECT_OBJECT(stk_result_op2)
			){
				ScriptObjectObject *so_object=ScriptObjectObject::newScriptObjectObjectAdd(
						this->zs
						,(ScriptObjectObject *)stk_result_op1->stk_value
						,(ScriptObjectObject *)stk_result_op2->stk_value
				);
				createSharedPointer(so_object);
				PUSH_OBJECT(so_object);
			}
			break;
		default:
			break;
		}

		return false;
	}

	inline bool VirtualMachine::applyMetamethod(
		 ScriptFunction *calling_function
		,Instruction *instruction
		,ByteCodeMetamethod byte_code_metamethod
		,StackElement *stk_result_op1
		,StackElement *stk_result_op2
	
	) {
		bool error=false;

		//std::string str_symbol_to_find="";
		std::string str_stk_result_op1_full_definition="";
		std::string str_stk_result_op2_full_definition="";
		ScriptObjectClass *calling_object=NULL;
		ScriptClass *script_class_aux=NULL;
		StackElement *stk_vm_current_backup,*stk_args;
		int stk_element_len=0;
		ScriptFunction *ptr_function_found=NULL;
		StackElement ret_obj;
		const char *byte_code_metamethod_operator_str=byte_code_metamethod_to_operator_str(byte_code_metamethod);
		const char *str_symbol_metamethod=byte_code_metamethod_to_symbol_str(byte_code_metamethod);
		//zs_vector *stk_elements=NULL;
		std::string error_found="";
		zs_vector * list_props=NULL;
		ScriptObjectClass *script_object_class=NULL;
		ScriptObjectClass *one_param_object_class = NULL;
		int n_stk_args=get_num_arguments_static_metamethod(byte_code_metamethod);
		StackElement *stk_return=NULL;
		int n_returned_arguments_from_function=0;
		//unsigned char n_args=instruction->value_op1; // number arguments will pass to this function
		//StackElement *stk_start_arg_call=(stk_vm_current-n_args);


		// init stk
		stk_vm_current_backup=stk_args=stk_vm_current;

		if(stk_result_op1->properties & MSK_STK_PROPERTY_PTR_STK){
			stk_result_op1 = (StackElement *)(stk_result_op1->stk_value);
		}

		if(STK_IS_SCRIPT_OBJECT_CLASS(stk_result_op1)){
			script_object_class = (ScriptObjectClass *)(stk_result_op1->stk_value);
		}

		// only C refs can check 2nd param
		if(script_object_class == NULL) { // script null

			if(((stk_result_op2->properties & MSK_STK_PROPERTY_PTR_STK))){
				stk_result_op2 = (StackElement *)(stk_result_op2->stk_value);
			}

			if(STK_IS_SCRIPT_OBJECT_CLASS(stk_result_op2)){
				script_object_class = (ScriptObjectClass *)(stk_result_op2->stk_value);
			}
		}


		if(applyMetamethodPrimitive(
					 calling_function
					,instruction
					,byte_code_metamethod
					,stk_result_op1
					,stk_result_op2
					, error
			)){
			return true;
		}

		if(error == true){
			return false;
		}



		if(script_object_class == NULL){ // cannot perform operation
			error_found="";
			goto apply_metamethod_error;
		}

		if(script_object_class->isNativeObject()){
			list_props=script_object_class->getAllElements();//getFunctions();

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
			StackElement * stk = script_object_class->getProperty(str_symbol_metamethod,NULL);

			if(stk == NULL){
				error_found=zs_strutils::format("Operator metamethod \"%s (aka %s)\" is not implemented",str_symbol_metamethod,byte_code_metamethod_operator_str);
				goto apply_metamethod_error;
			}

			if((stk->properties & MSK_STK_PROPERTY_FUNCTION)==0){
				error_found=zs_strutils::format("Operator metamethod \"%s (aka %s)\" is not a function",str_symbol_metamethod,byte_code_metamethod_operator_str);
				goto apply_metamethod_error;
			}

			ptr_function_found=(ScriptFunction *)stk->stk_value;

		}

		// non static ignores first parameter and set calling object to allow this
		if((ptr_function_found->symbol.properties & SYMBOL_PROPERTY_STATIC) == 0){
			calling_object=script_object_class;
			stk_args++;
			n_stk_args--;
		}

		if((ptr_function_found->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF) == 0){
			callFunctionScript(
				calling_object
				,ptr_function_found
				,stk_args
				,n_stk_args
			);
		}else{ //
			callFunctionNative(
					ptr_function_found
					,stk_args
					,n_stk_args
					,instruction
					,calling_object
			);
		}

		stk_return=(stk_args+ptr_function_found->registered_symbols->count );
		n_returned_arguments_from_function=stk_vm_current-stk_return;

		// setup all returned variables from function
		for(int i=0; i < n_returned_arguments_from_function; i++){

			StackElement *stk_ret = --stk_vm_current;

			// if a scriptvar --> init shared
			if(stk_ret->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
				ScriptObject *sv=(ScriptObject *)stk_ret->stk_value;

				if(byte_code_metamethod != BYTE_CODE_METAMETHOD_SET){ /* Auto destroy C when ref == 0 */
					((ScriptObjectClass *)(stk_ret->stk_value))->deleteNativeObjectOnDestroy(true);
				}

				if(sv->shared_pointer == NULL){ // if return this, it holds ptr_shared_pointer
					if(!createSharedPointer(sv)){
						return false;
					}
				}
			}
			// ... and push result if not function constructor...
		}

		ret_obj=stk_return[0];

		/*if(STK_IS_SCRIPT_OBJECT_CLASS(&ret_obj)){ //

			if(!((ScriptObject *)(ret_obj.stk_value))->initSharedPtr()){
				return false;
			}

		}*/

		// reset stack...
		stk_vm_current=stk_vm_current_backup;

		if(byte_code_metamethod != BYTE_CODE_METAMETHOD_SET){ /* Auto destroy C when ref == 0 */
			*stk_vm_current++ = ret_obj;
		}

		return true;

apply_metamethod_error:


		if(n_stk_args==1){
			VM_ERROR("cannot perform operation \"%s %s\". %s"
				,byte_code_to_operator_str(instruction->byte_code)
				,stk_result_op1->toString().c_str()
				,error_found.c_str()
				);
		}else{
			VM_ERROR("cannot perform operation \"%s %s %s\". %s"
				,stk_result_op1->toString().c_str()
				,byte_code_to_operator_str(instruction->byte_code)
				,stk_result_op2->toString().c_str()
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

		ScriptFunction * ptr_function_found=NULL;
		std::string aux_string;
		bool stk_element_are_vector_element_ptr=stk_elements_ptr!=vm_stack;

		for(int i = stk_elements_len-1; i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
			StackElement *stk_element=NULL;

			if(stk_element_are_vector_element_ptr){
				stk_element=(StackElement *)(((zs_int *)stk_elements_ptr)[i]);//(StackElement *)list_symbols->items[i];
			}else{
				stk_element=&((StackElement *)stk_elements_ptr)[i];
			}

			if((stk_element->properties & MSK_STK_PROPERTY_FUNCTION) == 0){
				continue;
			}


			ScriptFunction *irfs = (ScriptFunction *)stk_element->stk_value;
			aux_string=irfs->symbol.name;

			if((aux_string == symbol_to_find && irfs->params->count == n_args)){
				if((irfs->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF)){ /* C! Must match all args...*/
					bool all_check=true; /*  check arguments types ... */
					int idx_type=-1;
					int arg_idx_type=-1;
					for( unsigned k = 0; k < n_args && all_check;k++){
						StackElement *current_arg=&stk_arg[k];
						arg_idx_type=((ScriptFunctionArg *)irfs->params->items[k])->idx_type;

						if(arg_idx_type!=IDX_BUILTIN_TYPE_STACK_ELEMENT){

							if(current_arg->properties & MSK_STK_PROPERTY_PTR_STK){
								current_arg = (StackElement *)current_arg->stk_value;
							}

							unsigned short var_type = GET_MSK_STK_PROPERTY_TYPES(current_arg->properties);

							switch(var_type){
								default:
									aux_string="unknow";
									all_check=false;
									break;
								case MSK_STK_PROPERTY_ZS_INT:
									idx_type=IDX_BUILTIN_TYPE_ZS_INT_PTR_C;
									all_check=
											arg_idx_type==IDX_BUILTIN_TYPE_ZS_INT_PTR_C
										  ||arg_idx_type==IDX_BUILTIN_TYPE_ZS_INT_C
										  ||arg_idx_type==IDX_BUILTIN_TYPE_FLOAT_PTR_C;
									break;
								case MSK_STK_PROPERTY_ZS_FLOAT:
									idx_type=IDX_BUILTIN_TYPE_FLOAT_PTR_C;
									all_check=arg_idx_type==IDX_BUILTIN_TYPE_FLOAT_PTR_C
											||arg_idx_type==IDX_BUILTIN_TYPE_FLOAT_C
											||arg_idx_type==IDX_BUILTIN_TYPE_ZS_INT_PTR_C
										    ||arg_idx_type==IDX_BUILTIN_TYPE_ZS_INT_C;
									break;
								case MSK_STK_PROPERTY_BOOL:
									idx_type=IDX_BUILTIN_TYPE_BOOL_PTR_C;
									all_check=
											arg_idx_type==IDX_BUILTIN_TYPE_BOOL_PTR_C
										  ||arg_idx_type==IDX_BUILTIN_TYPE_BOOL_C;

									break;
								case MSK_STK_PROPERTY_UNDEFINED:
									all_check=false;
									break;
								case MSK_STK_PROPERTY_SCRIPT_OBJECT:

									if(STK_IS_SCRIPT_OBJECT_STRING(current_arg)){
										idx_type=IDX_BUILTIN_TYPE_STRING_PTR_C;

										all_check =
											(	arg_idx_type==IDX_BUILTIN_TYPE_STRING_PTR_C && current_arg->stk_value!=0)
										  ||	arg_idx_type==IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C;
									}else if(STK_IS_SCRIPT_OBJECT_CLASS(current_arg)){
										ScriptObjectClass *var_object_class=((ScriptObjectClass *)current_arg->stk_value);
										aux_string=var_object_class->getClassName();
										if(arg_idx_type==idx_type){
											all_check=true;
										}
									}else{
										ScriptObject *var_object = NULL;
										var_object=((ScriptObject *)current_arg->stk_value);
										aux_string=var_object->getClassName();
										if(arg_idx_type==idx_type){
											all_check=true;
										}
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
				if(current_arg->properties & MSK_STK_PROPERTY_PTR_STK){
					current_arg = (StackElement *)current_arg->stk_value;
				}

				if(k>0){
					args_str+=",";
				}
				unsigned short var_type = GET_MSK_STK_PROPERTY_TYPES(current_arg->properties);

				switch(var_type){

				default:
					aux_string="unknow";
					break;
				case MSK_STK_PROPERTY_ZS_INT:
					aux_string=k_str_zs_int_type;
					break;
				case MSK_STK_PROPERTY_ZS_FLOAT:
					aux_string=k_str_float_type;
					break;
				case MSK_STK_PROPERTY_BOOL:
					aux_string=k_str_bool_type;
					break;
				case MSK_STK_PROPERTY_UNDEFINED:
					aux_string="undefined";
					break;
				case MSK_STK_PROPERTY_SCRIPT_OBJECT:
					if(STK_IS_SCRIPT_OBJECT_STRING(current_arg)){
						aux_string=k_str_string_type_ptr;
						if(current_arg->stk_value==0){ /* is constant char */
							aux_string=	k_str_const_char_type_ptr;
						}
					}else if(STK_IS_SCRIPT_OBJECT_CLASS(current_arg)){
						aux_string = ((ScriptObjectClass *)current_arg->stk_value)->getNativePointerClassName();
					}else{ // object
						aux_string = ((ScriptObject *)current_arg->stk_value)->getClassName();
					}
					break;
				}
				args_str+=zs_rtti::demangle(aux_string);

				if(var_type == MSK_STK_PROPERTY_ZS_INT
				||var_type == MSK_STK_PROPERTY_ZS_FLOAT
				||var_type == MSK_STK_PROPERTY_BOOL
				){
					args_str+=" [*] ";
				}
			}

			for(int i = stk_elements_len-1; i>=0 && ptr_function_found==NULL; i--){ /* search all function that match symbol ... */
				StackElement *stk_element=NULL;
				if(stk_element_are_vector_element_ptr){
					stk_element=(StackElement *)(((zs_int *)stk_elements_ptr)[i]);//(StackElement *)list_symbols->items[i];
				}else{
					stk_element=&((StackElement *)stk_elements_ptr)[i];
				}
				if((stk_element->properties & MSK_STK_PROPERTY_FUNCTION)== 0){
					continue;
				}

				ScriptFunction *irfs = (ScriptFunction *)stk_element->stk_value;


				if(irfs->symbol.name == symbol_to_find){

					if(n_candidates == 0){
						str_candidates+="\tPossible candidates are:\n\n";
					}
					str_candidates+="\t\t-"+(calling_object==NULL?""
							:calling_object->idx_script_class!=IDX_BUILTIN_TYPE_MAIN?(calling_object->getClassName()+"::")
							:"")+irfs->symbol.name+"(";

					for(unsigned a = 0; a < irfs->params->count; a++){
						if(a>0){
							str_candidates+=",";
						}

						if(irfs->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF){
							str_candidates+=zs_rtti::demangle(
									GET_IDX_2_CLASS_C_STR(this,((ScriptFunctionArg *)irfs->params->items[a])->idx_type
							));
						}else{ /* typic var ... */
							str_candidates+="arg"+zs_strutils::zs_int_to_str(a+1);
						}
					}
					str_candidates+=");\n";
					n_candidates++;
				}
			}

			if(n_candidates == 0){
				VM_ERROR("Cannot find %s \"%s%s(%s)\".\n\n",
						is_constructor ? "constructor":"function",
						calling_object==NULL?"":calling_object->idx_script_class!=IDX_BUILTIN_TYPE_MAIN?(calling_object->getClassName()+"::").c_str():"",
								calling_function->getInstructionSymbolName(instruction),
						args_str.c_str()
				);

				return NULL;
			}
			else{
				VM_ERROR("Cannot match %s \"%s%s(%s)\" .\n\n%s",
					is_constructor ? "constructor":"function",
					calling_object==NULL?"":calling_object->idx_script_class!=IDX_BUILTIN_TYPE_MAIN?(calling_object->getClassName()+"::").c_str():"",
							calling_function->getInstructionSymbolName(instruction),
					args_str.c_str(),
					str_candidates.c_str());
				return NULL;
			}
		}

		return ptr_function_found;
	}



}
