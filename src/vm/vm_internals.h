/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#define __STK_VAR_COPY__			stk_aux1

// explains whether stk is this or not. Warning should be given as value and not as ptr
#define PRINT_DUAL_ERROR_OP(c)\
	VM_ERROR("cannot perform operator '%s' %s '%s'. Check whether op1 and op2 are same type, or type implements the metamethod",\
		stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op1),\
		c,\
		stk_to_typeof_str(VM_STR_AUX_PARAM_1,data->zs,stk_result_op2));\

#define PRINT_ERROR_OP(c)\
VM_ERROR("cannot perform preoperator %s'%s'. Check whether op1 implements the metamethod",\
	c,\
	stk_to_typeof_str(VM_STR_AUX_PARAM_0,data->zs,stk_result_op1));\
	return NULL;


#define CREATE_SHARE_POINTER_TO_ALL_RETURNING_OBJECTS(_stk_return, _n_return,_with_share) \
		for(int i=0; i < _n_return; i++){\
			StackElement *stk_ret = _stk_return+i;\
			if(stk_ret->properties & STK_PROPERTY_SCRIPT_OBJECT){\
				ScriptObject *sv=(ScriptObject *)stk_ret->value;\
				if(sv->shared_pointer == NULL){\
					if(!vm_create_shared_script_object(_vm,sv)){\
						return false;\
					}\
					if(_with_share==true){\
						if(vm_share_script_object(_vm,sv)==false){\
							return false;\
						}\
					}\
				}\
			}\
		}

#define VM_EXTRACT_FUNCTION_INFO\
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

#define VM_INNER_CALL(_so_object,_so_function,_n_args,_name)\
	if(vm_inner_call( \
			_vm \
			,_calling_function \
			,instruction \
			,_so_object \
			,_so_function \
			,_n_args \
	)==false){\
		goto lbl_exit_function;\
	}\

#define VM_PROPERTY_CALL_FROM_NATIVE	0x1

#define SET_INTEGER_RETURN(i)			CURRENT_VM->setIntegerReturnValue(i)
#define SET_BOOLEAN_RETURN(b) 			CURRENT_VM->setBooleanReturnValue(b)
#define SET_FLOAT_RETURN(f)   			CURRENT_VM->setFloatReturnValue(f)

#define NO_PARAMS zs_vector<StackElement>{}
#define ZS_VM_FUNCTION_TYPE std::function<ScriptObjectObject * (const zs_vector<ScriptObjectObject *> & param)>

#define VM_EXECUTE(vm,o,f,stk,n)		vm_execute(vm,o,f,stk,n,0,__FILE__,__LINE__)

#define VM_ERROR(_str_error,...)	\
	vm_set_file_line_error(\
		_vm \
		,SFI_GET_FILE(_calling_function,instruction)\
		,SFI_GET_LINE(_calling_function,instruction)\
		,_str_error\
		, __VA_ARGS__\
	);

#define VM_ERRORF(_str_error)					VM_ERROR(_str_error,NULL)

#define VM_ERROR_AND_RET(_str_error,...)	\
	vm_set_file_line_error(\
		_vm \
		,SFI_GET_FILE(_calling_function,instruction)\
		,SFI_GET_LINE(_calling_function,instruction)\
		,_str_error\
		, __VA_ARGS__\
	);\
	return;

#define VM_ERROR_AND_RETF(_str_error)			VM_ERROR_AND_RET(_str_error,NULL)

#define VM_STOP_EXECUTE(_str_error,...)	\
	vm_set_file_line_error(\
		_vm \
		,SFI_GET_FILE(_calling_function,instruction)\
		,SFI_GET_LINE(_calling_function,instruction)\
		,_str_error\
		, __VA_ARGS__\
	);\
	goto lbl_exit_function;

#define VM_STOP_EXECUTEF(_str_error) \
	VM_STOP_EXECUTE(_str_error,NULL)

#define VM_SET_USER_ERROR(_vm,_str_error,...)	\
	vm_set_file_line_error(_vm,__FILE__,__LINE__, _str_error, __VA_ARGS__)

#define VM_SET_USER_ERRORF(_vm,_str_error) \
	VM_SET_USER_ERROR(_vm,_str_error,NULL)

#define IDX_VM_CURRENT_SCOPE_FUNCTION 	(data->vm_current_scope_function-data->vm_scope_function)
#define VM_SCOPE_FUNCTION_MAIN 			(data->vm_scope_function+0)
#define VM_SCOPE_FUNCTION_FIRST 		(data->vm_scope_function+1)
#define VM_CURRENT_SCOPE_FUNCTION		(data->vm_current_scope_function-1)
#define VM_CURRENT_SCOPE_BLOCK			(VM_CURRENT_SCOPE_FUNCTION->current_scope_block-1)

#define VM_STR_AUX_PARAM_0				(data->vm_str_aux[0])
#define VM_STR_AUX_PARAM_1				(data->vm_str_aux[1])

#define VM_MAIN_ERROR(_error,_stk, _metamethod) \
		vm_print_main_error(\
				_vm\
				,_calling_function\
				,instruction\
				,_error\
				,_stk\
				,_metamethod\
		);\
		goto lbl_exit_function;

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


		 bool				vm_error,vm_error_max_stack_reached;
		 zs_string			vm_error_str;
		 zs_string			vm_error_file;
		 int 				vm_error_line;
		 zs_string 			vm_error_callstack_str;
		 char				vm_str_metamethod_aux[100];
		 char				vm_str_aux[2][100];
		 VM_ScopeFunction	*vm_current_scope_function;
		 VM_ScopeFunction	vm_scope_function[VM_FUNCTION_CALL_MAX];


		 StackElement     	vm_stack[VM_STACK_MAX];
		 zs_vector<InfoLifetimeObject *>			lifetime_object;

		 // global vars show be initialized to stack array taking the difference (the registered variables on the main function) - global_vars ...
		StackElement *stk_vm_current;
		ScriptFunction  *main_function_object;
		ScriptType *main_class_object;

		int idx_last_statment;
		const ScriptFunction *current_call_c_function;
		ZetScript *zs;
		ScriptFunctionFactory 	*script_function_factory;
		ScriptTypeFactory 		*script_type_factory;
		ScopeFactory 			*scope_factory;

		StackElement stk_aux;

		VirtualMachineData(ZetScript *_zs){
			memset(&vm_stack,0,sizeof(vm_stack));

			stk_vm_current=NULL;
			vm_current_scope_function = vm_scope_function;

			idx_last_statment=0;

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
			,ScriptFunction 		*	_calling_function
			,Instruction 			*	_instruction
			,ByteCodeMetamethod 		_byte_code_metamethod
			,StackElement 			*	_stk_result_op1
			,StackElement 			*	_stk_result_op2
			, bool 						_is_static=true
			, bool 						_is_je_case=false
		);

		bool vm_inner_call(
			VirtualMachine 			*	_vm
			,ScriptFunction			* 	_calling_function
			,Instruction			* 	_instruction
			,ScriptObject 			*	_script_object
			,ScriptFunction 		*	_script_function
			,int 						_n_args
		);

		/*void vm_pop_scope(
			VirtualMachine 			*	_vm
		);*/
		// defer all local vars


		bool  vm_insert_shared_node(
			VirtualMachine	 		*	_vm
			, InfoSharedList 		*	 list
			, InfoSharedPointerNode *	_node
		);

		inline bool vm_deattach_shared_node(VirtualMachine *vm, InfoSharedList * list, InfoSharedPointerNode *_node){

			if(list == NULL) return true;

			if(_node->next == NULL || _node->previous == NULL){
				VM_SET_USER_ERRORF(vm," Internal error: An already deattached node");
				return false;
			}

			if((_node->previous == _node) && (_node->next == _node)){ // 1 single node...
				list->last=list->first=NULL;
			}
			else{ // dettach and attach next...
				// [1]<->[2]<-> ...[P]<->[C]<->[N]...[M-1]<->[M]
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

		inline void vm_remove_empty_shared_pointers(VirtualMachine *vm,VM_ScopeBlock *scope_block){
			InfoSharedList *list=&scope_block->unreferenced_objects;//&data->zero_shares[idx_call_stack];
			InfoSharedPointerNode *next_node=NULL,*current=list->first;
			//bool check_empty_shared_pointers=false;

			if(current != NULL){
				bool finish=false;
				do{
					next_node=current->next;
					finish=next_node ==list->first;

					if(!vm_deattach_shared_node(vm,list,current)){
						return;
					}

					delete current->data.ptr_script_object_shared;
					current->data.ptr_script_object_shared=NULL;
					free(current);

					current=next_node;

				}while(!finish);
			}
		}


		bool vm_byte_code_new_object_by_value(
			VirtualMachine 		*vm
			,ScriptFunction *calling_function
			,Instruction *instruction
		);

		void vm_print_main_error(
			VirtualMachine 			*	_vm
			,ScriptFunction 		*	_calling_function
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
			,ScriptFunction 	*	_calling_function
			,Instruction 		* 	_instruction // call instruction
			,bool 					_is_constructor
			,const zs_string 	& 	_symbol_to_find
			,StackElement 		*	_stk_arg
			,unsigned char 			_n_args
		);

		inline void vm_pop_scope(VirtualMachine *vm)
		{\
			VirtualMachineData *data=(VirtualMachineData *)vm->data;
			VM_ScopeBlock *scope_block=--VM_CURRENT_SCOPE_FUNCTION->current_scope_block;\
			Scope *scope=scope_block->scope;\
			StackElement         * stk_local_vars	=VM_CURRENT_SCOPE_FUNCTION->stk_local_vars;\
			zs_vector<Symbol *> *scope_symbols=scope->symbol_variables;\
			int count=(int)scope_symbols->size();\
			if(count > 0){\
				StackElement *stk_local_var=stk_local_vars+scope_symbols->items[0]->idx_position;\
				while(count--){\
					if((stk_local_var->properties & STK_PROPERTY_SCRIPT_OBJECT)){\
						ScriptObject *so=(ScriptObject *)(stk_local_var->value);\
						if(so != NULL && so->shared_pointer!=NULL){\
							 vm_unref_shared_script_object(vm,so,NULL);\
						}\
					}\
					STK_SET_UNDEFINED(stk_local_var);\
					stk_local_var++;\
				}\
			}\
			vm_remove_empty_shared_pointers(vm,scope_block);\
		}
}




