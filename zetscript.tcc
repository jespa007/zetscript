namespace zetscript{

		// Helpers...
		 StackElement ZetScript::convertVarToStackElement(intptr_t ptr_var, int idx_builtin_type_var){
			//intptr_t ptr_var = (intptr_t)input_var;
				std::string s_return_value;
				StackElement stk_result={0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
				//int idx_builtin_type=getIdxClassFromItsNativeType(typeid(T).name());
				// save return type ...
				switch(idx_builtin_type_var){
				 case IDX_BUILTIN_TYPE_VOID_C:
					break;
				 case IDX_BUILTIN_TYPE_INT_PTR_C:
					 if(ptr_var==0) return stk_result;
					 stk_result={(void *)(*((intptr_t *)ptr_var)),0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER};
					 break;
				 case IDX_BUILTIN_TYPE_UNSIGNED_INT_C:
				 case IDX_BUILTIN_TYPE_INTPTR_T_C:
				 case IDX_BUILTIN_TYPE_INT_C:
					 stk_result={(void *)(((intptr_t)ptr_var)),0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER};
					 break;
				 case IDX_BUILTIN_TYPE_FLOAT_C:
					 stk_result.properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT;//{};
					 memcpy(&stk_result.stk_value,&ptr_var,sizeof(float));

					 break;
				 case IDX_BUILTIN_TYPE_FLOAT_PTR_C:
					 if(ptr_var==0) return stk_result;
					 stk_result.properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT;//{};
					 memcpy(&stk_result.stk_value,&(*(float *)ptr_var),sizeof(float));
					 break;
				 case IDX_BUILTIN_TYPE_BOOL_PTR_C:
					 if(ptr_var==0) return stk_result;
					 stk_result={(void *)(*((bool *)ptr_var)),0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN};
					 break;
				 case IDX_BUILTIN_TYPE_BOOL_C:
					 stk_result={(void *)(((bool)ptr_var)),0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN};
					 break;
				 case IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C:
					 if(ptr_var==0) return stk_result;
					 stk_result={(void *)ptr_var,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING};//new std::string(*((std::string *)result))};
					 break;
				 case IDX_BUILTIN_TYPE_STRING_PTR_C:
					 if(ptr_var==0) return stk_result;
					 stk_result={(void *)((std::string *)ptr_var)->c_str(),0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING};//new std::string(*((std::string *)result))};
					 break;
				 case IDX_BUILTIN_TYPE_STACK_ELEMENT:
					 if(ptr_var==0) return stk_result;
					 stk_result=*((StackElement *)ptr_var);//{MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING,(void *)((std::string *)ptr_var)->c_str(),NULL};//new std::string(*((std::string *)result))};
					 break;
				 default:
					 if(ptr_var==0) return stk_result;
					 stk_result = {NULL,script_class_factory->instanceScriptVariableByIdx(idx_builtin_type_var,(void *)ptr_var),MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR};
					 break;
				}

				return stk_result;
		}

		bool ZetScript::convertStackElementToVar(StackElement *stack_element, int idx_builtin_type, intptr_t *ptr_var, std::string & error){
			intptr_t val_ret=0;

			ScriptVar *script_variable=NULL;

			// save return type ...
			if(stack_element->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT){
				stack_element=((StackElement *)stack_element->var_ref);
			}

			if(idx_builtin_type == IDX_BUILTIN_TYPE_STACK_ELEMENT){//*ScriptClass::STACK_ELEMENT_PTR)){// && (stack_element->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_STACK_ELEMENT)){ // set directly stackvar
				val_ret=(intptr_t)stack_element;
			}else{

				switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stack_element->properties)){
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
					if(idx_builtin_type == IDX_BUILTIN_TYPE_BOOL_C){// *ScriptClass::k_str_bool_type){
						val_ret=(intptr_t)(stack_element->stk_value);
					}else if(idx_builtin_type == IDX_BUILTIN_TYPE_BOOL_PTR_C){//*ScriptClass::k_str_bool_type_ptr){
						val_ret=(intptr_t)(&stack_element->stk_value);
					}else{
						error="cannot convert "+zs_rtti::demangle((k_str_string_type_ptr))+" into %s"+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,idx_builtin_type)).c_str();

						return false;
					}

					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
					switch(idx_builtin_type){
					case IDX_BUILTIN_TYPE_FLOAT_C:
						memcpy(&val_ret,&stack_element->stk_value,sizeof(float));
						break;
					case IDX_BUILTIN_TYPE_FLOAT_PTR_C:
						val_ret=(intptr_t)(&stack_element->stk_value);
						break;
					case IDX_BUILTIN_TYPE_UNSIGNED_INT_C:
					case IDX_BUILTIN_TYPE_INTPTR_T_C:
					case IDX_BUILTIN_TYPE_INT_C:
						{
							int *aux_dst = ((int *)&val_ret);
							float *aux_src=(float *)&stack_element->stk_value;
							*aux_dst=(int)(*aux_src);
						}
						break;
					default:
						error="cannot convert "+zs_rtti::demangle((k_str_string_type_ptr))+" into %s"+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,idx_builtin_type));
						return false;
					}
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
					switch(idx_builtin_type){
					case IDX_BUILTIN_TYPE_UNSIGNED_INT_C:
					case IDX_BUILTIN_TYPE_INTPTR_T_C:
					case IDX_BUILTIN_TYPE_INT_C:
						val_ret=(intptr_t)(stack_element->stk_value);
						break;
					case IDX_BUILTIN_TYPE_INT_PTR_C:
						val_ret=(intptr_t)(&stack_element->stk_value);
						break;
					case IDX_BUILTIN_TYPE_FLOAT_C:
						{
							float *aux_dst = ((float *)&val_ret);
							int *aux_src=(int *)&stack_element->stk_value;
							*aux_dst = (float)(*aux_src);
						}
						break;
					default:
						error= "cannot convert "+zs_rtti::demangle((k_str_string_type_ptr))+" into "+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,idx_builtin_type));
						return false;
					}
					break;

				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
					if(idx_builtin_type == IDX_BUILTIN_TYPE_STRING_PTR_C){
						if(stack_element->var_ref != 0){
							val_ret=(intptr_t)(&((ScriptVarString *)(stack_element->var_ref))->str_value);
						}
						else{ // pass param std::string ...
							error= "(std::string *)Expected var_ref not NULL";
							return false;
						}

					}else if (idx_builtin_type == IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C){
						val_ret=(intptr_t)(stack_element->stk_value);
					}else{
						error= "cannot convert "+zs_rtti::demangle((k_str_string_type_ptr))+" into "+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,idx_builtin_type));
						return false;
					}


					break;
				default: // script variable by default ...

					script_variable=(ScriptVar *)stack_element->var_ref;
					ScriptClass *c_class=NULL;

					if(script_variable==NULL){

						error="Variable is not defined";
						return false;
					}

					if(script_variable->idx_class==IDX_BUILTIN_TYPE_CLASS_STRING){

						val_ret=(intptr_t)(&script_variable->str_value);
						if(idx_builtin_type == IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C){
							val_ret=(intptr_t)script_variable->str_value.c_str();
						}
					}else if(

					   (script_variable->idx_class==IDX_BUILTIN_TYPE_CLASS_VECTOR
					|| script_variable->idx_class==IDX_BUILTIN_TYPE_CLASS_DICTIONARY)){

						if(idx_builtin_type==script_variable->idx_class){
							val_ret=(intptr_t)script_variable->getNativeObject();
						}

					}else if((c_class=script_variable->getNativeClass())!=NULL){ // get the pointer directly ...

						if(c_class->idx_class==idx_builtin_type){
							val_ret=(intptr_t)script_variable->getNativeObject();
						}
						else if((val_ret=script_class_factory->doCast((intptr_t)script_variable->getNativeObject(),c_class->idx_class,idx_builtin_type))==0){//c_class->idx_class==idx_builtin_type){
							error = "cannot convert "+zs_rtti::demangle(script_variable->getNativePointerClassName())+" into "+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,idx_builtin_type));
							return false;
						}
					}else{ // ScriptVar ?
						error = " Error calling function, no C-object parameter! Unexpected script variable ("+zs_rtti::demangle(script_variable->getClassName())+")";
						return false;
					}
					break;
				}
			}

			*ptr_var = val_ret;

			return true;
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 0 PARAMS
		//
		template <typename R,typename T>
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<std::is_same<R,void>::value>::type
		{


			*f=((void *)(new std::function<void ()>(
				[&,calling_obj,fun_obj](){
					bool error=false;
					virtual_machine->execute(
										fun_obj,
										calling_obj);

				}
			)));
		}

		template <typename R,typename T>
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<!std::is_same<R,void>::value>::type
		{
			int idx_return = script_class_factory->getIdxClassFromItsNativeType(typeid(R).name());

			*f=((void *)(new std::function<R ()>(
				[&,calling_obj,fun_obj,idx_return](){
						std::string error_str;
						R ret_value;

						StackElement stk = virtual_machine->execute(
								fun_obj,
								calling_obj);

						if(!convertStackElementToVar(&stk, idx_return, (intptr_t *)(&ret_value),error_str)){
							THROW_RUNTIME_ERROR("run-time error converting result value:%s",error_str.c_str());
						}
						return ret_value;
				}
			)));

		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 1 PARAMS
		//
		// template for last parameter argIdx == 1
		template<typename R,typename T,  typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(void **f ,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type
		{
			//return NULL;

			using tParam1 = typename T::template Argument<0>::type;
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam1).name());

			*f=((void *)(new std::function<void (tParam1)>(
				[&,calling_obj,fun_obj, idx_param1](tParam1 p1){

					StackElement args[1]={
							 convertVarToStackElement((intptr_t)p1,idx_param1)
					};

					virtual_machine->execute(
								fun_obj,
								calling_obj,
								args,
								1);
				}
			)));
		}

		template<typename R,typename T,  typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(void **f ,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type
		{
			using tParam1 = typename T::template argument<0>::type;

			int idx_return = script_class_factory->getIdxClassFromItsNativeType(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam1).name());


			*f=((void *)(new std::function<R (tParam1)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1](tParam1 p1){

						R ret_value;
						std::string error_str;

						StackElement args[1]={
								convertVarToStackElement((intptr_t)p1,idx_param1)
						};

						StackElement stk = virtual_machine->execute(
													fun_obj,
													calling_obj,
													args,
													1);

						if(!convertStackElementToVar(&stk,idx_return, (intptr_t*)(&ret_value),error_str)){
							THROW_RUNTIME_ERROR("run-time error converting result value:%s",error_str.c_str());
						}
						return ret_value;
				}
			)));
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 2 PARAMS
		//
		// template when parameters argIdx == 2
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;


			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam2).name());

			*f=((void *)(new std::function<void (tParam1,tParam2)>(
				[&,calling_obj,fun_obj, idx_param1, idx_param2](tParam1 p1,tParam2 p2){

					StackElement args[2]={
							 convertVarToStackElement((intptr_t)p1,idx_param1)
							,convertVarToStackElement((intptr_t)p2,idx_param2)

					};

					virtual_machine->execute(
									fun_obj,
									calling_obj,
									args,
									2);

				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type
		{
			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;


			int idx_return = script_class_factory->getIdxClassFromItsNativeType(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam2).name());

			*f=((void *)(new std::function<R (tParam1,tParam2)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2](tParam1 p1,tParam2 p2){

						R ret_value;
						std::string error_str;

						StackElement args[2]={
								 convertVarToStackElement((intptr_t)p1,idx_param1)
								,convertVarToStackElement((intptr_t)p2,idx_param2)

						};

						StackElement stk = virtual_machine->execute(
													fun_obj,
													calling_obj,
													args,
													2);

						if(!convertStackElementToVar(&stk, idx_return, (intptr_t*)(&ret_value),error_str)){
							THROW_RUNTIME_ERROR("run-time error converting result value:%s",error_str.c_str());
						}
						return ret_value;
				}
			)));

			//return NULL;
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 3 PARAMS
		//
		// template when parameters argIdx == 3
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;


			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam3).name());


			*f=((void *)(new std::function<void (tParam1,tParam2,tParam3)>(
				[&,calling_obj,fun_obj,idx_param1, idx_param2, idx_param3](tParam1 p1,tParam2 p2,tParam3 p3){


					StackElement args[3]={
							 convertVarToStackElement((intptr_t)p1,idx_param1)
							,convertVarToStackElement((intptr_t)p2,idx_param2)
							,convertVarToStackElement((intptr_t)p3,idx_param3)
					};

					virtual_machine->execute(
									fun_obj,
									calling_obj,
									args,
									3);

				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type
		{
			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;

			int idx_return = script_class_factory->getIdxClassFromItsNativeType(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam3).name());

			*f=((void *)(new std::function<R (tParam1,tParam2,tParam3)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3](tParam1 p1,tParam2 p2,tParam3 p3){
					R ret_value;
					std::string error_str;

					StackElement args[3]={
							 convertVarToStackElement((intptr_t)p1,idx_param1)
							,convertVarToStackElement((intptr_t)p2,idx_param2)
							,convertVarToStackElement((intptr_t)p3,idx_param3)
					};

					StackElement stk = virtual_machine->execute(
												fun_obj,
												calling_obj,
												args,
												3);

					if(!convertStackElementToVar(&stk, idx_return, (intptr_t *)(&ret_value),error_str)){
						THROW_RUNTIME_ERROR("run-time error converting result value:%s",error_str.c_str());
					}
					return ret_value;
				}
			)));
		}


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 4 PARAMS
		//
		// template when parameters argIdx == 4
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;

			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam3).name());
			int idx_param4 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam4).name());


			*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4)>(
				[&,calling_obj,fun_obj, idx_param1, idx_param2, idx_param3, idx_param4](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4){

					StackElement args[4]={
							 convertVarToStackElement((intptr_t)p1,idx_param1)
							,convertVarToStackElement((intptr_t)p2,idx_param2)
							,convertVarToStackElement((intptr_t)p3,idx_param3)
							,convertVarToStackElement((intptr_t)p4,idx_param4)
					};

					virtual_machine->execute(
									fun_obj,
									calling_obj,
									args,
									4);

				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;

			int idx_return = script_class_factory->getIdxClassFromItsNativeType(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam3).name());
			int idx_param4 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam4).name());

			*f=((void *)(new std::function<R (tParam1,tParam2,tParam3,tParam4)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4){
						R ret_value;
						std::string error_str;

						StackElement args[4]={
								 convertVarToStackElement((intptr_t)p1,idx_param1)
								,convertVarToStackElement((intptr_t)p2,idx_param2)
								,convertVarToStackElement((intptr_t)p3,idx_param3)
								,convertVarToStackElement((intptr_t)p4,idx_param4)
						};

						StackElement stk = virtual_machine->execute(
													fun_obj,
													calling_obj,
													args,
													4);

						if(!convertStackElementToVar(&stk, idx_return, (intptr_t*)(&ret_value),error_str)){
							THROW_RUNTIME_ERROR("run-time error converting result value:%s",error_str.c_str());
						}
						return ret_value;
				}
			)));

		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 5 PARAMS
		//
		// template when parameters argIdx == 5
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;
			using tParam5 = typename T::template argument<4>::type;



			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam3).name());
			int idx_param4 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam4).name());
			int idx_param5 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam5).name());


			*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4,tParam5)>(
				[&,calling_obj,fun_obj,idx_param1, idx_param2, idx_param3, idx_param4, idx_param5](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5){

					StackElement args[5]={
							 convertVarToStackElement((intptr_t)p1,idx_param1)
							,convertVarToStackElement((intptr_t)p2,idx_param2)
							,convertVarToStackElement((intptr_t)p3,idx_param3)
							,convertVarToStackElement((intptr_t)p4,idx_param4)
							,convertVarToStackElement((intptr_t)p5,idx_param5)

					};

					virtual_machine->execute(
									fun_obj,
									calling_obj,
									args,
									5);
				}

			)));
		}


		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;
			using tParam5 = typename T::template argument<4>::type;

			int idx_return = script_class_factory->getIdxClassFromItsNativeType(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam3).name());
			int idx_param4 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam4).name());
			int idx_param5 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam5).name());

			*f=((void *)(new std::function<R (tParam1,tParam2,tParam3,tParam4,tParam5)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5){

					R ret_value;
					std::string error_str;

					StackElement args[5]={
							 convertVarToStackElement((intptr_t)p1,idx_param1)
							,convertVarToStackElement((intptr_t)p2,idx_param2)
							,convertVarToStackElement((intptr_t)p3,idx_param3)
							,convertVarToStackElement((intptr_t)p4,idx_param4)
							,convertVarToStackElement((intptr_t)p5,idx_param5)

					};

					StackElement stk = virtual_machine->execute(
												fun_obj,
												calling_obj,
												args,
												5);

					if(!convertStackElementToVar(&stk, idx_return, (intptr_t*)(&ret_value),error_str)){
						THROW_RUNTIME_ERROR("run-time error converting result value:%s",error_str.c_str());
					}
					return ret_value;
				}
			)));
		}


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 6 PARAMS
		//
		// template when parameters argIdx == 6
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 6)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;
			using tParam5 = typename T::template argument<4>::type;
			using tParam6 = typename T::template argument<5>::type;


			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam3).name());
			int idx_param4 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam4).name());
			int idx_param5 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam5).name());
			int idx_param6 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam6).name());




			*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4,tParam5,tParam6)>(
				[&,calling_obj,fun_obj, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5, idx_param6](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5,tParam6 p6){

					StackElement args[6]={
							 convertVarToStackElement((intptr_t)p1,idx_param1)
							,convertVarToStackElement((intptr_t)p2,idx_param2)
							,convertVarToStackElement((intptr_t)p3,idx_param3)
							,convertVarToStackElement((intptr_t)p4,idx_param4)
							,convertVarToStackElement((intptr_t)p5,idx_param5)
							,convertVarToStackElement((intptr_t)p6,idx_param6)
					};

					virtual_machine->execute(
									fun_obj,
									calling_obj,
									args,
									6);
				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 6)>::type
		{
			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;
			using tParam5 = typename T::template argument<4>::type;
			using tParam6 = typename T::template argument<5>::type;

			int idx_return = script_class_factory->getIdxClassFromItsNativeType(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam3).name());
			int idx_param4 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam4).name());
			int idx_param5 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam5).name());
			int idx_param6 = script_class_factory->getIdxClassFromItsNativeType(typeid(tParam6).name());

			*f=((void *)(new std::function<R (tParam1,tParam2,tParam3,tParam4,tParam5,tParam6)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5, idx_param6](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5,tParam6 p6){

						R ret_value;
						std::string error_str;

						StackElement args[6]={
								 convertVarToStackElement((intptr_t)p1,idx_param1)
								,convertVarToStackElement((intptr_t)p2,idx_param2)
								,convertVarToStackElement((intptr_t)p3,idx_param3)
								,convertVarToStackElement((intptr_t)p4,idx_param4)
								,convertVarToStackElement((intptr_t)p5,idx_param5)
								,convertVarToStackElement((intptr_t)p6,idx_param6)
						};

						StackElement stk = virtual_machine->execute(
													fun_obj,
													calling_obj,
													args,
													6);

						if(!convertStackElementToVar(&stk, idx_return, (intptr_t *)(&ret_value),error_str)){
							THROW_RUNTIME_ERROR("run-time error converting result value:%s",error_str.c_str());
						}
						return ret_value;

				}
			)));
		}


		//
		//
		//--------------------------------------------------------------------------------------------------------------------

		 template <typename F, std::size_t... Is>
		 auto ZetScript::bindScriptFunctionBuilderBase(void **f, ScriptVar *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity > 0)>::type
		{
			 bindScriptFunctionBuilder<typename F::return_type, F,  typename F::template Argument<Is>::type...>(f,calling_obj,fun_obj);
		}

		 template <typename F, std::size_t... Is>
		 auto ZetScript::bindScriptFunctionBuilderBase(void **f, ScriptVar *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity == 0)>::type
		{
			 bindScriptFunctionBuilder<typename F::return_type, F>(f,calling_obj,fun_obj);
		}


		template <  typename F>
		std::function<F> * ZetScript::bindScriptFunction(const std::string & function_access)
		{
			std::string return_type;
			std::vector<std::string> params;
			std::vector<std::string> arg;
			int idx_return_type=-1;
			void *ptr;
			ScriptFunction * fun=NULL;
			ScriptVar *calling_obj=NULL;

			// get function symbol ref from global scope ...


			if(getScriptObject(function_access,&calling_obj,&fun))
			{

				// 1. check all parameters ok.
				using Traits3 = FunctionTraits<F>;//decltype(function_type)>;
				getParamsFunction<Traits3>(0,return_type, arg, MakeIndexSequence<Traits3::arity>{});

				// 2. check valid parameters ...
				if((idx_return_type=script_class_factory->getIdxClassFromItsNativeType(return_type)) == -1){
					THROW_RUNTIME_ERROR("Return type \"%s\" for bind function not registered",zs_rtti::demangle(return_type).c_str());
					return NULL;
				}

				for(unsigned int i = 0; i < arg.size(); i++){
					if(script_class_factory->getIdxClassFromItsNativeType(arg[i])==-1){
						THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for bind function not registered",i,zs_rtti::demangle(arg[i]).c_str());
						return NULL;
					}
				}

				// 3. build custom function in function of parameter number ...
				bindScriptFunctionBuilderBase<Traits3>(&ptr,calling_obj,fun,MakeIndexSequence<Traits3::arity>{});
				return (std::function<F> *)ptr;

			}

			return NULL;
		}
}
