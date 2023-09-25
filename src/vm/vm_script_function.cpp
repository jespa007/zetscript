/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#define __STR_PTR_SYMBOL_TO_FIND__ 	str_symbol_aux1
#define __STR_PTR_END_CLASS__ 		str_symbol_aux2

#include "vm_operation.h"
#include "vm_operation_st.h"

#include "vm_call_metamethod.cpp"
#include "vm_call.cpp"
#include "vm_container.cpp"
#include "vm_iterator.cpp"
#include "vm_store.cpp"
#include "vm_new_object.cpp"


namespace zetscript{


	bool vm_unref_script_object_for_ret(
			VirtualMachine *vm
			, StackElement *stk_var
	);

	void vm_throw_error_cannot_find_symbol(
			VirtualMachine *_vm
			,ScriptFunction *_script_function
			,Instruction *_instruction
	);

	void vm_delete_object(
		VirtualMachine *vm
	);

	void vm_new_string_object(
			VirtualMachine 	*	_vm
			,Instruction 	*	_instruction
	);

	void vm_execute_script_function(
			VirtualMachine			* _vm,
			ScriptObject			* _this_object,
			ScriptFunction 			* _script_function,
			StackElement 		  	* _stk_local_var
	    ){
		// This is the main script function body, and due there's a lot of code, is important to reuse as many variables as possible
		// and not abuse of temporal variables (i.e return values from objects), keeping HEAP memory region low to avoid StackOverflow
		// in recursion
		//
		// generic vars for management
		VirtualMachineData 		*	data = (VirtualMachineData*)_vm->data;
		Instruction 			*	instruction=_script_function->instructions; // starting instruction
		ScriptObject 			*	so_aux=NULL;
		StackElement 			*	stk_result_op1=NULL;
		StackElement 			*	stk_result_op2=NULL;
		StackElement 				stk_aux1,stk_aux2;
		StackElement 			*	stk_var=NULL;
		Symbol 		 			*	symbol_aux=NULL;
		ClassScriptObject		*	so_class_aux1=NULL;
		zs_float 				*	zs_float_aux;

		uint32_t 					msk_properties=0;

		Instruction 			*	instruction_it=_script_function->instructions;
		StackElement 			*	stk_start=_stk_local_var+_script_function->local_variables->size();   // <-- here starts stk for aux vars for operations ..

		if (ZS_IDX_VM_CURRENT_SCOPE_FUNCTION >= ZS_VM_FUNCTION_CALL_MAX) {
			ZS_VM_ERROR_AND_RETF("Reached max stack");
			return;
		}

		data->vm_stk_current = stk_start;

		if(((stk_start-data->vm_stack)+_script_function->min_code_stack_needed)>=ZS_VM_STACK_MAX){
			data->vm_error_max_stack_reached=true;
			ZS_VM_STOP_EXECUTEF("Error MAXIMUM stack size reached");
		}

#ifdef __DEBUG__
		ZS_LOG_DEBUG("Executing function %s ...",_script_function->name_script_function.c_str());
#endif

		// Init local vars ...
		data->vm_current_scope_function->current_scope_block=data->vm_current_scope_function->first_scope_block;
		data->vm_current_scope_function->stk_local_vars=_stk_local_var;
		data->vm_current_scope_function++;


		// init local variables symbols (except arguments) as undefined
		if((_script_function->idx_script_function != ZS_IDX_SCRIPT_FUNCTION_MAIN)){
			ZS_VM_PUSH_SCOPE(_script_function->scope_script_function);
			for(int i=_script_function->params_len; i <(int)_script_function->local_variables->size(); i++){
				ZS_STK_SET_UNDEFINED(_stk_local_var+ i);
			}
		}

		//-----------------------------------------------------------------------------------------------------------------------
		//
		// MAIN LOOP
		//
		for(;;){ // foreach byte code instruction ...

			instruction = instruction_it++;

			switch(instruction->byte_code){
			// load
			case ZS_BYTE_CODE_LOAD_GLOBAL: // load variable ...
				*data->vm_stk_current++=*(data->vm_stack+instruction->value_op2);
				continue;
			case ZS_BYTE_CODE_LOAD_LOCAL: // load variable ...
				*data->vm_stk_current++=*(_stk_local_var+instruction->value_op2);
				continue;
			case ZS_BYTE_CODE_EQU:  // ==
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_COMPARE(==, ZS_METAMETHOD_BYTE_CODE_EQU,false);
				continue;
			case ZS_BYTE_CODE_NOT_EQU:  // !=
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_COMPARE(!=, ZS_METAMETHOD_BYTE_CODE_NEQU,false);
				continue;
			case ZS_BYTE_CODE_LT:  // <
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_COMPARE(<, ZS_METAMETHOD_BYTE_CODE_LT,false);
				continue;
			case ZS_BYTE_CODE_LTE:  // <=
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_COMPARE(<=, ZS_METAMETHOD_BYTE_CODE_LTE,false);
				continue;
			case ZS_BYTE_CODE_GT:  // >
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_COMPARE(>,ZS_METAMETHOD_BYTE_CODE_GT,false);
				continue;
			case ZS_BYTE_CODE_GTE:  // >=
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_COMPARE(>=,ZS_METAMETHOD_BYTE_CODE_GTE,false);
				continue;
			case ZS_BYTE_CODE_LOGIC_AND:  // &&
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_LOGIC(&&);
				continue;
			case ZS_BYTE_CODE_LOGIC_OR:  // ||
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_LOGIC(||);
				continue;
			case ZS_BYTE_CODE_TYPEOF:
				VM_POP_STK_ONE;
				*data->vm_stk_current++ = stk_result_op1->typeOf();
				continue;
			case ZS_BYTE_CODE_ADD: // +
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_ADD;
				continue;
			case ZS_BYTE_CODE_SUB: // -
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_ARITHMETIC(-, ZS_METAMETHOD_BYTE_CODE_SUB);
				continue;
			case ZS_BYTE_CODE_MUL: // *
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_ARITHMETIC(*, ZS_METAMETHOD_BYTE_CODE_MUL);
				continue;
			case ZS_BYTE_CODE_DIV: // /
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_DIV;
				continue;
			case ZS_BYTE_CODE_MOD: // /
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_MOD(ZS_METAMETHOD_BYTE_CODE_MOD);
				continue;
			case ZS_BYTE_CODE_BITWISE_AND: // &
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_BINARY(&, ZS_METAMETHOD_BYTE_CODE_AND);
				continue;
			case ZS_BYTE_CODE_BITWISE_OR: // *
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_BINARY(| , ZS_METAMETHOD_BYTE_CODE_OR);
				continue;
			case ZS_BYTE_CODE_BITWISE_XOR: // ^
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_BINARY(^, ZS_METAMETHOD_BYTE_CODE_XOR);
				continue;
			case ZS_BYTE_CODE_SHR: // >>
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_BINARY(>> , ZS_METAMETHOD_BYTE_CODE_SHR);
				continue;
			case ZS_BYTE_CODE_SHL: // <<
				VM_POP_STK_TWO;
				ZS_VM_OPERATION_BINARY(<< , ZS_METAMETHOD_BYTE_CODE_SHL);
				continue;
			case ZS_BYTE_CODE_NOT: // !
				VM_POP_STK_ONE;
				if(stk_result_op1->properties & ZS_STK_PROPERTY_BOOL){ // boolean operation
					ZS_VM_PUSH_STK_BOOLEAN((!((bool)(stk_result_op1->value))));
				}else{
					if(vm_call_metamethod(
						_vm
						,_script_function
						,instruction
						,ZS_METAMETHOD_BYTE_CODE_NOT
						,stk_result_op1
						,NULL
						,false
					)==false){
						return;
					}
				}
				continue;
			case ZS_BYTE_CODE_NEG: // -
				VM_POP_STK_ONE;
				if(stk_result_op1->properties & ZS_STK_PROPERTY_INT){ // arithmetic operation
					ZS_VM_PUSH_STK_INT((-((zs_int)(stk_result_op1->value))));
				}else if(stk_result_op1->properties & ZS_STK_PROPERTY_FLOAT){
					ZS_VM_PUSH_STK_FLOAT(-ZS_READ_INTPTR_FLOAT(stk_result_op1->value));
				}else{ // try metamethod ...
					if(!vm_call_metamethod(
						_vm
						,_script_function
						,instruction
						,ZS_METAMETHOD_BYTE_CODE_NEG
						,stk_result_op1
						,NULL
						,false
					)){
						return;
					}
				}
				continue;
			case ZS_BYTE_CODE_BWC: // ~
				VM_POP_STK_ONE;
				if(stk_result_op1->properties & ZS_STK_PROPERTY_INT){ // arithmetic operation
					ZS_VM_PUSH_STK_INT((~((zs_int)(stk_result_op1->value))));
				}else{ // try metamethod ...
					if(!vm_call_metamethod(
							_vm
							,_script_function
							,instruction
							,ZS_METAMETHOD_BYTE_CODE_BWC
							,stk_result_op1
							,NULL
							,false
					)){
						return;
					}
				}
				continue;
			 case ZS_BYTE_CODE_INSTANCEOF: // check instance of ...
				 VM_POP_STK_ONE;
				switch(instruction->value_op2){
				case ZS_IDX_UNDEFINED:
					ZS_VM_STOP_EXECUTE("type '%s' does not exist ",SFI_GET_SYMBOL_NAME(_script_function,instruction));
					break;
				case IDX_TYPE_INT_C:
					ZS_VM_PUSH_STK_BOOLEAN((stk_result_op1->properties & ZS_STK_PROPERTY_INT)!=0);
					break;
				case IDX_TYPE_FLOAT_C:
					ZS_VM_PUSH_STK_BOOLEAN((stk_result_op1->properties & ZS_STK_PROPERTY_FLOAT)!=0);
					break;
				case IDX_TYPE_BOOL_C:
					ZS_VM_PUSH_STK_BOOLEAN((stk_result_op1->properties & ZS_STK_PROPERTY_BOOL)!=0);
					break;
				case IDX_TYPE_FUNCTION:
					ZS_VM_PUSH_STK_BOOLEAN((stk_result_op1->properties & ZS_STK_PROPERTY_FUNCTION)!=0);
					break;
				default:
					if(stk_result_op1->properties & ZS_STK_PROPERTY_SCRIPT_OBJECT){
						ZS_VM_PUSH_STK_BOOLEAN(data->script_type_factory->scriptTypeInheritsFrom(			//
								((ObjectScriptObject *)(stk_result_op1->value))->idx_script_type // A
								, instruction->value_op2		// B
						));
					}else{
						ZS_VM_PUSH_STK_BOOLEAN(false);
					}
					break;
				}
				continue;
			 case ZS_BYTE_CODE_JMP:
				instruction_it=instruction+instruction->value_op2;
				continue;
			 case ZS_BYTE_CODE_JNT: // goto if not true ... goes end to conditional.
				VM_POP_STK_ONE;
				if((stk_result_op1->properties & ZS_STK_PROPERTY_BOOL)==0){
					ZS_VM_STOP_EXECUTE(
						"Expected boolean expression but it was '%s'"
						,data->zs->stackElementToStringTypeOf(ZS_VM_STR_AUX_PARAM_0,stk_result_op1)
					);
				}
				if(stk_result_op1->value == 0){
					instruction_it=instruction+instruction->value_op2;
				}
				continue;
			 case ZS_BYTE_CODE_JT: // goto if true ... goes end to conditional.
				VM_POP_STK_ONE;
				if((stk_result_op1->properties & ZS_STK_PROPERTY_BOOL)==0){
					ZS_VM_STOP_EXECUTE(
						"Expected boolean expression but it was '%s'"
						,data->zs->stackElementToStringTypeOf(ZS_VM_STR_AUX_PARAM_0,stk_result_op1)
					);
				}
				if(stk_result_op1->value != 0){
					instruction_it=instruction+instruction->value_op2;
				}
				continue;
			 case ZS_BYTE_CODE_JE_CASE:  // especial j for switch
				VM_POP_STK_ONE_LOAD2; // reads switch value and case value
				ZS_VM_OPERATION_COMPARE(==, ZS_METAMETHOD_BYTE_CODE_EQU,true);
				VM_POP_STK_ONE; // retrieve result...
				if(stk_result_op1->value != 0){ // if true goto
					// reset stack
					data->vm_stk_current=stk_start;

					// goto to state
					instruction_it=instruction+instruction->value_op2;
				}
				continue;
			case ZS_BYTE_CODE_JMP_CASE:
  			    data->vm_stk_current=stk_start;
				instruction_it=instruction+instruction->value_op2;
				continue;
			case ZS_BYTE_CODE_LOAD_UNDEFINED:
				ZS_VM_PUSH_STK_UNDEFINED;
				continue;
			case ZS_BYTE_CODE_LOAD_NULL:
				ZS_VM_PUSH_STK_NULL;
				continue;
			case ZS_BYTE_CODE_LOAD_FUNCTION: // expect constant and function has the same behaviour...
				ZS_VM_PUSH_STK_SCRIPT_FUNCTION(instruction->value_op2);
				continue;
			case ZS_BYTE_CODE_LOAD_INT:
				data->vm_stk_current->value=instruction->value_op2;
				data->vm_stk_current->properties=ZS_STK_PROPERTY_INT;
				data->vm_stk_current++;
				continue;
			case ZS_BYTE_CODE_LOAD_FLOAT:
				data->vm_stk_current->value=instruction->value_op2;
				data->vm_stk_current->properties=ZS_STK_PROPERTY_FLOAT;
				data->vm_stk_current++;
				continue;
			case ZS_BYTE_CODE_LOAD_BOOL:
				data->vm_stk_current->value=instruction->value_op2;
				data->vm_stk_current->properties=ZS_STK_PROPERTY_BOOL;
				data->vm_stk_current++;
				continue;
			case ZS_BYTE_CODE_LOAD_STACK_ELEMENT:
			case ZS_BYTE_CODE_LOAD_STRING:
				*data->vm_stk_current++=*((StackElement *)instruction->value_op2);
				continue;
				// to avoid deref global objects from returning functions
			case ZS_BYTE_CODE_PUSH_STK_GLOBAL_IRGO:

				if((data->vm_stack+instruction->value_op2)->properties & ZS_STK_PROPERTY_SCRIPT_OBJECT){
					// push load
					ZS_VM_PUSH_STK_PTR(data->vm_stack + instruction->value_op2);
				}else{
					// normal load
					*data->vm_stk_current++=*(data->vm_stack+instruction->value_op2);
				}
				continue;
			 case  ZS_BYTE_CODE_RET:
				for(stk_var=data->vm_stk_current-1;stk_var>=stk_start;stk_var--){ // can return something. value is +1 from stack
					stk_result_op1=stk_var;

					// if global var...
					// else if object ...
					if((stk_result_op1->properties & ZS_STK_PROPERTY_SCRIPT_OBJECT)==ZS_STK_PROPERTY_SCRIPT_OBJECT){
						// ... deref
						if(vm_unref_script_object_for_ret(_vm, stk_result_op1)==false){
							return;
						}
					}
				}
				goto lbl_return_function;
			case ZS_BYTE_CODE_PUSH_STK_GLOBAL: // load variable ...
				ZS_VM_PUSH_STK_PTR(data->vm_stack + instruction->value_op2);
				continue;
			case ZS_BYTE_CODE_PUSH_STK_LOCAL: // load variable ...
				ZS_VM_PUSH_STK_PTR(_stk_local_var+instruction->value_op2);
				continue;
			 case ZS_BYTE_CODE_PUSH_SCOPE:
				ZS_VM_PUSH_SCOPE(instruction->value_op2);
				continue;
			 case ZS_BYTE_CODE_POP_SCOPE:
				 vm_pop_scope(_vm);
				 continue;
			 case ZS_BYTE_CODE_POST_INC:
				 VM_POST_OPERATION(++,ZS_METAMETHOD_BYTE_CODE_POST_INC);
				 continue;
			 /*case ZS_BYTE_CODE_NEG_POST_INC:
				 ZS_VM_OPERATION_NEG_POST(++,ZS_METAMETHOD_BYTE_CODE_POST_INC);
				 continue;
			 case ZS_BYTE_CODE_BWC_POST_INC:
				 ZS_VM_OPERATION_BWC_POST(++,ZS_METAMETHOD_BYTE_CODE_POST_INC);
				 continue;*/
			 case ZS_BYTE_CODE_POST_DEC:
				 VM_POST_OPERATION(--,ZS_METAMETHOD_BYTE_CODE_POST_DEC);
				 continue;
			 /*case ZS_BYTE_CODE_NEG_POST_DEC:
				 ZS_VM_OPERATION_NEG_POST(--,ZS_METAMETHOD_BYTE_CODE_POST_DEC);
				 continue;
			 case ZS_BYTE_CODE_BWC_POST_DEC:
				 ZS_VM_OPERATION_BWC_POST(--,ZS_METAMETHOD_BYTE_CODE_POST_DEC);
				 continue;*/
			 case ZS_BYTE_CODE_PRE_INC:
				 VM_PRE_OPERATION(++,ZS_METAMETHOD_BYTE_CODE_PRE_INC);
				 continue;
			 case ZS_BYTE_CODE_PRE_DEC:
				 VM_PRE_OPERATION(--,ZS_METAMETHOD_BYTE_CODE_PRE_DEC);
				 continue;
			case ZS_BYTE_CODE_ADD_ST:
				ZS_LOAD_OPERANDS_ST_OPERATION;
				ZS_VM_OPERATION_ADD_ST();
				continue;
			case ZS_BYTE_CODE_SUB_ST:
				ZS_LOAD_OPERANDS_ST_OPERATION;
				ZS_VM_OPERATION_ARITHMETIC_ST(-=,ZS_METAMETHOD_BYTE_CODE_SUB_ST);
				continue;
			case ZS_BYTE_CODE_MUL_ST:
				ZS_LOAD_OPERANDS_ST_OPERATION;
				ZS_VM_OPERATION_ARITHMETIC_ST(*=,ZS_METAMETHOD_BYTE_CODE_MUL_ST);
				continue;
			case ZS_BYTE_CODE_DIV_ST:
				ZS_LOAD_OPERANDS_ST_OPERATION;
				ZS_VM_OPERATION_DIV_ST();
				continue;
			case ZS_BYTE_CODE_MOD_ST:
				ZS_LOAD_OPERANDS_ST_OPERATION;
				ZS_VM_OPERATION_MOD_ST();
				continue;
			case ZS_BYTE_CODE_BITWISE_AND_ST:
				ZS_LOAD_OPERANDS_ST_OPERATION;
				ZS_VM_OPERATION_BINARY_ST(&=,ZS_METAMETHOD_BYTE_CODE_AND_ST);
				continue;
			case ZS_BYTE_CODE_BITWISE_OR_ST:
				ZS_LOAD_OPERANDS_ST_OPERATION;
				ZS_VM_OPERATION_BINARY_ST(|=,ZS_METAMETHOD_BYTE_CODE_OR_ST);
				continue;
			case ZS_BYTE_CODE_BITWISE_XOR_ST:
				ZS_LOAD_OPERANDS_ST_OPERATION;
				ZS_VM_OPERATION_BINARY_ST(^=,ZS_METAMETHOD_BYTE_CODE_XOR_ST);
				continue;
			case ZS_BYTE_CODE_SHL_ST:
				ZS_LOAD_OPERANDS_ST_OPERATION;
				ZS_VM_OPERATION_BINARY_ST(<<=,ZS_METAMETHOD_BYTE_CODE_SHL_ST);
				continue;
			case ZS_BYTE_CODE_SHR_ST:
				ZS_LOAD_OPERANDS_ST_OPERATION;
				ZS_VM_OPERATION_BINARY_ST(>>=,ZS_METAMETHOD_BYTE_CODE_SHR_ST);
				continue;
			case ZS_BYTE_CODE_STORE_CONST:
			case ZS_BYTE_CODE_STORE:
				if(vm_store(
					_vm
					,_script_function
					,instruction
				)==false){
					return;
				}
				if(instruction->properties & ZS_INSTRUCTION_PROPERTY_RESET_STACK){
					//data->vm_stk_current=stk_start;
					vm_reset_stack(_vm,stk_start);
				}
				continue;
			//----- immediate call
			case  ZS_BYTE_CODE_CALL: // immediate call this
			case  ZS_BYTE_CODE_SUPER_CALL:
			case  ZS_BYTE_CODE_THIS_CALL: // immediate call this
			case  ZS_BYTE_CODE_INDIRECT_THIS_CALL:
			case  ZS_BYTE_CODE_INDIRECT_LOCAL_CALL: // call from idx var
			case  ZS_BYTE_CODE_INDIRECT_GLOBAL_CALL: // call from idx var
			case  ZS_BYTE_CODE_STACK_CALL: // stack call
			case  ZS_BYTE_CODE_CONSTRUCTOR_CALL:
			case  ZS_BYTE_CODE_MEMBER_CALL: // calling function after all of args are processed...
				if(vm_call(
						_vm
						,_this_object
						,_script_function
						,instruction
						,_stk_local_var
				)==false){
					return;\
				}

				if(instruction->properties & ZS_INSTRUCTION_PROPERTY_RESET_STACK){
					//data->vm_stk_current=stk_start;
					vm_reset_stack(_vm,stk_start);\
				}
				continue;
			case ZS_BYTE_CODE_PUSH_STK_THIS: // load variable ...
				ZS_VM_PUSH_STK_PTR(_this_object->getThis());
				continue;
			case ZS_BYTE_CODE_LOAD_REF:
				*data->vm_stk_current++=*ZS_GET_STK_VAR_REF(_stk_local_var+instruction->value_op2);
				continue;
			case ZS_BYTE_CODE_LOAD_THIS: // load variable ...
				*data->vm_stk_current++=*_this_object->getThis();
				continue;
			 case  ZS_BYTE_CODE_NEW_OBJECT_BY_TYPE:

				 	so_aux=NEW_OBJECT_VAR_BY_TYPE_IDX(data->script_type_factory,instruction->value_op1);

					vm_create_shared_script_object(_vm,so_aux);

					if(so_aux->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_CLASS){
						so_class_aux1=(ClassScriptObject *)so_aux;
						so_class_aux1->info_function_new=_script_function;
						so_class_aux1->instruction_new=instruction;
					}
					(*data->vm_stk_current++)={(zs_int)so_aux,ZS_STK_PROPERTY_SCRIPT_OBJECT};
					continue;
			 case  ZS_BYTE_CODE_NEW_OBJECT_BY_VALUE:
				 	 if(vm_new_object_by_value(
							_vm
							,_script_function
							,instruction
			 		)==false){
						return;
				 	 }
				 	 continue;
			 case ZS_BYTE_CODE_NEW_ARRAY: // Create new vector...
					so_aux=ZS_NEW_ARRAY_OBJECT(data->zs);
					vm_create_shared_script_object(_vm,so_aux);
					data->vm_stk_current->value=(zs_int)so_aux;
					data->vm_stk_current->properties=ZS_STK_PROPERTY_SCRIPT_OBJECT;
					data->vm_stk_current++;
					continue;
			 case  ZS_BYTE_CODE_NEW_OBJECT: // Create new object...
				 	so_aux=ZS_NEW_OBJECT_OBJECT(data->zs);
					vm_create_shared_script_object(_vm,so_aux);
					(*data->vm_stk_current++)={(zs_int)so_aux,ZS_STK_PROPERTY_SCRIPT_OBJECT};
					continue;

			 case  ZS_BYTE_CODE_NEW_STRING: // Create new string...
				 vm_new_string_object(
					 _vm
					 ,instruction
				);
				continue;
			 case  ZS_BYTE_CODE_DELETE:
				 vm_delete_object(
						_vm
				);
				continue;
			/* case ZS_BYTE_CODE_CLEAR_ZERO_POINTERS:
				 vm_remove_empty_shared_pointers(_vm,ZS_VM_CURRENT_SCOPE_BLOCK);
				 continue;*/
			 case ZS_BYTE_CODE_RESET_STACK:
				 data->vm_stk_current=stk_start;
				 continue;
			case ZS_BYTE_CODE_IT_INIT:
				if(vm_iterator_init(
						_vm
						,_this_object
						,_script_function
						,instruction
						,_stk_local_var
				)==false){
					return;
				}
				// always it does a reset
				data->vm_stk_current=stk_start;
				continue;
			 case ZS_BYTE_CODE_IN:
				 if(vm_perform_in_operator(
						 _vm
						 ,_this_object
						 ,_script_function
						 ,instruction
						 ,_stk_local_var
				)==false){
					return;
				 }
				 continue;
				case ZS_BYTE_CODE_LOAD_TYPE:
					ZS_VM_PUSH_STK_TYPE(instruction->value_op2);
					continue;
				case ZS_BYTE_CODE_PUSH_ARRAY_ITEM:
					if(vm_push_container_item(
							 _vm
							 ,_this_object
							 ,_script_function
							 ,instruction
							 ,_stk_local_var
							 ,false
					 )==false){
						return;
					 }
					continue;
				case ZS_BYTE_CODE_PUSH_OBJECT_ITEM:
					if(vm_push_container_item(
							 _vm
							 ,_this_object
							 ,_script_function
							 ,instruction
							 ,_stk_local_var
							 , true
					)==false){
						return;
					 }
					continue;
				// access vector (i.e vec[1]) or access object (i.e obj["aa"])
				case ZS_BYTE_CODE_PUSH_STK_ARRAY_ITEM:
				case ZS_BYTE_CODE_LOAD_ARRAY_ITEM:
					if(vm_load_vector_item(
						 _vm
						 ,_this_object
						 ,_script_function
						 ,instruction
						 ,_stk_local_var
					)==false){
						return;
					}
					continue;
				case ZS_BYTE_CODE_LOAD_THIS_FUNCTION:// direct load
					symbol_aux=(Symbol *)_this_object->getScriptType()->getSymbolMemberFunction(((Symbol *)instruction->value_op2)->name);
					if(symbol_aux==NULL){ // it calls overrided function (top-most)
						 ZS_VM_STOP_EXECUTE("Error load 'this.%s': Cannot find '%s::%s' member function"
								,((Symbol *)instruction->value_op2)->name.c_str()
								,_this_object->getScriptType()->str_script_type.c_str()
								,((Symbol *)instruction->value_op2)->name.c_str()
						);
					 }

					so_aux=ZS_NEW_OBJECT_MEMBER_FUNCTION(data->zs,_this_object,(ScriptFunction *)(symbol_aux->ref_ptr));

					 vm_create_shared_script_object(_vm,so_aux);
					 data->vm_stk_current->value=(zs_int)so_aux;
					 data->vm_stk_current->properties=ZS_STK_PROPERTY_SCRIPT_OBJECT;
					 data->vm_stk_current++;
					continue;
				case ZS_BYTE_CODE_LOAD_CONSTRUCTOR_FUNCT:
					so_aux=(ClassScriptObject *)((data->vm_stk_current-1)->value);
					if(instruction->value_op2 == ZS_IDX_UNDEFINED){
						ZS_VM_PUSH_STK_UNDEFINED;
					}else{
						data->vm_stk_current->value=(zs_int) so_aux->getScriptType()->scope_script_type->symbol_functions->get(instruction->value_op2);
						data->vm_stk_current->properties=ZS_STK_PROPERTY_MEMBER_FUNCTION;
						data->vm_stk_current++;
					}
					continue;
				case ZS_BYTE_CODE_LOAD_THIS_VARIABLE:
				case ZS_BYTE_CODE_PUSH_STK_THIS_VARIABLE:
				case ZS_BYTE_CODE_PUSH_STK_OBJECT_ITEM:
				case ZS_BYTE_CODE_LOAD_OBJECT_ITEM:
					if(vm_load_field(
							_vm
							,_this_object
							,_script_function
							,&instruction_it
					)==false){
						return;
					}
					continue;
			case ZS_BYTE_CODE_FIND_VARIABLE:
			case ZS_BYTE_CODE_UNRESOLVED_CALL:
				vm_print_main_error(\
						_vm\
						,_script_function\
						,instruction\
						,ZS_VM_MAIN_ERROR_CANNOT_FIND_SYMBOL
				);
				return;
			case ZS_BYTE_CODE_END_FUNCTION:
			default:
				goto lbl_exit_function;
			}

		 }

	lbl_exit_function:
		// default return null
		 ZS_STK_SET_UNDEFINED(data->vm_stk_current);
		// reset stack and set last stk return null;
		data->vm_stk_current=_stk_local_var;

	lbl_return_function:


		//=========================
		// POP STACK

		while (
			(ZS_VM_CURRENT_SCOPE_FUNCTION->current_scope_block > ZS_VM_CURRENT_SCOPE_FUNCTION->first_scope_block)
		){
			vm_pop_scope(_vm); // do not check removeEmptySharedPointers to have better performance
		}
		--data->vm_current_scope_function;

		// POP STACK
		//=========================
	}



	void vm_push_stk_boolean_equal_strings(
		VirtualMachine			*	_vm
		, StackElement 			*	_stk1
		, StackElement 			*	_stk2
		, MetamethodByteCode 		_metamethod_byte_code
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		zs_string str1="@@@STR1_NOT_INIT@@@@";
		zs_string str2="@@@STR2_NOT_INIT@@@@";

		if(ZS_STK_IS_STRING_SCRIPT_OBJECT(_stk1)){
			str1=((StringScriptObject *)(_stk1->value))->get();
		}else{
			str1=data->zs->stackElementToString(_stk1);
		}



		if(ZS_STK_IS_STRING_SCRIPT_OBJECT(_stk2)){
			str2=((StringScriptObject *)(_stk2->value))->get();
		}else{
			str2=data->zs->stackElementToString( _stk2);
		}

		bool result=false;

		switch(_metamethod_byte_code){
		case ZS_METAMETHOD_BYTE_CODE_NEQU:
			result= ZS_STRCMP(str1.c_str(),!=,str2.c_str());
			break;
		case ZS_METAMETHOD_BYTE_CODE_EQU:
			result= ZS_STRCMP(str1.c_str(),==,str2.c_str());
			break;
		case ZS_METAMETHOD_BYTE_CODE_LT:
			result= ZS_STRCMP(str1.c_str(),<,str2.c_str());
			break;
		case ZS_METAMETHOD_BYTE_CODE_GT:
			result= ZS_STRCMP(str1.c_str(),>,str2.c_str());
			break;
		case ZS_METAMETHOD_BYTE_CODE_LTE:
			result= ZS_STRCMP(str1.c_str(),<=,str2.c_str());
			break;
		case ZS_METAMETHOD_BYTE_CODE_GTE:
			result= ZS_STRCMP(str1.c_str(),>=,str2.c_str());
			break;
		default:
			ZS_VM_SET_USER_ERRORF(_vm," Internal error expected metamethod_byte_code comparer");
			return;
			break;
		}

		ZS_VM_PUSH_STK_BOOLEAN(result);
	}	

	void vm_print_main_error(
		VirtualMachine 			*	_vm
		,ScriptFunction 		*	_script_function
		,Instruction 			*	_instruction
		,VM_MainError 				_error
		,StackElement 			*	_stk
		,MetamethodByteCode 		_metamethod_byte_code
	){

		VirtualMachineData 	*	data=(VirtualMachineData *)_vm->data;
		Instruction			*	instruction=_instruction;

		switch(_error){

		case 	ZS_VM_MAIN_ERROR_ZS_LOAD_PROPERTIES_ERROR:
			vm_set_file_line_error(\
				_vm \
				,SFI_GET_FILE(_script_function,instruction)\
				,SFI_GET_LINE(_script_function,instruction)\
				,"Error performing '%s%s': Cannot perform operation with value as '%s%s%s'"\
				,SFI_GET_SYMBOL_NAME(_script_function,instruction-1)
				,metamethod_byte_code_to_operator_str(_metamethod_byte_code)
				,SFI_GET_SYMBOL_NAME(_script_function,instruction-1)
				,metamethod_byte_code_to_operator_str(_metamethod_byte_code)
				,data->zs->stackElementToString(ZS_VM_STR_AUX_PARAM_0,ZS_VM_STR_AUX_MAX_LENGTH,_stk)
			);
			break;
		case ZS_VM_MAIN_ERROR_METAMETHOD_OPERATION_MEMBER_PROPERTY_NOT_IMPLEMENTED:
			vm_set_file_line_error(\
				_vm \
				,SFI_GET_FILE(_script_function,instruction)\
				,SFI_GET_LINE(_script_function,instruction)\
				,"Member property '%s' not implements metamethod '%s' (aka '%s') " \
				,SFI_GET_SYMBOL_NAME(_script_function,instruction-1)\
				,metamethod_byte_code_to_symbol_str(_metamethod_byte_code)\
				,metamethod_byte_code_to_operator_str(_metamethod_byte_code)\
			);\
			break;
		case ZS_VM_MAIN_ERROR_METAMETHOD_OPERATION_SYMBOL_NOT_IMPLEMENTED:
			vm_set_file_line_error(\
				_vm \
				,SFI_GET_FILE(_script_function,instruction)\
				,SFI_GET_LINE(_script_function,instruction)\
				,"Symbol '%s' as type '%s' not implements metamethod '%s' (aka '%s') " \
				,SFI_GET_SYMBOL_NAME(_script_function,instruction-1)\
				,data->zs->stackElementToStringTypeOf(ZS_VM_STR_AUX_PARAM_0,_stk) \
				,metamethod_byte_code_to_symbol_str(_metamethod_byte_code)\
				,metamethod_byte_code_to_operator_str(_metamethod_byte_code)\
			);\
			break;
		case ZS_VM_MAIN_ERROR_CANNOT_FIND_SYMBOL:
			{

				const char *__STR_PTR_SYMBOL_TO_FIND__=SFI_GET_SYMBOL_NAME(_script_function,instruction);
				const char *__STR_PTR_END_CLASS__=NULL;

				if((__STR_PTR_END_CLASS__=strstr(__STR_PTR_SYMBOL_TO_FIND__,"::"))!=NULL){ // static access
					char str_script_type[512]={0};

					strncpy(str_script_type,__STR_PTR_SYMBOL_TO_FIND__,__STR_PTR_END_CLASS__-__STR_PTR_SYMBOL_TO_FIND__);


					if(data->zs->getScriptTypeFactory()->getScriptType(str_script_type) == NULL){
						vm_set_file_line_error(\
								_vm \
								,SFI_GET_FILE(_script_function,instruction)\
								,SFI_GET_LINE(_script_function,instruction)\
								,"type '%s' not exist"
								,str_script_type
						);
					}else{
						vm_set_file_line_error(\
								_vm \
								,SFI_GET_FILE(_script_function,instruction)\
								,SFI_GET_LINE(_script_function,instruction)\
								,"static symbol '%s::%s' not exist"
								,str_script_type
								,__STR_PTR_END_CLASS__+2
						);
					}
				}else{
					vm_set_file_line_error(\
							_vm \
							,SFI_GET_FILE(_script_function,instruction)\
							,SFI_GET_LINE(_script_function,instruction)\
							,"Symbol '%s' not defined"
							,__STR_PTR_SYMBOL_TO_FIND__
					);
				}

			}
			break;

			case ZS_VM_MAIN_ERROR_POST_OPERATOR_CANNOT_PERFORM_NEGATE_OPERATION:
				vm_set_file_line_error(\
					_vm \
					,SFI_GET_FILE(_script_function,instruction)\
					,SFI_GET_LINE(_script_function,instruction)\
					,"Symbol '%s' as type '%s' the metamethod '%s' (aka '%s') doesn't returns value to perform negate operation" \
					,SFI_GET_SYMBOL_NAME(_script_function,instruction-1)\
					,data->zs->stackElementToStringTypeOf(ZS_VM_STR_AUX_PARAM_0,_stk) \
					,metamethod_byte_code_to_symbol_str(_metamethod_byte_code)\
					,metamethod_byte_code_to_operator_str(_metamethod_byte_code)\
				);\
			break;
		}
	}

	bool vm_unref_script_object_for_ret(
			VirtualMachine *vm
			, StackElement *stk_var
	){
		VirtualMachineData *data=(VirtualMachineData *)vm->data;
		ScriptObject *so_aux=(ScriptObject *)stk_var->value;

		//special case for constant string object (they don't are shared elements)
		if(so_aux->idx_script_type == IDX_TYPE_SCRIPT_OBJECT_STRING && (so_aux->properties & SCRIPT_OBJECT_PROPERTY_CONSTANT)){
			// if is not shared is constant...
			so_aux=ZS_NEW_STRING_OBJECT(data->zs,so_aux->toString());
			stk_var->properties=ZS_STK_PROPERTY_SCRIPT_OBJECT;
			stk_var->value=(zs_int)so_aux;
		}else{
			if(so_aux->shared_pointer->data.n_shares<=1){ // was created here... remove share data

				// deatch from list of created scope block to avoid double dealloc on pop scope
				if(so_aux->shared_pointer->data.n_shares == 0){
					vm_deattach_shared_node(
							vm
							,&so_aux->shared_pointer->data.vm_scope_block_where_created->unreferenced_objects
							,so_aux->shared_pointer
					);
				}
				free(so_aux->shared_pointer);
				so_aux->shared_pointer=NULL;
			}
			//-------------------------------------------------------------------
		}

		return true;
	}

	void vm_delete_object(
		VirtualMachine *_vm
	){
		VirtualMachineData *data=(VirtualMachineData *)_vm->data;
		StackElement *stk_result_op1=NULL;
		ScriptObject *so_aux=NULL;
		ClassScriptObject *so_class_aux1=NULL;

		VM_POP_STK_ONE;
		//script_var
		if(stk_result_op1->properties & ZS_STK_PROPERTY_PTR_STK){
			stk_result_op1=(StackElement *)stk_result_op1->value;
		}

		if(stk_result_op1->properties & ZS_STK_PROPERTY_SCRIPT_OBJECT){
			so_class_aux1=NULL;

			so_aux = (ScriptObject *)(stk_result_op1)->value;

			vm_unref_shared_script_object(_vm,so_aux,NULL);

			if(so_aux->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_CLASS)
			{ // max ...
				so_class_aux1=(ClassScriptObject *)so_aux;

				if(so_class_aux1->isCreatedByContructor()){
					so_class_aux1->deleteNativeObjectOnDestroy(true);
				}
			}
			ZS_STK_SET_UNDEFINED(stk_result_op1);
		}
	}

	void vm_new_string_object(
			VirtualMachine 	*	_vm
			,Instruction 	*	_instruction
	){
		VirtualMachineData 	*	data=(VirtualMachineData *)_vm->data;
		ScriptObject 		*	so_aux=NULL;


		so_aux= StringScriptObject::newStringScriptObject(data->zs,_instruction->getConstantValueOp2ToString(false));
		vm_create_shared_script_object(_vm,so_aux);
		(*data->vm_stk_current++)={(zs_int)so_aux,ZS_STK_PROPERTY_SCRIPT_OBJECT};
	}
}

