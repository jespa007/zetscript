
#define PROCESS_MOD_OPERATION \
{ \
	unsigned short properties = GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STK_PROPERTY_ZS_INT){\
			PUSH_INTEGER(STK_VALUE_TO_ZS_INT(stk_result_op1) % STK_VALUE_TO_ZS_INT(stk_result_op2));\
	}\
	else if(properties == (MSK_STK_PROPERTY_ZS_INT|MSK_STK_PROPERTY_FLOAT)){\
			if (STK_VALUE_IS_ZS_INT(stk_result_op1) && STK_VALUE_IS_FLOAT(stk_result_op2)){\
				COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
				PUSH_FLOAT(fmod(STK_VALUE_TO_ZS_INT(stk_result_op1) , f_aux_value2));\
			}else{\
				COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
				PUSH_FLOAT(fmod(f_aux_value1 , STK_VALUE_TO_ZS_INT(stk_result_op2)));\
			}\
	}\
	else if(properties == MSK_STK_PROPERTY_FLOAT){\
			COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
			COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
			PUSH_FLOAT(fmod(f_aux_value1 , f_aux_value2));\
	}\
	else{\
		if(applyMetamethod(\
			calling_function\
			,instruction\
			,BYTE_CODE_METAMETHOD_MOD\
			,stk_result_op1\
			,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\
}

#define PROCESS_ARITHMETIC_OPERATION(__C_OP__, __METAMETHOD__)\
{\
	unsigned short properties = GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STK_PROPERTY_ZS_INT){\
			PUSH_INTEGER(STK_VALUE_TO_ZS_INT(stk_result_op1) __C_OP__ STK_VALUE_TO_ZS_INT(stk_result_op2));\
	}\
	else if(properties == (MSK_STK_PROPERTY_ZS_INT|MSK_STK_PROPERTY_FLOAT)){\
			if (STK_VALUE_IS_ZS_INT(stk_result_op1) && STK_VALUE_IS_FLOAT(stk_result_op2)){\
				COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
				PUSH_FLOAT(STK_VALUE_TO_ZS_INT(stk_result_op1) __C_OP__ f_aux_value2);\
			}else{\
				COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
				PUSH_FLOAT(f_aux_value1 __C_OP__ STK_VALUE_TO_ZS_INT(stk_result_op2));\
			}\
	}\
	else if(properties == MSK_STK_PROPERTY_FLOAT){\
			COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
			COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
			PUSH_FLOAT(f_aux_value1 __C_OP__ f_aux_value2);\
	}\
	else{\
		if(applyMetamethod(\
				calling_function\
				,instruction\
				,__METAMETHOD__\
				,stk_result_op1\
				,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\
}

#define PROCESS_COMPARE_OPERATION(__C_OP__, __METAMETHOD__)\
{\
	unsigned short properties = GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STK_PROPERTY_ZS_INT){\
		PUSH_BOOLEAN(STK_VALUE_TO_ZS_INT(stk_result_op1) __C_OP__ STK_VALUE_TO_ZS_INT(stk_result_op2));\
	}\
	else if(properties & MSK_STK_PROPERTY_BOOL){\
		PUSH_BOOLEAN(STK_VALUE_TO_BOOL(stk_result_op1) __C_OP__ STK_VALUE_TO_BOOL(stk_result_op2));\
	}\
	else if(properties & (MSK_STK_PROPERTY_ZS_INT|MSK_STK_PROPERTY_FLOAT)){\
		if (STK_VALUE_IS_ZS_INT(stk_result_op1) && STK_VALUE_IS_FLOAT(stk_result_op2)){\
			COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
			PUSH_BOOLEAN(STK_VALUE_TO_ZS_INT(stk_result_op1) __C_OP__ f_aux_value2);\
		}else{\
			COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
			PUSH_BOOLEAN(f_aux_value1 __C_OP__ STK_VALUE_TO_ZS_INT(stk_result_op2));\
		}\
	}\
	else if(properties == MSK_STK_PROPERTY_FLOAT){\
		COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);\
		COPY_FLOAT(&f_aux_value2,&stk_result_op2->stk_value);\
		PUSH_BOOLEAN(f_aux_value1 __C_OP__ f_aux_value2);\
	}\
	else if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1) && STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)){\
		PUSH_BOOLEAN(ZS_STRCMP(stk_result_op1->toString().c_str(), __C_OP__ ,stk_result_op2->toString().c_str()));\
	}else if(  (stk_result_op1->properties==MSK_STK_PROPERTY_UNDEFINED || stk_result_op2->properties==MSK_STK_PROPERTY_UNDEFINED)\
			&& (__METAMETHOD__ == BYTE_CODE_METAMETHOD_EQU || __METAMETHOD__ == BYTE_CODE_METAMETHOD_NOT_EQU)\
			){\
		if(properties == MSK_STK_PROPERTY_UNDEFINED){\
			PUSH_BOOLEAN(true  __C_OP__  true);\
		}else{\
			PUSH_BOOLEAN(false);\
		}\
	}else{\
		if(applyMetamethod(\
			 calling_function\
			,instruction\
			, __METAMETHOD__\
			,stk_result_op1\
			,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\
}

#define PROCESS_LOGIC_OPERATION(__C_OP__)\
{\
	unsigned short properties = GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STK_PROPERTY_BOOL){\
		PUSH_BOOLEAN(STK_VALUE_TO_BOOL(stk_result_op1) __C_OP__ STK_VALUE_TO_BOOL(stk_result_op2));\
	}else{\
		PRINT_DUAL_ERROR_OP(STR(__C_OP__));\
		goto lbl_exit_function;\
	}\
}

#define PROCESS_BINARY_OPERATION(__C_OP__, __METAMETHOD__)\
{\
	unsigned short properties = GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_result_op1->properties|stk_result_op2->properties);\
	if(properties == MSK_STK_PROPERTY_ZS_INT){\
		PUSH_INTEGER(STK_VALUE_TO_ZS_INT(stk_result_op1) __C_OP__ STK_VALUE_TO_ZS_INT(stk_result_op2));\
	}else{\
		if(applyMetamethod(\
			calling_function\
			,instruction\
			, __METAMETHOD__\
			,stk_result_op1\
			,stk_result_op2\
		)==false){\
			goto lbl_exit_function;\
		}\
	}\
}

#define PERFORM_POST_OPERATOR(__PRE_OP__,__OPERATOR__) \
{\
	stk_var=--stk_vm_current;\
	if(stk_var->properties & MSK_STK_PROPERTY_PTR_STK){\
		stk_var=(StackElement *)((stk_var)->stk_value);\
	}\
	void **ref=(void **)(&((stk_var)->stk_value));\
	if(stk_var->properties & MSK_STK_PROPERTY_IS_VAR_C){\
		ref=(void **)((stk_var)->stk_value);\
	}\
	switch(GET_STK_PROPERTY_PRIMITIVE_TYPES((stk_var)->properties)){\
	case MSK_STK_PROPERTY_ZS_INT:\
			PUSH_INTEGER(__PRE_OP__(*((zs_int *)(ref))));\
			(*((zs_int *)(ref)))__OPERATOR__;\
			break;\
	case MSK_STK_PROPERTY_FLOAT:\
			PUSH_FLOAT(__PRE_OP__(*((float *)(ref))));\
			(*((float *)(ref)))__OPERATOR__;\
			break;\
	default:\
		VM_STOP_EXECUTE(" Cannot perform pre/post operator (%s)",stk_var->typeStr());\
		break;\
	}\
}

#define PERFORM_PRE_OPERATOR(__OPERATOR__) \
{\
	stk_var=--stk_vm_current;\
	if(stk_var->properties & MSK_STK_PROPERTY_PTR_STK){\
		stk_var=(StackElement *)((stk_var)->stk_value);\
	}\
	void **ref=(void **)(&((stk_var)->stk_value));\
	if(stk_var->properties & MSK_STK_PROPERTY_IS_VAR_C){\
		ref=(void **)((stk_var)->stk_value);\
	}\
	switch(GET_STK_PROPERTY_PRIMITIVE_TYPES((stk_var)->properties)){\
	case MSK_STK_PROPERTY_ZS_INT:\
			(*((zs_int *)(ref)))__OPERATOR__;\
			PUSH_INTEGER(*((zs_int *)(ref)));\
			break;\
	case MSK_STK_PROPERTY_FLOAT:\
			(*((float *)(ref)))__OPERATOR__;\
			PUSH_FLOAT(*((float *)(ref)));\
			break;\
	default:\
		VM_STOP_EXECUTE(" Cannot perform pre/post operator (%s)",stk_var->typeStr());\
		break;\
	}\
}

#define PUSH_VM_SCOPE(_scope,_ptr_info_function, _ptr_local_var,_properties) {\
	if(vm_current_scope >=  vm_scope_max){THROW_RUNTIME_ERROR("reached max scope");}\
	*vm_current_scope++={(Scope *)_scope,_ptr_info_function,_ptr_local_var,_properties};\
}


#define PUSH_UNDEFINED \
*stk_vm_current++=stk_undefined; \

#define PUSH_BOOLEAN(init_value) \
stk_vm_current->stk_value=(void *)((zs_int)(init_value)); \
stk_vm_current->properties=MSK_STK_PROPERTY_BOOL; \
stk_vm_current++;


#define PUSH_INTEGER(init_value) \
stk_vm_current->stk_value=(void *)((zs_int)(init_value)); \
stk_vm_current->properties=MSK_STK_PROPERTY_ZS_INT; \
stk_vm_current++;

#define PUSH_ARRAY_STK(n_stk) \
stk_vm_current->stk_value=(void *)((zs_int)(n_stk)); \
stk_vm_current->properties=MSK_STK_PROPERTY_ARRAY_STK; \
stk_vm_current++;

#define PUSH_CONST_CHAR(const_char) \
stk_vm_current->stk_value=(void *)((zs_int)(const_char)); \
stk_vm_current->properties=MSK_STK_PROPERTY_CONST_CHAR; \
stk_vm_current++;

#define PUSH_OBJECT(obj_value) \
stk_vm_current->stk_value=(void *)((zs_int)(obj_value)); \
stk_vm_current->properties=MSK_STK_PROPERTY_SCRIPT_OBJECT; \
stk_vm_current++;


#define PUSH_FLOAT(init_value) \
{\
	float aux=(float)(init_value); \
	COPY_FLOAT(&stk_vm_current->stk_value,&aux); \
	stk_vm_current->properties=MSK_STK_PROPERTY_FLOAT; \
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



#define LOAD_FROM_STACK(offset,properties) \
	 ((properties) & MSK_INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL) ? _stk_local_var+offset \
	:((properties) & MSK_INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_MEMBER) ? this_object->getElementAt(offset) \
	:vm_stack + offset\



/*
PERFORM_PRE_OPERATION(dst_var,stk_var);
PERFORM_POST_OPERATION(stk_var);
*/



#define POP_TWO \
    switch(instruction->properties & MSK_INSTRUCTION_PROPERTY_ILOAD){\
    default:\
    case 0:\
		stk_result_op2=--stk_vm_current;\
		stk_result_op1=--stk_vm_current;\
		break;\
    case MSK_INSTRUCTION_PROPERTY_ILOAD_K: /* only perfom with one constant*/\
		 stk_result_op1=--stk_vm_current;\
		 stk_result_op2=&stk_constant_aux;\
         stk_result_op2->stk_value=(void *)instruction->value_op2;\
         stk_result_op2->properties = INSTRUCTION_CONST_TO_STK_CONST_PROPERTY(instruction->properties);\
         break;\
    case MSK_INSTRUCTION_PROPERTY_ILOAD_R: /* only perfom with one Register */\
		 stk_result_op1=--stk_vm_current;\
         stk_result_op2=LOAD_FROM_STACK(instruction->value_op1,instruction->properties);\
         break;\
    case MSK_INSTRUCTION_PROPERTY_ILOAD_KR: /* perfom Konstant-Register*/\
		stk_result_op1=&stk_constant_aux;\
        stk_result_op1->stk_value=(void *)instruction->value_op2;\
        stk_result_op1->properties = INSTRUCTION_CONST_TO_STK_CONST_PROPERTY(instruction->properties);\
        stk_result_op2=LOAD_FROM_STACK(instruction->value_op1,instruction->properties);\
        break;\
    case MSK_INSTRUCTION_PROPERTY_ILOAD_RK: /* perfom Register-Konstant */\
        stk_result_op1=LOAD_FROM_STACK(instruction->value_op1,instruction->properties);\
        stk_result_op2=&stk_constant_aux;\
        stk_result_op2->stk_value=(void *)instruction->value_op2;\
        stk_result_op2->properties = INSTRUCTION_CONST_TO_STK_CONST_PROPERTY(instruction->properties);\
        break;\
   case MSK_INSTRUCTION_PROPERTY_ILOAD_RR: /* perfom Register-Register*/ \
        stk_result_op1=LOAD_FROM_STACK(instruction->value_op1,instruction->properties);\
        stk_result_op2=LOAD_FROM_STACK(((instruction->value_op2&0xff0000)>>16),instruction->value_op2);\
        break;\
    }


#define READ_TWO_POP_ONE \
stk_result_op2=--stk_vm_current;\
stk_result_op1=(stk_vm_current-1);

#define POP_ONE \
stk_result_op1=--stk_vm_current;

#define PUSH_STK_PTR(stk_ptr) \
	stk_vm_current->stk_value=(stk_ptr);\
	stk_vm_current->properties=MSK_STK_PROPERTY_PTR_STK;\
	stk_vm_current++;


namespace zetscript{

	void VirtualMachine::callFunctionScript(
			ScriptObjectAnonymous	* this_object,
			ScriptFunction 			* calling_function,
			StackElement 		  	* _stk_local_var,
			//std::string 		  	* _str_start,
			unsigned char 			n_args
	    ){

		//StackElement  stk_result=stk_undefined;
		Instruction * instruction = NULL;//calling_instruction;

		if(vm_idx_call >= MAX_FUNCTION_CALL){
			VM_ERROR_AND_RET("Reached max stack");
		}

		if(calling_function->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF){
			VM_ERROR_AND_RET("Internal error: Not script function");
		}

		float aux_float=0.0;
		ScriptObject *script_var=NULL;
		ScriptObjectAnonymous *script_object_anonymous_aux=NULL;
		ScriptObjectClass *script_object_class=NULL;
		ScriptObjectAnonymous *calling_object=NULL; // used on call instruction
		StackElement *stk_result_op1=NULL;
		StackElement *stk_result_op2=NULL;
		StackElement stk_constant_aux;
		StackElement *stk_var;
		const char *str_symbol=NULL,*str_aux=NULL;
		Symbol * symbol_not_defined=NULL;
		int idx_stk_element;

		unsigned short pre_post_properties=0;
		ScriptObject *var_object = NULL;

		unsigned short scope_type=0;

		StackElement *stk_dst=NULL;
		StackElement *stk_src=NULL;

		Instruction *instructions=calling_function->instructions; // starting instruction
		Instruction *instruction_it=instructions;

		VM_Scope * vm_scope_start=vm_current_scope; // save current scope...

		Scope * scope = calling_function->symbol.scope;// ast->idx_scope;

		vm_idx_call++;

		zs_vector *registered_symbols=calling_function->registered_symbols;
		ZS_PRINT_DEBUG("Executing function %s ...",calling_function->symbol.name.c_str());

		//=========================================
		// PUSH STACK
		// reserve vars and assign argv vars ...
		unsigned symbols_count=registered_symbols->count;
		StackElement *stk_start=&_stk_local_var[symbols_count];   // <-- here starts stk for aux vars for operations ..

		if(stk_start >= &vm_stack[VM_STACK_LOCAL_VAR_MAX-1]){
			VM_STOP_EXECUTE("Error MAXIMUM stack size reached");
		}

		// stk_vm_current starts always from stk_start to avoid overwritting local variables...
		stk_vm_current = stk_start;

		// Init local vars ...
		if((calling_function->idx_script_function != IDX_SCRIPT_FUNCTION_MAIN) && (vm_idx_call > 1)){
			StackElement *ptr_aux = _stk_local_var+n_args;
			for(unsigned i = n_args; i < (symbols_count); i++){ // from n_args, setup local vars
				Symbol *symbol=(Symbol *)registered_symbols->items[i];
				if(symbol->properties & SYMBOL_PROPERTY_FUNCTION){
					ptr_aux->stk_value=(void *)symbol->ref_ptr;
					ptr_aux->properties=MSK_STK_PROPERTY_FUNCTION;

				}else{
					*ptr_aux=stk_undefined;		// undefined as default
				}

				ptr_aux++;
			}
		}

		// PUSH STACK
		//=========================================

		//-----------------------------------------------------------------------------------------------------------------------
		//
		// MAIN LOOP
		//
		 for(;;){ // foreach byte code instruction ...

			instruction = instruction_it++;

			switch(instruction->byte_code){
			default:
				VM_STOP_EXECUTE("byte code \"%s\" implemented",ByteCodeToStr(instruction->byte_code));
			case BYTE_CODE_END_FUNCTION:
				goto lbl_exit_function;
			case BYTE_CODE_FIND_VARIABLE:

				str_symbol=SFI_GET_SYMBOL_NAME(calling_function,instruction);

				if((str_aux=strstr(str_symbol,"::")) != NULL){ // static access
					ScriptClass *static_class=NULL;
					std::string static_error;
					char copy_aux[512]={0};

					// get class
					strncpy(copy_aux,str_symbol,str_aux-str_symbol);

					static_class=zs->getScriptClassFactory()->getScriptClass(copy_aux);

					if(static_class==NULL){
						VM_STOP_EXECUTE("Cannot link static access '%s' class '%s' not exist"
							,str_symbol
							,copy_aux
						);
					}

					// advance ::
					str_aux+=2;

					//get member
					strcpy(copy_aux,str_aux);

					symbol_not_defined=static_class->getSymbol(copy_aux); // ... and member as well we can define the instruction here

					if(symbol_not_defined == NULL){
						VM_STOP_EXECUTE("Cannot link static access '%s' class '%s' not exist"
							,str_symbol
							,copy_aux
						);
					}

					if(!eval::eval_set_instruction_static_symbol(instruction,symbol_not_defined,static_error)){

						VM_STOP_EXECUTE("Symbol \"%s\" %s"
							,str_symbol
							,static_error.c_str()
						);
					}

				}else{
					symbol_not_defined = main_function_object->getSymbol(MAIN_SCOPE(this),str_symbol);//, NO_PARAMS_SYMBOL_ONLY, ScopeDirection::SCOPE_DIRECTION_DOWN);
				}

				if(symbol_not_defined != NULL){
					if(symbol_not_defined->n_params==NO_PARAMS_SYMBOL_ONLY){ // variable
						instruction->byte_code=BYTE_CODE_LOAD_GLOBAL;
						PUSH_STK_PTR(vm_stack + instruction->value_op2);
					}else{ // function
						// assign script function ...
						instruction->byte_code=BYTE_CODE_LOAD_FUNCTION;
						instruction->value_op2=symbol_not_defined->ref_ptr;
						PUSH_FUNCTION(instruction->value_op2);
					}
				}else{ // load undefined as default!
					VM_STOP_EXECUTE("Symbol \"%s\" is not defined",str_symbol);
				}

				continue;
			case BYTE_CODE_LOAD_GLOBAL: // load variable ...
				PUSH_STK_PTR(vm_stack + instruction->value_op2);
				continue;
			case BYTE_CODE_LOAD_LOCAL: // load variable ...
				PUSH_STK_PTR(_stk_local_var+instruction->value_op2);
				continue;
			case BYTE_CODE_LOAD_THIS: // load variable ...
				PUSH_STK_PTR(this_object->getThisProperty());
				continue;
			case BYTE_CODE_LOAD_MEMBER: // direct load
				PUSH_STK_PTR(this_object->getElementAt(instruction->value_op2));
				continue;
			case BYTE_CODE_LOAD_ELEMENT_VECTOR:
				POP_TWO;
				var_object=NULL;
				if(stk_result_op1->properties & MSK_STK_PROPERTY_PTR_STK){
					stk_result_op1 = (StackElement *)stk_result_op1->stk_value;
				}

				if(stk_result_op1->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
					var_object = (ScriptObjectAnonymous *)(stk_result_op1->stk_value);
				}

				stk_var=NULL;

				if(var_object != NULL){
					if(var_object->idx_script_class == IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_VECTOR){
						ScriptObjectVector * var_vector = (ScriptObjectVector *)var_object;
						if(STK_VALUE_IS_ZS_INT(stk_result_op2)==false){
							VM_STOP_EXECUTE("Expected std::vector-index as integer or std::string");
						}

						// determine object ...
						if((stk_var =var_vector->getElementAt(STK_VALUE_TO_ZS_INT(stk_result_op2)))==NULL){
							goto lbl_exit_function;
						}
					}
				}

				if(stk_var == NULL){ // push undefined
					PUSH_UNDEFINED;
					continue;
				}

				PUSH_STK_PTR(stk_var);

				//PERFORM_PRE_PUSH_POST_OPERATION;
				continue;
			case BYTE_CODE_LOAD_ELEMENT_OBJECT:
			case BYTE_CODE_LOAD_ELEMENT_THIS:
load_element_object:

				stk_var=NULL;
				script_object_anonymous_aux=this_object; // take this as default
				if(instruction->byte_code == BYTE_CODE_LOAD_ELEMENT_OBJECT){

					Instruction *previous_ins= (instruction-1);

					if(previous_ins->byte_code == BYTE_CODE_NEW_CLASS){
						stk_result_op1=(stk_vm_current-1);
					}
					else{
						POP_ONE; // get var op1 and symbol op2
					}

					/*if((stk_result_op1->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT)!= MSK_STK_PROPERTY_SCRIPT_OBJECT)
					{
						VM_STOP_EXECUTE(
							"Cannot perform access operation [ ... %s.%s ]. Expected \"%s\" as object but is type \"%s\""
							,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
							,SFI_GET_SYMBOL_NAME(calling_function,instruction)
							,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
							,stk_result_op1->typeStr()
						);
					}*/

					if(stk_result_op1->properties & MSK_STK_PROPERTY_PTR_STK) {
						stk_result_op1=((StackElement *)stk_result_op1->stk_value);
					}

					if((stk_result_op1->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT)!= MSK_STK_PROPERTY_SCRIPT_OBJECT)
					{
						VM_STOP_EXECUTE(
							"Cannot perform access operation [ ... %s.%s ]. Expected \"%s\" as object but is type \"%s\""
							,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
							,SFI_GET_SYMBOL_NAME(calling_function,instruction)
							,SFI_GET_SYMBOL_NAME(calling_function,instruction-1)
							,stk_result_op1->typeStr()
						);
					}

					script_object_anonymous_aux=((ScriptObjectAnonymous *)stk_result_op1->stk_value);


					if(script_object_anonymous_aux == NULL)
					{
						VM_STOP_EXECUTE("var \"%s\" is not scriptvariable",SFI_GET_SYMBOL_NAME(script_object_anonymous_aux,previous_ins));
					}


				}else{ // check whether if exist or not
					if(instruction->value_op2>=0){ // on dynamic members fix idx when we find a C function
						stk_var=this_object->getElementAt(instruction->value_op2);
					}
				}

				if(stk_var==NULL){ // load element from object or dynamic member element from this
					// get dynamic property
					if(instruction->value_op2==ZS_IDX_INSTRUCTION_OP2_CONSTRUCTOR){
						str_symbol=(char *)script_object_anonymous_aux->getScriptClass()->symbol_class.name.c_str(); //FUNCTION_MEMBER_CONSTRUCTOR_NAME;
					}else{
						str_symbol=(char *)SFI_GET_SYMBOL_NAME(calling_function,instruction);
					}

					if((stk_var=script_object_anonymous_aux->getProperty(str_symbol, &idx_stk_element)) == NULL){

						// something went wrong
						if(vm_error == true){
							goto lbl_exit_function;
						}

						// pack member info for store information...
						if(instruction->properties & MSK_INSTRUCTION_PROPERTY_PACK_MEMBER_INFO){
							// save
							PUSH_CONST_CHAR(str_symbol);
							PUSH_OBJECT(calling_object);
							PUSH_ARRAY_STK(2);
						}
						else{
							stk_vm_current->stk_value=0;
							stk_vm_current->properties=MSK_STK_PROPERTY_UNDEFINED;
							stk_vm_current++;
						}
						continue;
					}
				}
				PUSH_STK_PTR(stk_var);

				if((instruction+1)->byte_code == BYTE_CODE_LOAD_ELEMENT_OBJECT){ // fast load access without pass through switch instruction
					instruction++;
					goto load_element_object;
				}

				continue;
			case BYTE_CODE_LOAD_UNDEFINED:
				PUSH_UNDEFINED;
				continue;
			case BYTE_CODE_LOAD_FUNCTION: // expect constant and function has the same behaviour...
				PUSH_FUNCTION(instruction->value_op2);
				continue;
			case BYTE_CODE_LOAD_ZS_INT:
				stk_vm_current->stk_value=(void *)instruction->value_op2;
				stk_vm_current->properties=MSK_STK_PROPERTY_ZS_INT;
				stk_vm_current++;
				continue;
			case BYTE_CODE_LOAD_FLOAT:
				stk_vm_current->stk_value=(void *)instruction->value_op2;
				stk_vm_current->properties=MSK_STK_PROPERTY_FLOAT;
				stk_vm_current++;
				continue;
			case BYTE_CODE_LOAD_BOOL:
				stk_vm_current->stk_value=(void *)instruction->value_op2;
				stk_vm_current->properties=MSK_STK_PROPERTY_BOOL;
				stk_vm_current++;
				continue;
			case BYTE_CODE_LOAD_STACK_ELEMENT:
			case BYTE_CODE_LOAD_STRING:
				*stk_vm_current++=*((StackElement *)instruction->value_op2);
				continue;
			case BYTE_CODE_LOAD_CLASS:
				PUSH_CLASS(instruction->value_op2);
				continue;
			case BYTE_CODE_STORE:
			case BYTE_CODE_PUSH_VECTOR_ELEMENT:
			case BYTE_CODE_PUSH_OBJECT_ELEMENT:
			case BYTE_CODE_STORE_CONST:
			case BYTE_CODE_STORE_ADD:
			case BYTE_CODE_STORE_SUB:
			case BYTE_CODE_STORE_MUL:
			case BYTE_CODE_STORE_DIV:
			case BYTE_CODE_STORE_MOD:
			case BYTE_CODE_STORE_AND:
			case BYTE_CODE_STORE_OR:
			case BYTE_CODE_STORE_XOR:
			case BYTE_CODE_STORE_SHL:
			case BYTE_CODE_STORE_SHR:

				{
					bool assign_metamethod=false;
					char n_elements_left=0;
					StackElement *stk_multi_var_src=NULL;

					if(instruction->byte_code==BYTE_CODE_PUSH_VECTOR_ELEMENT){
						POP_ONE; // only pops the value, the last is the std::vector variable itself
						ScriptObjectAnonymous *vec_obj = NULL;
						if((stk_vm_current-1)->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
							vec_obj = (ScriptObjectAnonymous *)(stk_vm_current-1)->stk_value;
							if(vec_obj->idx_script_class == IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_VECTOR){ // push value ...
								// op1 is now the src value ...
								stk_src=stk_result_op1;
								if(stk_src->properties & MSK_STK_PROPERTY_PTR_STK){
									stk_src=(StackElement *)stk_result_op1->stk_value;
								}

								stk_dst=((ScriptObjectVector *)vec_obj)->newSlot();
							}
						}

						if(vec_obj==NULL){
							VM_STOP_EXECUTE("Expected std::vector object");
						}
					}else if(instruction->byte_code==BYTE_CODE_PUSH_OBJECT_ELEMENT){

						POP_TWO; // first must be a string that describes variable name and the other the variable itself ...
						ScriptObjectAnonymous *obj = NULL;
						StackElement *stk_object=(stk_vm_current-1);
						if(STK_IS_SCRIPT_OBJECT_ANONYMOUS(stk_object)){
							VM_STOP_EXECUTE("Expected object but is type ");
						}

						obj = (ScriptObjectAnonymous *)stk_object->stk_value;
						if(obj->idx_script_class != IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_CLASS){ // push value ...
							VM_STOP_EXECUTE("Expected object but is type ");
						}

						if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1)){
							VM_STOP_EXECUTE("internal error (operator2 is not std::string)");
						}
								// op1 is now the src value ...
						StackElement *se=NULL;
						const char *str = (const char *)stk_result_op1->stk_value;
						stk_src=stk_result_op2;
						if((se =obj->addProperty(str,vm_error_str))==NULL){
							VM_STOP_EXECUTE(vm_error_str.c_str());
						}

						stk_dst=se;

					}
					else{ // can be assign or arithmetic and assing pop two parameters nothing ...

						n_elements_left=(char)instruction->value_op1;

						if(instruction->byte_code==BYTE_CODE_STORE && n_elements_left > 0){

							StackElement *stk_aux=stk_vm_current;

							//stk_vm_current--; // set to 0

							// set stack elements to the top
							for(int i=0; i < n_elements_left; i++){

								stk_aux--;

								if(stk_aux->properties & MSK_STK_PROPERTY_ARRAY_STK){
									if((zs_int)stk_aux->stk_value != 2){
										VM_STOP_EXECUTE("Internal expected array stk = 2");
									}
									stk_aux-=2;
								}else if ((stk_aux->properties & MSK_STK_PROPERTY_PTR_STK) == 0){
									VM_STOP_EXECUTE("Expected stk ptr or array stk");
								}
							}

							stk_multi_var_src=stk_aux-n_elements_left-1;
							n_elements_left=n_elements_left-1;
							stk_result_op2=--stk_vm_current;
							stk_result_op1=++stk_multi_var_src;


						}else{
							READ_TWO_POP_ONE

							if(IS_BYTE_CODE_STORE_WITH_OPERATION(instruction->byte_code)){ // arithmetic

								switch(instruction->byte_code){
								case BYTE_CODE_STORE_ADD:
									PROCESS_ARITHMETIC_OPERATION(+,BYTE_CODE_METAMETHOD_ADD);
									break;
								case BYTE_CODE_STORE_SUB:
									PROCESS_ARITHMETIC_OPERATION(-,BYTE_CODE_METAMETHOD_SUB);
									break;
								case BYTE_CODE_STORE_MUL:
									PROCESS_ARITHMETIC_OPERATION(*,BYTE_CODE_METAMETHOD_MUL);
									break;
								case BYTE_CODE_STORE_DIV:
									PROCESS_ARITHMETIC_OPERATION(/,BYTE_CODE_METAMETHOD_DIV);
									break;
								case BYTE_CODE_STORE_MOD:
									PROCESS_MOD_OPERATION;
									break;
								case BYTE_CODE_STORE_AND:
									PROCESS_BINARY_OPERATION(&,BYTE_CODE_METAMETHOD_ADD);
									break;
								case BYTE_CODE_STORE_OR:
									PROCESS_BINARY_OPERATION(|,BYTE_CODE_METAMETHOD_OR);
									break;
								case BYTE_CODE_STORE_XOR:
									PROCESS_BINARY_OPERATION(^,BYTE_CODE_METAMETHOD_XOR);
									break;
								case BYTE_CODE_STORE_SHL:
									PROCESS_BINARY_OPERATION(<<,BYTE_CODE_METAMETHOD_SHL);
									break;
								case BYTE_CODE_STORE_SHR:
									PROCESS_BINARY_OPERATION(>>,BYTE_CODE_METAMETHOD_SHR);
									break;

								}

							}
						}

	vm_store_next:

						stk_dst=stk_result_op2;

						if(stk_dst->properties & MSK_STK_PROPERTY_READ_ONLY){
							VM_STOP_EXECUTE("Assignment to constant variable");
						}

						// TODO: JEB get calling element
						if(stk_dst->properties & MSK_STK_PROPERTY_PTR_STK) {
							stk_dst=(StackElement *)stk_dst->stk_value; // stk_value is expect to contents a stack variable
						}else if(stk_dst->properties & MSK_STK_PROPERTY_ARRAY_STK){ // we have member information
							//
							if((zs_int)stk_dst->stk_value != 2){
								VM_STOP_EXECUTE("Internal error, store on packed info is not length 2");
							}

							StackElement *stk_pack_info=stk_dst;
							StackElement *stk_calling_object_info=--stk_vm_current;
							StackElement *stk_symbol_info=--stk_vm_current;


							if((stk_symbol_info->properties != MSK_STK_PROPERTY_CONST_CHAR)
								&& (stk_calling_object_info->properties != MSK_STK_PROPERTY_SCRIPT_OBJECT)){
									VM_STOP_EXECUTE("Internal error, store on packed member info has not object and const char information");
							}

							ScriptObjectAnonymous *calling_object_info=(ScriptObjectAnonymous *)stk_calling_object_info->stk_value;// calling object
							if((stk_dst=calling_object_info->addProperty((const char *)stk_symbol_info->stk_value, vm_error_str))==NULL){
								VM_STOP_EXECUTE(vm_error_str.c_str());
							}


						}else{
							VM_STOP_EXECUTE("Expected l-value on assignment but it was type \"%s\"",stk_dst->typeStr());
						}


						stk_src=stk_result_op1; // store ptr instruction2 op as src_var_value

						// we need primitive stackelement in order to assign...
						if(stk_src->properties & MSK_STK_PROPERTY_PTR_STK) {// == ScriptObjectAnonymous::VAR_TYPE::OBJECT){
							stk_src=(StackElement *)stk_src->stk_value; // stk_value is expect to contents a stack variable
						}

						// ok load object pointer ...
						if(STK_IS_SCRIPT_OBJECT_CLASS(stk_dst)){

							ScriptObjectClass *script_object_class=((ScriptObjectClass *)stk_dst->stk_value);

							if(script_object_class->itHasSetMetamethod()){
								if(applyMetamethod(
										 calling_function
										,instruction
										,BYTE_CODE_METAMETHOD_SET
										,stk_result_op1 // it contents variable to be assigned
										,stk_result_op2 // it contects the result of expression or whatever
								)==false){
									goto lbl_exit_function;
								}
								assign_metamethod=true;
							}

							if(STK_IS_THIS(stk_dst)){
								VM_STOP_EXECUTE("\"this\" is not assignable");
							}
						}
					}

					if(! assign_metamethod){

						StackElement old_stk_dst = *stk_dst; // save dst_var to check after assignment...
						{

							void *copy_aux=NULL;/*copy aux in case of the var is c and primitive (we have to update stk_value on save) */
							void **stk_src_ref=&stk_src->stk_value;
							void **stk_dst_ref=&stk_dst->stk_value;
							if(stk_src->properties & MSK_STK_PROPERTY_IS_VAR_C){
								stk_src_ref=(void **)((stk_src)->stk_value);
							}
							if(stk_dst->properties & MSK_STK_PROPERTY_IS_VAR_C){
								if(!STK_VALUE_IS_FLOAT_OR_INT(stk_src) && STK_VALUE_IS_FLOAT_OR_INT(stk_dst)){
									if(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_src->properties) != GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_dst->properties)
									){
										VM_STOP_EXECUTE("different types! dst var is native (i.e embedd C++) and cannot change its type. dest and src must be equals",SFI_GET_SYMBOL_NAME(calling_function,instruction));
									}else{
										if(
											(stk_src->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT)
										){
											VM_STOP_EXECUTE("Assign native C scriptvar is not allowed to avoid memory leaks. Define '=' operator in order to make the proper operation.");
										}
									}
								}
								stk_dst_ref=(void **)((stk_dst)->stk_value);
								copy_aux=&((stk_dst)->stk_value);
							}
							unsigned short type_var=stk_src->properties;
							//unsigned short runtime_var=0; /* there's no reason to heredate runtime_props ?!? GET_MSK_STK_PROPERTY_RUNTIME(type_var);*/

							// init stk_dst
							*stk_dst = stk_undefined;

							if(type_var == MSK_STK_PROPERTY_UNDEFINED){
								stk_dst->properties=MSK_STK_PROPERTY_UNDEFINED;
							}else if(type_var & MSK_STK_PROPERTY_ZS_INT){
								stk_dst->properties=MSK_STK_PROPERTY_ZS_INT;
								*((zs_int *)stk_dst_ref)=*((zs_int *)stk_src_ref);
								if(copy_aux!=NULL)(*(zs_int *)copy_aux)=*((zs_int *)stk_src_ref);
							}else if(type_var & MSK_STK_PROPERTY_FLOAT){
								stk_dst->properties=MSK_STK_PROPERTY_FLOAT;
								*((float *)stk_dst_ref)=*((float *)stk_src_ref);
								if(copy_aux!=NULL)(*(float *)copy_aux)=*((float *)stk_src_ref);
							}else if(type_var & MSK_STK_PROPERTY_BOOL){
								stk_dst->properties=MSK_STK_PROPERTY_BOOL;
								*((bool *)stk_dst_ref)=*((bool *)stk_src_ref);
								if(copy_aux!=NULL)(*(bool *)copy_aux)=*((bool *)stk_src_ref);
							}else if(type_var  &  (MSK_STK_PROPERTY_FUNCTION | MSK_STK_PROPERTY_CLASS) ){
								*stk_dst=*stk_src;
							}else if(type_var & MSK_STK_PROPERTY_SCRIPT_OBJECT){

								if(STK_IS_SCRIPT_OBJECT_STRING(stk_src)){
									ScriptObjectString *str_object=NULL;

									if(STK_IS_SCRIPT_OBJECT_STRING(stk_dst)){ // dst is string reload
										str_object=(ScriptObjectString *)stk_dst->stk_value;
									}else{ // Generates a std::string var
										stk_dst->stk_value=str_object= NEW_STRING_VAR;
										stk_dst->properties=MSK_STK_PROPERTY_SCRIPT_OBJECT;
										if(!str_object->initSharedPtr()){
											goto lbl_exit_function;
										}
										if(!sharePointer(str_object->shared_pointer)){
											goto lbl_exit_function;
										}
									}

									str_object->set(stk_src->toString());

								}else{ // object we pass its reference

									script_var=(ScriptObject *)stk_src->stk_value;

									stk_dst->stk_value=script_var;
									stk_dst->properties=MSK_STK_PROPERTY_SCRIPT_OBJECT;

									if(!STK_IS_THIS(stk_src)){ // do not share this!
										if(!sharePointer(script_var->shared_pointer)){
											goto lbl_exit_function;
										}
									}
								}
							}else{
								VM_STOP_EXECUTE("(internal) cannot determine var type %s",stk_src->typeStr());
							}
							if(copy_aux!=NULL)stk_dst->properties|=MSK_STK_PROPERTY_IS_VAR_C;
						}

						// check old var structure ...
						switch(GET_MSK_STK_PROPERTY_TYPES(old_stk_dst.properties)){
						case MSK_STK_PROPERTY_UNDEFINED:
						case MSK_STK_PROPERTY_ZS_INT:
						case MSK_STK_PROPERTY_FLOAT:
						case MSK_STK_PROPERTY_BOOL:
						case MSK_STK_PROPERTY_FUNCTION: // we aren't take care about nothing! :)
							break;
						case MSK_STK_PROPERTY_SCRIPT_OBJECT: // we are getting script vars ...
							if((old_stk_dst.properties & (MSK_STK_PROPERTY_IS_VAR_C))==(MSK_STK_PROPERTY_IS_VAR_C)==0){ // is not C class
								if(old_stk_dst.stk_value!=NULL){ // it had a pointer (no constant)...
									//ScriptObjectAnonymous *old_script_dst_var_ref=(ScriptObjectAnonymous *)(old_stk_dst.var_ref);
									if(
										old_stk_dst.stk_value != stk_dst->stk_value  // not same ref ...
									&&  STK_IS_THIS(&old_stk_dst)  // ... or this, do not share/unshare
									){ // unref pointer because new pointer has been attached...
										if(!unrefSharedScriptObject(((ScriptObjectAnonymous  *)old_stk_dst.stk_value)->shared_pointer,vm_idx_call)){
											goto lbl_exit_function;
										}
									}
								}
							}
							break;
						}
					}

					if(instruction->byte_code ==BYTE_CODE_STORE_CONST){
						stk_dst->properties |= MSK_STK_PROPERTY_READ_ONLY;
					}

					if(n_elements_left > 0){
						n_elements_left--;
						//stk_vm_current++;
						//POP_TWO;
						stk_result_op2=--stk_vm_current;//stk_multi_var_dest++; // left assigment
						stk_result_op1=++stk_multi_var_src; // result on the right

						goto vm_store_next;
					}
					else{
						/*if((instruction+1)->byte_code ==BYTE_CODE_RESET_STACK // it marks end expression so ignore it
						){
							instruction++;
						}else{ // push to allow eval multi assigment*/
						*stk_vm_current++=*stk_dst;
						//}
					}
				}
				continue;
			case BYTE_CODE_EQU:  // ==
				POP_TWO;
				PROCESS_COMPARE_OPERATION(==, BYTE_CODE_METAMETHOD_EQU);
				continue;
			case BYTE_CODE_NOT_EQU:  // !=
				POP_TWO;
				PROCESS_COMPARE_OPERATION(!=, BYTE_CODE_METAMETHOD_NOT_EQU);
				continue;
			case BYTE_CODE_LT:  // <
				POP_TWO;
				PROCESS_COMPARE_OPERATION(<, BYTE_CODE_METAMETHOD_LT);
				continue;
			case BYTE_CODE_LTE:  // <=
				POP_TWO;
				PROCESS_COMPARE_OPERATION(<=, BYTE_CODE_METAMETHOD_LTE);
				continue;
			case BYTE_CODE_GT:  // >
				POP_TWO;
				PROCESS_COMPARE_OPERATION(>,BYTE_CODE_METAMETHOD_GT);
				continue;
			case BYTE_CODE_GTE:  // >=
				POP_TWO;
				PROCESS_COMPARE_OPERATION(>=,BYTE_CODE_METAMETHOD_GTE);
				continue;
			case BYTE_CODE_LOGIC_AND:  // &&
				POP_TWO;
				PROCESS_LOGIC_OPERATION(&&);
				continue;
			case BYTE_CODE_LOGIC_OR:  // ||
				POP_TWO;
				PROCESS_LOGIC_OPERATION(||);
				continue;
			case BYTE_CODE_NOT: // !
				POP_ONE;
				if(stk_result_op1->properties & MSK_STK_PROPERTY_BOOL){ // operation will result as integer.
					PUSH_BOOLEAN((!((bool)(stk_result_op1->stk_value))));
				}else{
					if(applyMetamethod(
						calling_function
						,instruction
						,BYTE_CODE_METAMETHOD_NOT
						,stk_result_op1
						,stk_result_op2
					)==false){
						goto lbl_exit_function;
					}
				}
				continue;
			case BYTE_CODE_NEG: // -
				POP_ONE;
				if(stk_result_op1->properties & MSK_STK_PROPERTY_ZS_INT){ // operation will result as integer.
					PUSH_INTEGER((-((zs_int)(stk_result_op1->stk_value))));
				}else if(stk_result_op1->properties & MSK_STK_PROPERTY_FLOAT){
					COPY_FLOAT(&f_aux_value1,&stk_result_op1->stk_value);
					PUSH_FLOAT(-f_aux_value1);
				}else{ // try metamethod ...
					if(!applyMetamethod(
							calling_function
							,instruction
							,BYTE_CODE_METAMETHOD_NEG
							,stk_result_op1
							,stk_result_op2
					)){
						goto lbl_exit_function;
					}
				}
				continue;
			case BYTE_CODE_ADD: // +
				POP_TWO;
				PROCESS_ARITHMETIC_OPERATION(+,BYTE_CODE_METAMETHOD_ADD);
				continue;
			case BYTE_CODE_SUB: // -
				POP_TWO;
				PROCESS_ARITHMETIC_OPERATION(-,BYTE_CODE_METAMETHOD_SUB);
				continue;
			case BYTE_CODE_MUL: // *
				POP_TWO;
				PROCESS_ARITHMETIC_OPERATION(*,BYTE_CODE_METAMETHOD_MUL);
				continue;
			case BYTE_CODE_DIV: // /
				POP_TWO;
				PROCESS_ARITHMETIC_OPERATION(/, BYTE_CODE_METAMETHOD_DIV);
				continue;
			 case BYTE_CODE_MOD: // /
				POP_TWO;
				PROCESS_MOD_OPERATION;
				continue;
			 case BYTE_CODE_AND: // &
				POP_TWO;
				PROCESS_BINARY_OPERATION(&, BYTE_CODE_METAMETHOD_AND);
				continue;
			 case BYTE_CODE_OR: // *
				POP_TWO;
				PROCESS_BINARY_OPERATION(|, BYTE_CODE_METAMETHOD_OR);
				continue;
			 case BYTE_CODE_XOR: // ^
				POP_TWO;
				PROCESS_BINARY_OPERATION(^, BYTE_CODE_METAMETHOD_XOR);
				continue;
			 case BYTE_CODE_SHR: // >>
				POP_TWO;
				PROCESS_BINARY_OPERATION(>>,BYTE_CODE_METAMETHOD_SHR);
				continue;
			 case BYTE_CODE_SHL: // <<
				POP_TWO;
				PROCESS_BINARY_OPERATION(<<, BYTE_CODE_METAMETHOD_SHL);
				continue;
			 case BYTE_CODE_INSTANCEOF: // check instance of ...
				 POP_TWO;

				if(stk_result_op1->properties & MSK_STK_PROPERTY_PTR_STK) {// == ScriptObjectAnonymous::VAR_TYPE::OBJECT){
					stk_result_op1=(StackElement *)stk_result_op1->stk_value; // stk_value is expect to contents a stack variable
				}
				switch((zs_int)stk_result_op2->stk_value){
				case IDX_BUILTIN_TYPE_ZS_INT_PTR_C:
					PUSH_BOOLEAN((stk_result_op1->properties & MSK_STK_PROPERTY_ZS_INT)!=0);
					break;
				case IDX_BUILTIN_TYPE_FLOAT_PTR_C:
					PUSH_BOOLEAN((stk_result_op1->properties & MSK_STK_PROPERTY_FLOAT)!=0);
					break;
				case IDX_BUILTIN_TYPE_BOOL_PTR_C:
					PUSH_BOOLEAN((stk_result_op1->properties & MSK_STK_PROPERTY_BOOL)!=0);
					break;
				case IDX_BUILTIN_TYPE_FUNCTION:
					PUSH_BOOLEAN((stk_result_op1->properties & MSK_STK_PROPERTY_FUNCTION)!=0);
					break;
				default:
					if(stk_result_op1->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
						bool b = this->script_class_factory->isClassInheritsFrom(			//
								((ScriptObjectAnonymous *)(stk_result_op1->stk_value))->idx_script_class // A
								, (zs_int)stk_result_op2->stk_value				// B
						);
						PUSH_BOOLEAN(b);
					}else{
						PUSH_BOOLEAN(false);
					}
					break;
				}
				continue;
			 case BYTE_CODE_JMP:
				instruction_it=instruction+instruction->value_op2;
				continue;
			 case BYTE_CODE_JNT: // goto if not true ... goes end to conditional.
				POP_ONE;
				if(stk_result_op1->stk_value == 0){
					instruction_it=instruction+instruction->value_op2;
				}
				continue;
			 case BYTE_CODE_JT: // goto if true ... goes end to conditional.
				POP_ONE;
				if(stk_result_op1->stk_value != 0){
					instruction_it=instruction+instruction->value_op2;
				}
				continue;
			case BYTE_CODE_JE:  // especial j for switch
				READ_TWO_POP_ONE; // reads switch value and case value
				PROCESS_COMPARE_OPERATION(==, BYTE_CODE_METAMETHOD_EQU);
				POP_ONE; // retrieve result...
				if(stk_result_op1->stk_value != 0){ // if true goto
					instruction_it=instruction+instruction->value_op2;
				}
				continue;

			 case  BYTE_CODE_CALL: // calling function after all of args are processed...

				 {
					ScriptFunction *sf = NULL;
					unsigned char n_args=instruction->value_op1; // number arguments will pass to this function
					StackElement *stk_function_ref=NULL;
					bool calling_from_object_type=false;
					zs_int idx_function=ZS_IDX_UNDEFINED;

					StackElement *stk_start_arg_call=(stk_vm_current-n_args);

					stk_function_ref = ((stk_start_arg_call-1));
					bool is_constructor=instruction->byte_code==BYTE_CODE_CALL_CONSTRUCTOR;

					calling_object = this_object;

					if(stk_function_ref->properties & MSK_STK_PROPERTY_FUNCTION_MEMBER){
						calling_object = ((FunctionMember *)stk_function_ref)->so_object;
					}

					if(stk_function_ref->properties & MSK_STK_PROPERTY_PTR_STK){
						stk_function_ref=(StackElement *)stk_function_ref->stk_value;
					}

					if((stk_function_ref->properties & (MSK_STK_PROPERTY_FUNCTION))==0){
						VM_STOP_EXECUTE("%s is not function or not exist",SFI_GET_SYMBOL_NAME(calling_function,instruction));
					}

					sf=(ScriptFunction *)stk_function_ref->stk_value;

					if(sf == NULL){
						VM_STOP_EXECUTE("internal error ScriptFunction null");
					}

					// if a c function that it has more than 1 symbol with same number of parameters, so we have to solve and get the right one...
					if(sf->symbol.properties & SYMBOL_PROPERTY_DEDUCE_AT_RUNTIME){

						StackElement *stk_element_ptr=vm_stack;
						int stk_element_len = main_function_object->registered_symbols->count;
						bool ignore_call=false;

						if(
							calling_from_object_type //scope_type&(MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_FIELD|MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS)
						){
							ignore_call= (is_constructor) && calling_object->isNativeObject() && n_args==0;
							zs_vector * list_props=calling_object->getAllElements();//getFunctions();
							stk_element_ptr=(StackElement *)list_props->items;
							stk_element_len=list_props->count;
						}

						if(ignore_call == false)
						{
							ScriptFunction *sf_aux;
							if((sf_aux=findFunction(
									 calling_object
									,calling_function
									,instruction
									,is_constructor
									,stk_element_ptr
									,stk_element_len
									,sf->symbol.name // symbol to find
									,stk_start_arg_call
									,n_args))==NULL){

								VM_STOP_EXECUTE("cannot find function \"%s\"",sf->symbol.name.c_str());
							}

							sf=sf_aux;
						}
					}

					// call function
					if((sf->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF) == 0){ // if script function...
						// we pass everything by copy (TODO implement ref)
						if(n_args > 0){
							StackElement *stk_arg=stk_start_arg_call;
							ScriptObjectVector *var_args=NULL;
							zs_int *function_param=&sf->params->items[0];
							int effective_args=n_args < sf->params->count ? n_args:sf->params->count;
							int i=0;
							if(sf->params->count > 0){
								for(;;){

									if(((FunctionParam *)(*function_param))->by_ref == false){ // copy
										unsigned short properties = stk_arg->properties;
										if(properties & MSK_STK_PROPERTY_PTR_STK){
											*stk_arg=*((StackElement *)stk_arg->stk_value);
										}else if(STK_IS_SCRIPT_OBJECT_STRING(stk_arg)){ // remove
											ScriptObjectString *sc=ScriptObjectString::newStringObject(this->zs);
											if(!sc->initSharedPtr()){
												goto lbl_exit_function;
											}
											sc->set((const char *)stk_arg->stk_value);
											stk_arg->stk_value=(void *)sc;
											stk_arg->properties=MSK_STK_PROPERTY_SCRIPT_OBJECT;
										}
									}

									if(var_args!=NULL){
										var_args->push(stk_arg);

										if(i+1 >= (unsigned)n_args){
											break;
										}
									}else{
										if(((FunctionParam *)(*function_param))->var_args == true){ // enter var args
											var_args=ScriptObjectVector::newVectorObject(this->zs);
											if(!var_args->initSharedPtr()){
												goto lbl_exit_function;
											}

											// push first arg
											var_args->push(stk_arg);
											// replace for vector type...
											stk_arg->stk_value=(void *)var_args;
											stk_arg->properties=MSK_STK_PROPERTY_SCRIPT_OBJECT;
											if(i+1 >= (unsigned)n_args){
												break;
											}
										}else{
											function_param++;
											if(i+1 >= (unsigned)effective_args){
												break;
											}
										}
									}
									stk_arg++;
									i++;
								}
							}

						}

						// ... we must set the rest of parameters as undefined in case user put less params. If params exceds the number of accepted params in function,
						// will be ignored always.
						for(unsigned i = n_args; i < sf->params->count; i++){
							PUSH_UNDEFINED;
							n_args++;
						}

						callFunctionScript(
							calling_object
							,sf
							,stk_start_arg_call
							,n_args
						);
					}
					else{ // C function
						if((is_constructor && (sf->symbol.properties & SYMBOL_PROPERTY_SET_FIRST_PARAMETER_AS_THIS))
							){
							calling_object= (ScriptObjectAnonymous *)(stk_start_arg_call-2)->stk_value; // the object should be before (start_arg -1 (idx_function)  - 2 (idx_object))
						}

						if(calling_object != NULL && calling_object->idx_script_class<IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_CLASS){
							VM_STOP_EXECUTE("Internal error, expected object class");
						}

						callFunctionNative(
							 sf
							,stk_start_arg_call
							,n_args
							,instruction
							,(ScriptObjectClass *)calling_object
						);

						// restore stk_start_arg_call due in C args are not considered as local symbols (only for scripts)
						stk_start_arg_call+=n_args;
					}

					if(vm_error == true){
						vm_error_callstack_str+=zs_strutils::format(
								"\nat %s (file:%s line:%i)" // TODO: get full symbol ?
								,sf->symbol.name.c_str()
								,SFI_GET_FILE(calling_function,instruction)
								,SFI_GET_LINE(calling_function,instruction)
						);
						goto lbl_exit_function;
					}

					// reset stack (function+instruction (-1 op less))...
					/*StackElement *src_returned_variables=stk_vm_current;

					// TODO: calcule returned stack elements
					//int min_return=1;//stk_vm_current-sf->params->count;
					int max_return=instruction->value_op2;
					if(max_return <= 0){ // as default returns 1
						max_return=1;
					}*/

					StackElement *stk_return=(stk_start_arg_call+sf->registered_symbols->count );
					int n_returned_arguments_from_function=stk_vm_current-stk_return;

					// setup all returned variables from function
					for(int i=0; i < n_returned_arguments_from_function; i++){

						StackElement *stk_ret = --stk_vm_current;

						// if a scriptvar --> init shared
						if(stk_ret->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
							ScriptObject *sv=(ScriptObject *)stk_ret->stk_value;
							if(sv->shared_pointer == NULL){ // if return this, it holds ptr_shared_pointer
								if(!sv->initSharedPtr()){
									goto lbl_exit_function;
								}
							}
						}
						// ... and push result if not function constructor...
					}

					stk_vm_current=stk_start_arg_call-1; // reset stack

					if(instruction->value_op2 == ZS_IDX_INSTRUCTION_OP2_RETURN_ALL_STACK) {
						StackElement tmp;
						// return all elements in reverse order in order to get right assignment ...

						// reverse returned items
						for(int i=0; i<(n_returned_arguments_from_function>>1); i++){
							tmp=stk_return[n_returned_arguments_from_function-i-1];
							stk_return[n_returned_arguments_from_function-i-1]=stk_return[i];
							stk_return[i]=tmp;
						}

						// copy to vm stack
						stk_vm_current=stk_start_arg_call-1;//+n_returned_arguments_from_function; // stk_vm_current points to first stack element

						for(int i=0;i<n_returned_arguments_from_function;i++){
							//if(n_returned_arguments_from_function > 0){
							*stk_vm_current++= *stk_return++; // only return first argument
							/*}
							else{
								PUSH_UNDEFINED; // no return push undefined
							}*/
						}

						//stk_vm_current+=n_returned_arguments_from_function;

					}else{ // return only first element
						// set undefined for other assignments...
							//for(int i=max_return-1; i >=0; i--){
						if(n_returned_arguments_from_function > 0){
							*stk_vm_current++= stk_return[0]; // only return first argument
						}
						else{
							PUSH_UNDEFINED; // no return push undefined
						}
							//}
					}
				 }
				continue;
			 case  BYTE_CODE_RET:
				for(StackElement *stk_it=stk_vm_current-1;stk_it>=stk_start;stk_it--){ // can return something. value is +1 from stack
					if(stk_it->properties & MSK_STK_PROPERTY_PTR_STK){ // unpack
						*stk_it=*((StackElement *)stk_it->stk_value); // unpack
					}

					// if scriptvariable and in the zeros list, deattach
					if(stk_it->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
						if(!STK_IS_THIS(stk_it)){
							ScriptObjectAnonymous *script_var=(ScriptObjectAnonymous *)stk_it->stk_value;

							// deattach from zero shares if exist...
							if(deattachShareNode(script_var->shared_pointer->data.zero_shares,script_var->shared_pointer)==false){
								goto lbl_exit_function;
							}

							// and free
							free(script_var->shared_pointer);
							script_var->shared_pointer=NULL;
						}
					}
				}
				goto lbl_exit_function;
			 case  BYTE_CODE_NEW_CLASS:
				 	 script_object_class=NEW_CLASS_VAR_BY_IDX(this,instruction->value_op1);

					if(!script_object_class->initSharedPtr()){
						goto lbl_exit_function;
					}
					script_object_class->info_function_new=calling_function;
					script_object_class->instruction_new=instruction;
					(*stk_vm_current++)={script_object_class,MSK_STK_PROPERTY_SCRIPT_OBJECT};
					continue;
			 case BYTE_CODE_NEW_VECTOR: // Create new std::vector object...
					script_var=NEW_VECTOR_VAR;
					if(!script_var->initSharedPtr()){
						goto lbl_exit_function;
					}
					(*stk_vm_current++)={script_var,MSK_STK_PROPERTY_SCRIPT_OBJECT};
					continue;
			 case  BYTE_CODE_NEW_ANONYMOUS: // Create new std::vector object...
					script_var=ScriptObjectAnonymous::newAnonymousObject(this->zs);
					if(!script_var->initSharedPtr()){
						goto lbl_exit_function;
					}
					(*stk_vm_current++)={script_var,MSK_STK_PROPERTY_SCRIPT_OBJECT};
					continue;
			 case  BYTE_CODE_DELETE:
					POP_ONE;
					//script_var
					if(stk_result_op1->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
						StackElement *se=stk_result_op1;
						if(stk_result_op1->properties & MSK_STK_PROPERTY_PTR_STK){
							se=(StackElement *)(stk_result_op1->stk_value);
						}

						script_var = (ScriptObject *)(se)->stk_value;

						if(!script_var->unrefSharedPtr(vm_idx_call)){
							goto lbl_exit_function;
						}

						if(script_var->idx_script_class==IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_CLASS)
						{ // max ...
							script_object_class=(ScriptObjectClass *)script_var;

							if(script_object_class->isCreatedByContructor()){
								script_object_class->deleteNativeObjectOnDestroy(true);
							}
							*se=stk_undefined;
						}
					}
					else{
						VM_STOP_EXECUTE("Error deleting \"%s\". cannot perform delete on variables type \"%s\"",SFI_GET_SYMBOL_NAME(calling_function,instruction-1),stk_result_op1->typeStr());
					}
					continue;
			 case BYTE_CODE_PUSH_SCOPE:
				PUSH_VM_SCOPE(instruction->value_op2,calling_function,_stk_local_var,instruction->value_op1);
				continue;

			 case BYTE_CODE_POP_SCOPE:
				popVmScope();
				continue;
			 case BYTE_CODE_IT_NEXT:
				 VM_STOP_EXECUTE("BYTE_CODE_SET_AND_NEXT TODOOOOO!",
					 SFI_GET_SYMBOL_NAME(calling_function,instruction)
				);
				 //*((StackElement *)vm_foreach_current->key)=((ScriptObjectVector *)vm_foreach_current->ptr)->m_objVector[vm_foreach_current->idx_current++];
				 continue;
			 case BYTE_CODE_IT_END:
				 VM_STOP_EXECUTE("BYTE_CODE_IT_CHK_END TODOOOOO!",
				 SFI_GET_SYMBOL_NAME(calling_function,instruction)
				);
				 continue;
			 case BYTE_CODE_RESET_STACK:
				 stk_vm_current=stk_start;
				 continue;
			 case BYTE_CODE_POST_INC:
				 PERFORM_POST_OPERATOR(+,++);
				 continue;
			 case BYTE_CODE_POST_DEC:
				 PERFORM_POST_OPERATOR(+,--);
				 continue;
			 case BYTE_CODE_PRE_INC:
				 PERFORM_PRE_OPERATOR(++);
				 continue;
			 case BYTE_CODE_PRE_DEC:
				 PERFORM_PRE_OPERATOR(--);
				 continue;
			 case BYTE_CODE_NEG_POST_INC:
				 PERFORM_POST_OPERATOR(-,++);
				 continue;
			 case BYTE_CODE_NEG_POST_DEC:
				 PERFORM_POST_OPERATOR(-,--);
				 continue;

			}
		 }

	lbl_exit_function:

		//=========================
		// POP STACK
		if(calling_function->idx_script_function != IDX_SCRIPT_FUNCTION_MAIN){ // if main function only remove empty shared pointers but preserve global variables!)
			// pop all scopes
			while(vm_scope_start<(vm_current_scope)){
				popVmScope(false); // do not check removeEmptySharedPointers to have better performance
			}

			removeEmptySharedPointers(vm_idx_call);
		}

		vm_idx_call--;

		// POP STACK
		//=========================
	}
}
