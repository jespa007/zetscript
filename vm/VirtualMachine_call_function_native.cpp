#define PTR_FUNCTION_VOID_PARAM0(f) ((void (*)())(f))
#define PTR_FUNCTION_VOID_PARAM1(f) ((void (*)(intptr_t))(f))
#define PTR_FUNCTION_VOID_PARAM2(f) ((void (*)(intptr_t,intptr_t))(f))
#define PTR_FUNCTION_VOID_PARAM3(f) ((void (*)(intptr_t,intptr_t,intptr_t))(f))
#define PTR_FUNCTION_VOID_PARAM4(f) ((void (*)(intptr_t,intptr_t,intptr_t,intptr_t))(f))
#define PTR_FUNCTION_VOID_PARAM5(f) ((void (*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(f))
#define PTR_FUNCTION_VOID_PARAM6(f) ((void (*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(f))

#define PTR_FUNCTION_RET_PARAM0(f) ((intptr_t (*)())(f))
#define PTR_FUNCTION_RET_PARAM1(f) ((intptr_t (*)(intptr_t))(f))
#define PTR_FUNCTION_RET_PARAM2(f) ((intptr_t (*)(intptr_t,intptr_t))(f))
#define PTR_FUNCTION_RET_PARAM3(f) ((intptr_t (*)(intptr_t,intptr_t,intptr_t))(f))
#define PTR_FUNCTION_RET_PARAM4(f) ((intptr_t (*)(intptr_t,intptr_t,intptr_t,intptr_t))(f))
#define PTR_FUNCTION_RET_PARAM5(f) ((intptr_t (*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(f))
#define PTR_FUNCTION_RET_PARAM6(f) ((intptr_t (*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(f))


#define PTR_FUNCTION_RET_FLOAT_PARAM0(f) ((float (*)())(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM1(f) ((float (*)(intptr_t))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM2(f) ((float (*)(intptr_t,intptr_t))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM3(f) ((float (*)(intptr_t,intptr_t,intptr_t))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM4(f) ((float (*)(intptr_t,intptr_t,intptr_t,intptr_t))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM5(f) ((float (*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(f))
#define PTR_FUNCTION_RET_FLOAT_PARAM6(f) ((float (*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(f))

#define PTR_FUNCTION_RET_BOOL_PARAM0(f) ((bool (*)())(f))
#define PTR_FUNCTION_RET_BOOL_PARAM1(f) ((bool (*)(intptr_t))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM2(f) ((bool (*)(intptr_t,intptr_t))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM3(f) ((bool (*)(intptr_t,intptr_t,intptr_t))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM4(f)	((bool (*)(intptr_t,intptr_t,intptr_t,intptr_t))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM5(f) ((bool (*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(f))
#define PTR_FUNCTION_RET_BOOL_PARAM6(f) ((bool (*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(f))

namespace zetscript{

	StackElement  VirtualMachine::callFunctionNative(
			intptr_t fun_ptr,
			const ScriptFunction *calling_function,
			StackElement *stk_arg_calling_function,
			unsigned char n_args,
			Instruction *ins,
			ScriptVar  * this_object
			){

		StackElement stk_result={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

		intptr_t converted_param[MAX_NATIVE_FUNCTION_ARGS];
		int this_arg=(calling_function->symbol.symbol_properties&SYMBOL_PROPERTY_SET_FIRST_PARAMETER_AS_THIS)?1:0;
		intptr_t result=0;
		StackElement *stk_arg_current;
		current_call_c_function = calling_function;
		bool static_ref=calling_function->symbol.symbol_properties&SYMBOL_PROPERTY_STATIC_REF;
		//float aux_float=0;

		n_args=n_args+this_arg;

		if(n_args>MAX_NATIVE_FUNCTION_ARGS){
			THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,ins),"Max run-time args! (Max:%i Provided:%i)",MAX_NATIVE_FUNCTION_ARGS,n_args);
		}

		if((calling_function->symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) != SYMBOL_PROPERTY_C_OBJECT_REF) {
			THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,ins),"Function is not registered as C");
		}

		if(fun_ptr==0){
			THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,ins),"Null function");
		}

		if((char)calling_function->function_params->count != (n_args)){
			THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,ins),"C argument VS scrip argument doestn't match sizes");
		}

		if(calling_function->function_params->count > MAX_NATIVE_FUNCTION_ARGS){
			THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,ins),"Reached max param for C function (Current: %i Max Allowed: %i)",calling_function->function_params->count,MAX_NATIVE_FUNCTION_ARGS);
		}

		// convert parameters script to c...
		for(unsigned char  i = 0; i < n_args;i++){

			if( i==0 && this_arg==1){ // pass this...
				if(!static_ref){
					THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,ins),"Internal error: Cannot set parameter as this object due is not static");
				}

				if(this_object==NULL){
					THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,ins),"Internal error: Cannot set parameter as this object due this object is NULL");
				}

				if(this_object->idx_class>=IDX_BUILTIN_TYPE_MAX){
					converted_param[0]=(intptr_t)this_object->getNativeObject(); // pass c object
				}else{ // pass script var
					converted_param[0]=(intptr_t)this_object; // pass c object
				}
			}
			else{
				stk_arg_current=&stk_arg_calling_function[i];
				FunctionParam *function_param=(FunctionParam *)calling_function->function_params->items[i];

				if(!zs->convertStackElementToVar(stk_arg_current,function_param->idx_type,(intptr_t *)&converted_param[i],error_str)){
					THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(calling_function,ins),"Function %s, param %i: %s. The function C %s that was found for first time it has different argument types now.",
																	calling_function->symbol.name.c_str(),
																	i,
																	error_str.c_str(),
																	calling_function->symbol.name.c_str()
																	);
				}
			}
		}

		ZS_PRINT_DEBUG("pre_call %i",n_args);

		if(calling_function->idx_return_type == IDX_BUILTIN_TYPE_VOID_C){ // getInstance()->getIdxClassVoid()){

			switch(n_args){
			case 0:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM0(fun_ptr)();
				}else{
					(*((CFunctionMemberPointerVoidParam0 *)fun_ptr))();
				}
				break;
			case 1:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM1(fun_ptr)(
						converted_param[0]
					);
				}else{
					(*((CFunctionMemberPointerVoidParam1 *)fun_ptr))(
						converted_param[0]
					);
				}
				break;
			case 2:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM2(fun_ptr)(
						converted_param[0]
						,converted_param[1]
					);
				}else{
					(*((CFunctionMemberPointerVoidParam2 *)fun_ptr))(
						converted_param[0]
						,converted_param[1]
					);
				}
				break;
			case 3:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM3(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}else{
					(*((CFunctionMemberPointerVoidParam3 *)fun_ptr))(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}
				break;
			case 4:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM4(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}else{
					(*((CFunctionMemberPointerVoidParam4 *)fun_ptr))(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}
				break;
			case 5:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM5(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
				  );
				}else{
					(*((CFunctionMemberPointerVoidParam5 *)fun_ptr))(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
					);
				}
				break;
			case 6:
				if(static_ref){
					PTR_FUNCTION_VOID_PARAM6(fun_ptr)(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
				  );
				}else{
					(*((CFunctionMemberPointerVoidParam6 *)fun_ptr))(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
					);
				}
				break;
			}

		}else if(calling_function->idx_return_type==IDX_BUILTIN_TYPE_BOOL_C){  // we must do a bool cast in order to get float return.
			switch(n_args){
			case 0:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM0(fun_ptr)();
				}else{
					result=(*((FunctionMemberPointerRetParam0 *)fun_ptr)).ret_bool();
				}
				break;
			case 1:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM1(fun_ptr)(
						converted_param[0]
					);
				}else{
					result=(*((FunctionMemberPointerRetParam1 *)fun_ptr)).ret_bool(
						converted_param[0]
					);
				}
				break;
			case 2:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM2(fun_ptr)(
						converted_param[0]
						,converted_param[1]
					);
				}else{
					result=(*((FunctionMemberPointerRetParam2 *)fun_ptr)).ret_bool(
						converted_param[0]
						,converted_param[1]
					);
				}
				break;
			case 3:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM3(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}else{
					result=(*((FunctionMemberPointerRetParam3 *)fun_ptr)).ret_bool(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}
				break;
			case 4:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM4(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}else{
					result=(*((FunctionMemberPointerRetParam4 *)fun_ptr)).ret_bool(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}
				break;
			case 5:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM5(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
				  );
				}else{
					result=(*((FunctionMemberPointerRetParam5 *)fun_ptr)).ret_bool(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
					);
				}
				break;
			case 6:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM6(fun_ptr)(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
				  );
				}else{
					result=(*((FunctionMemberPointerRetParam6 *)fun_ptr))(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
					);
				}
				break;
			}
		}else if(calling_function->idx_return_type==IDX_BUILTIN_TYPE_FLOAT_C){ // we must do a float cast in order to get float return.
			float aux_flt;
			switch(n_args){
			case 0:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM0(fun_ptr)();
				}else{
					aux_flt=(*((FunctionMemberPointerRetParam0 *)fun_ptr)).ret_float();
				}
				break;
			case 1:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM1(fun_ptr)(
						converted_param[0]
					);
				}else{
					aux_flt=(*((FunctionMemberPointerRetParam1 *)fun_ptr)).ret_float(
						converted_param[0]
					);
				}
				break;
			case 2:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM2(fun_ptr)(
						converted_param[0]
						,converted_param[1]
					);
				}else{
					aux_flt=(*((FunctionMemberPointerRetParam2 *)fun_ptr)).ret_float(
						converted_param[0]
						,converted_param[1]
					);
				}
				break;
			case 3:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM3(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}else{
					aux_flt=(*((FunctionMemberPointerRetParam3 *)fun_ptr)).ret_float(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}
				break;
			case 4:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM4(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}else{
					aux_flt=(*((FunctionMemberPointerRetParam4 *)fun_ptr)).ret_float(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}
				break;
			case 5:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM5(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
				  );
				}else{
					aux_flt=(*((FunctionMemberPointerRetParam5 *)fun_ptr)).ret_float(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
					);
				}
				break;
			case 6:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM6(fun_ptr)(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
				  );
				}else{
					aux_flt=(*((FunctionMemberPointerRetParam6 *)fun_ptr))(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
					);
				}
				break;
			}

			memcpy(&result, &aux_flt, sizeof(float));
		}else{ // generic pointer or int

			switch(n_args){
			case 0:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM0(fun_ptr)();
				}else{
					result=(*((FunctionMemberPointerRetParam0 *)fun_ptr))();
				}
				break;
			case 1:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM1(fun_ptr)(
						converted_param[0]
					);
				}else{
					result=(*((FunctionMemberPointerRetParam1 *)fun_ptr))(
						converted_param[0]
					);
				}
				break;
			case 2:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM2(fun_ptr)(
						converted_param[0]
						,converted_param[1]
					);
				}else{
					result=(*((FunctionMemberPointerRetParam2 *)fun_ptr))(
						converted_param[0]
						,converted_param[1]
					);
				}
				break;
			case 3:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM3(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}else{
					result=(*((FunctionMemberPointerRetParam3 *)fun_ptr))(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
					);
				}
				break;
			case 4:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM4(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}else{
					result=(*((FunctionMemberPointerRetParam4 *)fun_ptr))(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
					);
				}
				break;
			case 5:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM5(fun_ptr)(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
				  );
				}else{
					result=(*((FunctionMemberPointerRetParam5 *)fun_ptr))(
						converted_param[0]
						,converted_param[1]
						,converted_param[2]
						,converted_param[3]
						,converted_param[4]
					);
				}
				break;
			case 6:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM6(fun_ptr)(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
				  );
				}else{
					result=(*((FunctionMemberPointerRetParam6 *)fun_ptr))(
						converted_param[0]
						 ,converted_param[1]
						 ,converted_param[2]
						 ,converted_param[3]
						 ,converted_param[4]
						 ,converted_param[5]
					);
				}
				break;
			}
		}

		stk_result=this->zs->convertVarToStackElement(result,calling_function->idx_return_type);

		return stk_result;
	}
}