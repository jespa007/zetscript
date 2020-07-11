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

	StackElement  VirtualMachine::callFunction_C(
			intptr_t fun_ptr,
			const ScriptFunction *irfs,
			bool & error,
			StackElement *ptrArg,
			unsigned char n_args,
			Instruction *ins,
			ScriptVar  * this_object
			){

		StackElement callc_result={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

		intptr_t converted_param[MAX_N_ARGS];
		intptr_t result=0;
		StackElement *currentArg;
		current_call_c_function = irfs;
		bool static_ref=irfs->symbol_info.symbol_info_properties&SYMBOL_INFO_PROPERTY_STATIC_REF;
		//float aux_float=0;

		if(n_args>MAX_N_ARGS){
			writeError(SFI_GET_FILE_LINE(irfs,ins),"Max run-time args! (Max:%i Provided:%i)",MAX_N_ARGS,n_args);
			RETURN_ERROR;
		}


		if((irfs->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTY_C_OBJECT_REF) {
			writeError(SFI_GET_FILE_LINE(irfs,ins),"Function is not registered as C");
			RETURN_ERROR;
		}

		if(fun_ptr==0){
			writeError(SFI_GET_FILE_LINE(irfs,ins),"Null function");
			//return &callc_result;//ScriptVar::UndefinedSymbol;
			RETURN_ERROR;
		}

		if((char)irfs->arg_info.size() != n_args){
			writeError(SFI_GET_FILE_LINE(irfs,ins),"C argument VS scrip argument doestn't match sizes");
			RETURN_ERROR;
		}

		if(irfs->arg_info.size() > MAX_N_ARGS){
			writeError(SFI_GET_FILE_LINE(irfs,ins),"Reached max param for C function (Current: %i Max Allowed: %i)",irfs->arg_info.size(),MAX_N_ARGS);
			RETURN_ERROR;
			//return &callc_result;//ScriptVar::UndefinedSymbol;
		}

		// convert parameters script to c...
		for(unsigned char  i = 0; i < n_args;i++){

			if( i==0 && (irfs->symbol_info.symbol_info_properties&(SYMBOL_INFO_PROPERTY_SET_FIRST_PARAMETER_AS_THIS))){
				if(!static_ref){
					writeError(SFI_GET_FILE_LINE(irfs,ins),"Internal error: Cannot set parameter as this object due is not static");
					RETURN_ERROR;
				}

				if(this_object==NULL){
					writeError(SFI_GET_FILE_LINE(irfs,ins),"Internal error: Cannot set parameter as this object due this object is NULL");
					RETURN_ERROR;
				}

				converted_param[0]=(intptr_t)this_object->Get_C_Object();
			}
			else{
				currentArg=&ptrArg[i];

				if(!zs->stackElementToVar(currentArg,irfs->arg_info[i].idx_type,(intptr_t *)&converted_param[i],error_str)){
					writeError(SFI_GET_FILE_LINE(irfs,ins),"Function %s, param %i: %s. The function C %s that was found for first time it has different argument types now.",
																	irfs->symbol_info.symbol->name.c_str(),
																	i,
																	error_str.c_str(),
																	irfs->symbol_info.symbol->name.c_str()
																	);
					RETURN_ERROR;
				}
			}
		}

		ZS_PRINT_DEBUG("pre_call %i",n_args);

		if(irfs->idx_return_type == IDX_BUILTIN_TYPE_VOID_C){ // getInstance()->getIdxClassVoid()){

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
					PTR_FUNCTION_VOID_PARAM1(
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

		}else if(irfs->idx_return_type==IDX_BUILTIN_TYPE_BOOL_C){  // we must do a bool cast in order to get float return.
			switch(n_args){
			case 0:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM0(fun_ptr)();
				}else{
					result=(*((CFunctionMemberPointerRetParam0 *)fun_ptr)).ret_bool();
				}
				break;
			case 1:
				if(static_ref){
					result=PTR_FUNCTION_RET_BOOL_PARAM1(fun_ptr)(
						converted_param[0]
					);
				}else{
					result=(*((CFunctionMemberPointerRetParam1 *)fun_ptr)).ret_bool(
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
					result=(*((CFunctionMemberPointerRetParam2 *)fun_ptr)).ret_bool(
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
					result=(*((CFunctionMemberPointerRetParam3 *)fun_ptr)).ret_bool(
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
					result=(*((CFunctionMemberPointerRetParam4 *)fun_ptr)).ret_bool(
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
					result=(*((CFunctionMemberPointerRetParam5 *)fun_ptr)).ret_bool(
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
					result=(*((CFunctionMemberPointerRetParam6 *)fun_ptr))(
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
		}else if(irfs->idx_return_type==IDX_BUILTIN_TYPE_FLOAT_C){ // we must do a float cast in order to get float return.
			float aux_flt;
			switch(n_args){
			case 0:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM0(fun_ptr)();
				}else{
					aux_flt=(*((CFunctionMemberPointerRetParam0 *)fun_ptr)).ret_float();
				}
				break;
			case 1:
				if(static_ref){
					aux_flt=PTR_FUNCTION_RET_FLOAT_PARAM1(fun_ptr)(
						converted_param[0]
					);
				}else{
					aux_flt=(*((CFunctionMemberPointerRetParam1 *)fun_ptr)).ret_float(
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
					aux_flt=(*((CFunctionMemberPointerRetParam2 *)fun_ptr)).ret_float(
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
					aux_flt=(*((CFunctionMemberPointerRetParam3 *)fun_ptr)).ret_float(
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
					aux_flt=(*((CFunctionMemberPointerRetParam4 *)fun_ptr)).ret_float(
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
					aux_flt=(*((CFunctionMemberPointerRetParam5 *)fun_ptr)).ret_float(
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
					aux_flt=(*((CFunctionMemberPointerRetParam6 *)fun_ptr))(
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
					result=(*((CFunctionMemberPointerRetParam0 *)fun_ptr))();
				}
				break;
			case 1:
				if(static_ref){
					result=PTR_FUNCTION_RET_PARAM1(fun_ptr)(
						converted_param[0]
					);
				}else{
					result=(*((CFunctionMemberPointerRetParam1 *)fun_ptr))(
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
					result=(*((CFunctionMemberPointerRetParam2 *)fun_ptr))(
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
					result=(*((CFunctionMemberPointerRetParam3 *)fun_ptr))(
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
					result=(*((CFunctionMemberPointerRetParam4 *)fun_ptr))(
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
					result=(*((CFunctionMemberPointerRetParam5 *)fun_ptr))(
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
					result=(*((CFunctionMemberPointerRetParam6 *)fun_ptr))(
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

		callc_result=this->zs->varToStackElement(result,irfs->idx_return_type);

		return callc_result;
	}
}
