/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#define __ZS_STK_VAR_COPY__			stk_aux1

// explains whether stk is this or not. Warning should be given as value and not as ptr
#define ZS_PRINT_DUAL_ERROR_OP(c)\
	ZS_VM_ERROR("cannot perform operator '%s' %s '%s'. Check whether op1 and op2 are same type, or type implements the metamethod",\
		data->zs->stackElementToTypeOfString(ZS_VM_STR_AUX_PARAM_0,stk_result_op1),\
		c,\
		data->zs->stackElementToTypeOfString(ZS_VM_STR_AUX_PARAM_1,stk_result_op2));\

#define ZS_PRINT_ERROR_OP(c)\
ZS_VM_ERROR("cannot perform preoperator %s'%s'. Check whether op1 implements the metamethod",\
	c,\
	stk_to_typeof_str(ZS_VM_STR_AUX_PARAM_0,data->zs,stk_result_op1));\
	return NULL;


#define ZS_CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(_stk_return, _n_return) \
	for(int i=0; i < _n_return; i++){\
		StackElement *stk_ret = _stk_return+i;\
		if(stk_ret->properties & STK_PROPERTY_SCRIPT_OBJECT){\
			ScriptObject *sv=(ScriptObject *)stk_ret->value;\
			if(sv->shared_pointer == NULL){\
				if(!vm_create_shared_script_object(_vm,sv)){\
					return false;\
				}\
			}\
		}\
	}

#define ZS_VM_EXTRACT_FUNCTION_INFO\
	{ /* get elements from type */ \
		Symbol *symbol = (Symbol *)(*(stk_elements_builtin_ptr+i));\
		if(symbol->properties & SYMBOL_PROPERTY_FUNCTION){ \
			irfs = (ScriptFunction *)symbol->ref_ptr;\
			if(irfs->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION ){\
				start_param=2;\
			}\
		}\
	}\
	if(irfs==NULL) continue;

#define ZS_VM_INNER_CALL(_so_object,_so_function,_n_args,_name)\
	if(vm_inner_call( \
			_vm \
			,_script_function \
			,instruction \
			,_so_object \
			,_so_function \
			,_n_args \
	)==false){\
		goto lbl_exit_function;\
	}\

#define ZS_VM_PROPERTY_CALL_FROM_NATIVE	0x1

#define ZS_SET_INTEGER_RETURN(i)			CURRENT_VM->setIntegerReturnValue(i)
#define ZS_SET_BOOLEAN_RETURN(b) 			CURRENT_VM->setBooleanReturnValue(b)
#define ZS_SET_FLOAT_RETURN(f)   			CURRENT_VM->setFloatReturnValue(f)

#define ZS_NO_PARAMS zs_vector<StackElement>{}
#define ZS_VM_FUNCTION_TYPE std::function<ObjectScriptObject * (const zs_vector<ObjectScriptObject *> & param)>

#define ZS_VM_EXECUTE(vm,o,f,stk,n)		vm_execute(vm,o,f,stk,n,0,__FILE__,__LINE__)

#define ZS_VM_ERROR(_str_error,...)	\
	vm_set_file_line_error(\
		_vm \
		,SFI_GET_FILE(_script_function,instruction)\
		,SFI_GET_LINE(_script_function,instruction)\
		,_str_error\
		, __VA_ARGS__\
	);

#define ZS_VM_ERRORF(_str_error)					ZS_VM_ERROR(_str_error,NULL)

#define ZS_VM_ERROR_AND_RET(_str_error,...)	\
	vm_set_file_line_error(\
		_vm \
		,SFI_GET_FILE(_script_function,instruction)\
		,SFI_GET_LINE(_script_function,instruction)\
		,_str_error\
		, __VA_ARGS__\
	);\
	return;

#define ZS_VM_ERROR_AND_RETF(_str_error)			ZS_VM_ERROR_AND_RET(_str_error,NULL)

#define ZS_VM_STOP_EXECUTE(_str_error,...)	\
	vm_set_file_line_error(\
		_vm \
		,SFI_GET_FILE(_script_function,instruction)\
		,SFI_GET_LINE(_script_function,instruction)\
		,_str_error\
		, __VA_ARGS__\
	);\
	goto lbl_exit_function;

#define ZS_VM_STOP_EXECUTEF(_str_error) \
	ZS_VM_STOP_EXECUTE(_str_error,NULL)

#define ZS_VM_SET_USER_ERROR(_vm,_str_error,...)	\
	vm_set_file_line_error(_vm,__FILE__,__LINE__, _str_error, __VA_ARGS__)

#define ZS_VM_SET_USER_ERRORF(_vm,_str_error) \
	ZS_VM_SET_USER_ERROR(_vm,_str_error,NULL)

#define ZS_IDX_VM_CURRENT_SCOPE_FUNCTION 		(data->vm_current_scope_function-data->vm_scope_function)
#define ZS_VM_SCOPE_FUNCTION_MAIN 				(data->vm_scope_function+0)
#define ZS_VM_SCOPE_FUNCTION_FIRST 				(data->vm_scope_function+1)
#define ZS_VM_CURRENT_SCOPE_FUNCTION			(data->vm_current_scope_function-1)
#define ZS_VM_CURRENT_SCOPE_BLOCK				(ZS_VM_CURRENT_SCOPE_FUNCTION->current_scope_block-1)
#define ZS_VM_MAIN_SCOPE_BLOCK					(ZS_VM_SCOPE_FUNCTION_MAIN->scope_block)

#define ZS_VM_STR_AUX_PARAM_0					(data->vm_str_aux[0])
#define ZS_VM_STR_AUX_PARAM_1					(data->vm_str_aux[1])

#define ZS_VM_MAIN_ERROR(_error,_stk, _metamethod) \
		vm_print_main_error(\
				_vm\
				,_script_function\
				,instruction\
				,_error\
				,_stk\
				,_metamethod\
		);\
		goto lbl_exit_function;

#define ZS_VM_STK_IS_GLOBAL(stk_var) \
		((stk_var - data->vm_stack)< data->main_function_object->local_variables->size())

#define ZS_VM_CHECK_CONTAINER_FOR_SLOT(_container) \
	(\
		(_container->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_ARRAY) \
				   || \
		(_container->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_OBJECT) \
					|| \
		(((_container->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_CLASS) && _container->isNativeObject())==false) \
	)

namespace zetscript{

	class ZetScript;

	typedef enum{
		VM_MAIN_ERROR_LOAD_PROPERTIES_ERROR=0
		,VM_MAIN_ERROR_METAMETHOD_OPERATION_MEMBER_PROPERTY_NOT_IMPLEMENTED
		,VM_MAIN_ERROR_METAMETHOD_OPERATION_SYMBOL_NOT_IMPLEMENTED
		,VM_MAIN_ERROR_CANNOT_FIND_SYMBOL
	}VM_MainError;

	typedef struct{
		const char *file;
		int line;
		ScriptObject * script_object;
	}InfoLifetimeObject;

	struct VirtualMachineData{

		 bool									vm_error,vm_error_max_stack_reached;
		 zs_string								vm_error_description;
		 zs_string								vm_error_file;
		 int 									vm_error_line;
		 zs_string 								vm_error_callstack_str;
		 char									vm_str_metamethod_aux[100];
		 char									vm_str_aux[2][100];
		 VM_ScopeFunction					*	vm_current_scope_function;
		 VM_ScopeFunction						vm_scope_function[VM_FUNCTION_CALL_MAX];


		 StackElement     						vm_stack[VM_STACK_MAX];
		 zs_vector<InfoLifetimeObject *>		lifetime_object;

		 // global vars show be initialized to stack array taking the difference (the registered variables on the main function) - global_vars ...
		StackElement 						*	vm_stk_current;
		ScriptFunction  					*	main_function_object;
		ScriptType 							*	main_class_object;

		const ScriptFunction 				*	current_call_c_function;
		ZetScript *zs;
		ScriptFunctionFactory 				*	script_function_factory;
		ScriptTypeFactory 					*	script_type_factory;
		ScopeFactory 						*	scope_factory;
		zs_map_int								cyclic_container_instances;

		VirtualMachineData(ZetScript *_zs){
			memset(&vm_stack,0,sizeof(vm_stack));

			vm_stk_current=NULL;
			vm_current_scope_function = vm_scope_function;

			main_function_object = NULL;
			main_class_object=NULL;

			current_call_c_function = NULL;
			zs=_zs;

			script_function_factory=NULL;
			script_type_factory=NULL;
			scope_factory=NULL;
			vm_error=false;
			vm_error_line=-1;
			vm_error_max_stack_reached=false;

			// each push scope from main is done from block scope 1
			vm_scope_function[0].first_scope_block=&vm_scope_function[0].scope_block[1];
		}
	};

	bool vm_call_metamethod(
		VirtualMachine			*	_vm
		,ScriptFunction 		*	_script_function
		,Instruction 			*	_instruction
		,ByteCodeMetamethod 		_byte_code_metamethod
		,StackElement 			*	_stk_result_op1
		,StackElement 			*	_stk_result_op2
		, bool 						_is_static=true
		, bool 						_is_je_case=false
	);

	bool vm_inner_call(
		VirtualMachine 			*	_vm
		,ScriptFunction			* 	_script_function
		,Instruction			* 	_instruction
		,ScriptObject 			*	_script_object
		,ScriptFunction 		*	_script_function_to_call
		,int 						_n_args
	);

	bool  vm_insert_shared_node(
		VirtualMachine	 		*	_vm
		, InfoSharedList 		*	 list
		, InfoSharedPointerNode *	_node
	);

	bool vm_byte_code_new_object_by_value(
		VirtualMachine 		*vm
		,ScriptFunction *calling_function
		,Instruction *instruction
	);

	void vm_print_main_error(
		VirtualMachine 			*	_vm
		,ScriptFunction 		*	_script_function
		,Instruction 			*	_instruction
		,VM_MainError 				_error
		,StackElement 			*	_stk=NULL
		,ByteCodeMetamethod 		_byte_code_metamethod=BYTE_CODE_METAMETHOD_INVALID
	);

	void vm_push_stk_boolean_equal_strings(
		VirtualMachine			*	_vm
		, StackElement 			*	_stk1
		, StackElement 			*	_stk2
		, ByteCodeMetamethod 		_byte_code_metamethod
	);

	ScriptFunction * vm_find_native_function(
		VirtualMachine 		*	_vm
		,ScriptType 		*	_class_obj // if NULL is MainClass
		,ScriptFunction 	*	_script_function
		,Instruction 		* 	_instruction // call instruction
		,bool 					_is_constructor
		,const zs_string 	& 	_symbol_to_find
		,StackElement 		*	_stk_arg
		,uint8_t	 			_n_args
	);

	void vm_assign_container_slot(
		VirtualMachine *_vm
		, ContainerSlot *_container_slot
		, ContainerScriptObject *_src_container_ref
	);

	void vm_deref_cyclic_references(
		VirtualMachine *_vm
	);

	void vm_remove_container_instance_cyclic_references_map(
		  VirtualMachine *_vm
		, ContainerScriptObject *_container_slot
	);

	zs_map_int *vm_get_cyclic_container_instances(
			VirtualMachine *_vm
	);
}




