/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define RETURN_ERROR\
	error=true;\
	return stk_result;



#define PROCESS_MOD_OPERATION \
{ \
	unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){\
			PUSH_INTEGER(STK_VALUE_TO_INT(stk_result_op1) % STK_VALUE_TO_INT(stk_result_op2));\
	}\
	else if(properties == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){\
			if (STK_VALUE_IS_INT(stk_result_op1) && STK_VALUE_IS_FLOAT(stk_result_op2)){\
				COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
				PUSH_FLOAT(fmod(STK_VALUE_TO_INT(stk_result_op1) , f_aux_value2));\
			}else{\
				COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
				PUSH_FLOAT(fmod(f_aux_value1 , STK_VALUE_TO_INT(stk_result_op2)));\
			}\
	}\
	else if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){\
			COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
			COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
			PUSH_FLOAT(fmod(f_aux_value1 , f_aux_value2));\
	}\
	else{\
		if(!applyMetamethod(\
						 calling_object\
						,calling_function\
						,instruction\
						,STR(%)\
						,BYTE_CODE_METAMETHOD_MOD\
						,stk_result_op1\
						,stk_result_op2\
		)){\
			RETURN_ERROR;\
		}\
	}\
}

#define PROCESS_ARITHMETIC_OPERATION(__OVERR_OP__, __METAMETHOD__)\
{\
	unsigned short properties = GET_INS_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){\
			PUSH_INTEGER(STK_VALUE_TO_INT(stk_result_op1) __OVERR_OP__ STK_VALUE_TO_INT(stk_result_op2));\
	}\
	else if(properties == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){\
			if (STK_VALUE_IS_INT(stk_result_op1) && STK_VALUE_IS_FLOAT(stk_result_op2)){\
				COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
				PUSH_FLOAT(STK_VALUE_TO_INT(stk_result_op1) __OVERR_OP__ f_aux_value2);\
			}else{\
				COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
				PUSH_FLOAT(f_aux_value1 __OVERR_OP__ STK_VALUE_TO_INT(stk_result_op2));\
			}\
	}\
	else if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){\
			COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
			COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
			PUSH_FLOAT(f_aux_value1 __OVERR_OP__ f_aux_value2);\
	}\
	else{\
		if(!applyMetamethod(\
						calling_object\
						,calling_function\
						,instruction\
						,STR(__OVERR_OP__)\
						,__METAMETHOD__\
						,stk_result_op1\
						,stk_result_op2\
		)){\
			RETURN_ERROR;\
		}\
	}\
}

#define PROCESS_COMPARE_OPERATION(__OVERR_OP__, __METAMETHOD__)\
{\
	unsigned short properties = GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){\
			PUSH_BOOLEAN(STK_VALUE_TO_INT(stk_result_op1) __OVERR_OP__ STK_VALUE_TO_INT(stk_result_op2));\
	}\
	else if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN){\
		PUSH_BOOLEAN(STK_VALUE_TO_BOOL(stk_result_op1) __OVERR_OP__ STK_VALUE_TO_BOOL(stk_result_op2));\
	}\
	else if(properties == (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)){\
			if (STK_VALUE_IS_INT(stk_result_op1) && STK_VALUE_IS_FLOAT(stk_result_op2)){\
				COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
				PUSH_BOOLEAN(STK_VALUE_TO_INT(stk_result_op1) __OVERR_OP__ f_aux_value2);\
			}else{\
				COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
				PUSH_BOOLEAN(f_aux_value1 __OVERR_OP__ STK_VALUE_TO_INT(stk_result_op2));\
			}\
	}\
	else if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){\
			COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
			COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
			PUSH_BOOLEAN(f_aux_value1 __OVERR_OP__ f_aux_value2);\
	}\
	else if((stk_result_op1->properties&stk_result_op2->properties) == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){\
		PUSH_BOOLEAN(ZS_STRCMP(STK_VALUE_TO_STRING(stk_result_op1), __OVERR_OP__ ,STK_VALUE_TO_STRING(stk_result_op2)));\
	}else{\
		if(!applyMetamethod(\
					 calling_object\
					,calling_function\
					,instruction\
					,STR(__OVERR_OP__)\
					, __METAMETHOD__\
					,stk_result_op1\
					,stk_result_op2\
		)){\
			RETURN_ERROR;\
		}\
	}\
}

#define PROCESS_LOGIC_OPERATION(__OVERR_OP__)\
{\
	unsigned short properties = GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN){\
		PUSH_BOOLEAN(STK_VALUE_TO_BOOL(stk_result_op1) __OVERR_OP__ STK_VALUE_TO_BOOL(stk_result_op2));\
	}else{\
		PRINT_DUAL_ERROR_OP(STR(__OVERR_OP__));\
	}\
}

#define PROCESS_BINARY_OPERATION(__OVERR_OP__, __METAMETHOD__)\
{\
	unsigned short properties = GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){\
		PUSH_INTEGER(STK_VALUE_TO_INT(stk_result_op1) __OVERR_OP__ STK_VALUE_TO_INT(stk_result_op2));\
	}else{\
		if(!applyMetamethod(\
						 calling_object\
						,calling_function\
						,instruction\
						,STR(__OVERR_OP__)\
						, __METAMETHOD__\
						,stk_result_op1\
						,stk_result_op2\
		)){\
			RETURN_ERROR;\
		}\
	}\
}

#define PERFORM_PRE_POST_OPERATOR(stk_var, __OPERATOR__) \
{\
	void **ref=(void **)(&((stk_var)->stk_value));\
	if(stk_var->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){\
		ref=(void **)((stk_var)->var_ref);\
	}\
	switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES((stk_var)->properties)){\
	case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:\
			(*((int *)(ref)))__OPERATOR__;\
			break;\
	case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:\
			(*((float *)(ref)))__OPERATOR__;\
			break;\
	default:\
		writeError(SFI_GET_FILE_LINE(calling_function,instruction)," Cannot perform pre/post operator (%s)",stk_var->toString());\
		RETURN_ERROR;\
		break;\
	}\
}


#define assignStackVar(stk_dst, stk_src) \
{\
	ScriptVar *script_var=NULL;\
	std::string *aux_str=NULL;\
	void *copy_aux=NULL;/*copy aux in case of the var is c and primitive (we have to update stk_value on save) */\
	void **src_ref=&stk_src->stk_value;\
	void **dst_ref=&stk_dst->stk_value;\
	if(stk_src->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){\
		src_ref=(void **)((stk_src)->var_ref);\
	}\
	if(stk_dst->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){\
		if(!STK_VALUE_IS_FLOAT_OR_INT(stk_src) && STK_VALUE_IS_FLOAT_OR_INT(stk_dst)){\
			if(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_src->properties) != GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_dst->properties)\
			){\
				writeError(SFI_GET_FILE_LINE(calling_function,instruction),"different types! dst var is native (i.e embedd C++) and cannot change its type. dest and src must be equals",SFI_GET_SYMBOL_NAME(calling_function,instruction));\
				RETURN_ERROR;\
			}else{\
				if(\
					(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_src->properties) == MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR)\
				){\
						ZS_PRINT_INFO("Assign native C scriptvar is not allowed to avoid memory leaks. Define '=' operator in order to make the proper operation.");\
						RETURN_ERROR;\
				}\
			}\
		}\
		dst_ref=(void **)((stk_dst)->var_ref);\
		copy_aux=&((stk_dst)->stk_value);\
	}\
	unsigned short type_var=stk_src->properties;\
	unsigned short runtime_var=0; /* there's no reason to heredate runtime_props ?!? GET_MSK_STACK_ELEMENT_PROPERTY_RUNTIME(type_var);*/\
	if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED){\
		stk_dst->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED;\
	}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL){\
		stk_dst->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL;\
	}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){\
		stk_dst->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER;\
		*((int *)dst_ref)=*((int *)src_ref);\
		if(copy_aux!=NULL)(*(int *)copy_aux)=*((int *)src_ref);\
	}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){\
		stk_dst->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT;\
		*((float *)dst_ref)=*((float *)src_ref);\
		if(copy_aux!=NULL)(*(float *)copy_aux)=*((float *)src_ref);\
	}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN){\
		stk_dst->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN;\
		*((bool *)dst_ref)=*((bool *)src_ref);\
		if(copy_aux!=NULL)(*(bool *)copy_aux)=*((bool *)src_ref);\
	}else if(type_var  &  MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION){\
		*stk_dst={	stk_src->stk_value,\
					NULL,\
					(unsigned short)(runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION)\
					};\
	}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){\
		if(stk_dst->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C){\
			*((std::string *)stk_dst->var_ref)=((const char *)stk_src->stk_value);/* Assign std::string */\
			stk_dst->stk_value=(void *)(((std::string *)stk_dst->var_ref)->c_str());/* Because std::string assignment implies reallocs ptr char it changes, so reassing const char pointer */\
		}else{\
			if(((stk_dst->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)==0) || (stk_dst->var_ref==NULL)){/* Generates a std::string var */  \
				script_var= NEW_STRING_VAR;\
				stk_dst->var_ref=script_var;\
				aux_str=&(((ScriptVarString *)script_var)->str_value);\
				stk_dst->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR;\
				script_var->initSharedPtr(true);\
			}\
			(*aux_str)=((const char *)stk_src->stk_value);\
			stk_dst->stk_value=(void *)aux_str->c_str();/* Because std::string assignment implies reallocs ptr char it changes, so reassing const char pointer */ \
		}\
	}else if(type_var & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){\
		script_var=(ScriptVar *)stk_src->var_ref;\
		stk_dst->properties=runtime_var | MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR;\
		stk_dst->stk_value=NULL;\
		stk_dst->var_ref=script_var;\
		if((stk_dst->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_THIS) !=  MSK_STACK_ELEMENT_PROPERTY_IS_VAR_THIS){\
			sharePointer(script_var->ptr_shared_pointer_node);\
		}\
	}else{\
		writeError(SFI_GET_FILE_LINE(calling_function,instruction),"(internal) cannot determine var type %i",GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stk_src->properties));\
		RETURN_ERROR;\
	}\
	if(copy_aux!=NULL)stk_dst->properties|=MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C;\
}

#define SHARE_LIST_INSERT(list,_node){\
		if(list.first == NULL){\
			_node->previous=_node->next= list.last = list.first =_node;\
		}\
		else{\
			list.last->next=_node;\
			_node->previous=list.last;\
			list.last=_node;\
			list.first->previous=list.last;\
			list.last->next=list.first;\
		}\
	}

#define SHARE_LIST_DEATTACH(list,_node) \
{\
		if(_node->next == _node){/*one  node: trivial ?*/\
			list.first = list.last = _node->next = _node->previous=NULL;\
		}else{/* >1 node */\
			PInfoSharedPointerNode previous=_node->previous;\
			PInfoSharedPointerNode next=_node->next;\
			if(_node==list.first){\
				list.first = next;\
			}\
			else if(_node==list.last){\
				list.last = previous;\
			}\
			previous->next = next;\
			next->previous = previous;\
		}\
}

#define BYTE_CODE_PUSH_SCOPE(_index,_ptr_info_function, _ptr_local_var,_properties) {\
	if(current_scope_info_ptr >=  max_scope_info){writeError(NULL,0,"reached max scope"); RETURN_ERROR;}\
	*current_scope_info_ptr++={(short)(_index),_ptr_info_function,_ptr_local_var,_properties};\
}


#define CALL_GC SHARED_LIST_DESTROY(zero_shares[idx_current_stack])

namespace zetscript{

	void  writeError(const char *filename, int line, const  char  *string_text, ...);


	void VirtualMachine::doStackDump(){
		// derefer all variables in all scopes (except main )...
		while(scope_info<(current_scope_info_ptr)){
			popScopeCall(idx_current_stack,NULL,0);
		}
		idx_current_stack=0;
	}

	VirtualMachine::VirtualMachine(ZetScript *_zs){

		//-----------------------------------------------------------
		// set memory manager

		memset(zero_shares,0,sizeof(zero_shares));
		memset(shared_var,0,sizeof(shared_var));
		//memset(stack,0,sizeof(stack));

		StackElement *aux=stack;

		for(int i=0; i < VM_LOCAL_VAR_MAX_STACK;i++){
			*aux++={0,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
		}

		idx_current_stack=0;
		//------------------

		f_return_value=0;
		s_return_value="unknow";

		cancel_execution=false;

		stk_current=NULL;
		current_scope_info_ptr = scope_info;

		f_aux_value1=0;
		f_aux_value2=0;

		custom_error = "unknow_error";

		idx_last_statment=0;

		str_last=&stk_string[VM_MAX_AUX_STRINGS-1]; // aux values for std::string ...
		str_current=NULL;

		max_scope_info = &scope_info[VM_MAX_SCOPES-1];

		//current_ast_node_call_c_function=-1;

		main_function_object = NULL;

		script_functions = NULL;
		//vec_ast_node = NULL;

		size_vec_script_function_object_node = 0;
		//size_vec_ast_node = 0;
		current_foreach=NULL;
		current_call_c_function = NULL;
		n_globals=0;
		zs=_zs;
		script_function_factory=this->zs->getScriptFunctionFactory();
		script_class_factory=this->zs->getScriptClassFactory();

	}

	#ifdef  __ZETSCRIPT_VERBOSE_MESSAGE__

	#define print_vm_cr ZS_PRINT_INFO
	#else
	#define print_vm_cr(s,...)
	#endif


	//============================================================================================================================================
	// POINTER MANANAGER

	PInfoSharedPointerNode VirtualMachine::newSharedPointer(ScriptVar *_var_ptr){
		//int index = VirtualMachine::getFreeCell();
		PInfoSharedPointerNode _node = (PInfoSharedPointerNode)malloc(sizeof(tInfoSharedPointerNode));
		_node->data.n_shares=0;
		_node->data.shared_ptr=_var_ptr;
		_node->currentStack = idx_current_stack;

		// insert node into shared nodes ...
		SHARE_LIST_INSERT(zero_shares[idx_current_stack],_node);
		//zero_shares[idx_current_stack].InsertNode(_node);
		return _node;
	}


	bool VirtualMachine::sharePointer(PInfoSharedPointerNode _node){

		unsigned char *n_shares = &_node->data.n_shares;

		bool move_to_shared_list=*n_shares==0;

		if(*n_shares >= MAX_SHARES_VARIABLE){
			THROW_RUNTIME_ERROR(zs_strutils::format("MAX SHARED VARIABLES (Max. %i)",MAX_SHARES_VARIABLE));
			return false;

		}

		(*n_shares)++;

		if(move_to_shared_list){

			// Mov to shared pointer...
			SHARE_LIST_DEATTACH(zero_shares[_node->currentStack],_node);
			// update current stack due different levels from functions!
			_node->currentStack=idx_current_stack;
			SHARE_LIST_INSERT(shared_var[idx_current_stack],_node);
		}

		return true;

	}

	void VirtualMachine::unrefSharedScriptVar(PInfoSharedPointerNode _node, bool remove_if_0){

		unsigned char *n_shares = &_node->data.n_shares;
		if(*n_shares > 0){
			if(--(*n_shares)==0){ // mov back to 0s shares (candidate to be deleted on GC check)

				SHARE_LIST_DEATTACH(shared_var[_node->currentStack],_node);

				if(remove_if_0){ // remove node and data instead...
					delete _node->data.shared_ptr;
					free(_node);
				}
				else{ // insert into zero array.. if not referenced anymore will be removed by REMOVE_0_SHARED
					// update current stack due different levels from functions!
					_node->currentStack=idx_current_stack;
					SHARE_LIST_INSERT(zero_shares[idx_current_stack],_node);
				}
			}
		}
	}

	const ScriptFunction * VirtualMachine::getCurrent_C_FunctionCall(){
		return current_call_c_function;
	}

	StackElement * VirtualMachine::getStackElement(unsigned int idx_glb_element){
		ScriptFunction  *main_function = MAIN_FUNCTION(this);//GET_SCRIPT_FUNCTION(this,0);
		if(idx_glb_element < main_function->local_variable.size()){
			return &stack[idx_glb_element];
		}

		return NULL;
	}

	void VirtualMachine::setError(const char *str){
		writeError("custom_user",-1,str);
		custom_error = str;
		cancel_execution = true;
	}

	void VirtualMachine::cancelExecution(){
		custom_error=NULL;
		cancel_execution = true;
	}

	void VirtualMachine::destroyCache(){
		main_function_object=NULL;
		if(script_functions!= NULL){
			free(script_functions);
			script_functions=NULL;
			size_vec_script_function_object_node=0;
		}
	}

	void VirtualMachine::buildCache(){

		destroyCache();

		main_function_object = MAIN_FUNCTION(this);
		std::vector<ScriptFunction *> *vec_script_function_object_node_aux=script_function_factory->getScriptFunctions();
		size_vec_script_function_object_node=vec_script_function_object_node_aux->size();
		script_functions=(ScriptFunction **)malloc(sizeof(ScriptFunction *)*size_vec_script_function_object_node);
		for(unsigned i=0; i < size_vec_script_function_object_node; i++){
			script_functions[i]=vec_script_function_object_node_aux->at(i);
		}
	}

	void VirtualMachine::addGlobalVar(const StackElement & stk){

		if(n_globals < VM_LOCAL_VAR_MAX_STACK){
			stack[n_globals++]=stk;
		}else{
			writeError(NULL,-1,"Max stack element over limit (%i)",VM_LOCAL_VAR_MAX_STACK);
		}
	}

	void VirtualMachine::clearGlobalVars(){
		ScriptFunction  *main_function = MAIN_FUNCTION(this);

		// zero shares have a late loading so it can be null at first time...
		if(zero_shares == NULL){
			return;
		}

		if(n_globals!=(int)main_function->local_variable.size()){
			THROW_RUNTIME_ERROR("n_globals != main variables");
			return;
		}

		bool end=false;
		for(int i =  (int)(main_function->local_variable.size())-1; i >= 0 && !end; i--){
			//switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(ptr_ale->properties)){
			//case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
			end=(main_function->local_variable[i].symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTY_C_OBJECT_REF;
			if(!end){
				StackElement *ptr_ale =&stack[i];
				ScriptVar *var = NULL;

				if(ptr_ale->properties &MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
					var =((ScriptVar *)(ptr_ale->var_ref));
					if(var){
						if(var->ptr_shared_pointer_node != NULL){
							var->unrefSharedPtr();
						}
					}
				}
			}
		}

		removeEmptySharedPointers(0,NULL);

		// clear all stack variables
		StackElement *aux=stack;
		for(int i=0; i < VM_LOCAL_VAR_MAX_STACK;i++){
			*aux++={0,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
		}

		memset(zero_shares,0,sizeof(zero_shares));
		memset(shared_var,0,sizeof(shared_var));
		idx_current_stack=0;
	}

	StackElement  VirtualMachine::callFunction(
			 ScriptFunction *calling_function,
			 ScriptVar *this_object,
			 bool & error,
			const std::vector<StackElement> & arg
			){

		StackElement stk_result={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

		if(calling_function==NULL){
			RETURN_ERROR;
		}

		if(idx_current_stack==0){ // set stack and Init vars for first call...

			if(main_function_object->instruction == NULL){ // no statments
				RETURN_ERROR;
			}
			cancel_execution=false;

			stk_current=stack;
			//*stk_current={MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED,0,0}; // ini first op

			if(calling_function->idx_script_function != 0){ // calls script function from C : preserve stack space for global vars
				stk_current=&stack[main_function_object->local_variable.size()];
			}
			current_foreach=&stk_foreach[0];
		}

		int n_arg=0;
		if(arg.size()>0){ // pass parameters...

			for(unsigned i = 0; i < arg.size(); i++){
				*stk_current++=arg[i];
			}
			//advance idxBaseStk...
			n_arg=arg.size();
		}

		// Script function starts here.... later script function can call c++ function, but once in c++ function is not possible by now call script function again.
		StackElement info=callFunctionInternal(
				calling_function,
				this_object,
				error,
				stk_current,
				NULL,
				n_arg);

		if(error){ // it was error so reset stack and stop execution ? ...
			doStackDump();
		}

		return info;
	}

	StackElement  * VirtualMachine::getLastStackValue(){
		return (stk_current-1);
	}


	VirtualMachine::~VirtualMachine(){
		destroyCache();
	}
}


#include "VirtualMachine_call_function_c.cpp"
#include "VirtualMachine_call_function.cpp"
