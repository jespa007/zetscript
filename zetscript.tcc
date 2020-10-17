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
					 stk_result = {NULL,script_class_factory->instanceScriptObjectiableByIdx(idx_builtin_type_var,(void *)ptr_var),MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_OBJECT};
					 break;
				}

				return stk_result;
		}

		bool ZetScript::convertStackElementToVar(StackElement *stack_element, int idx_builtin_type, intptr_t *ptr_var, std::string & error){
			intptr_t val_ret=0;

			ScriptObject *script_variable=NULL;

			// save return type ...
			if(stack_element->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK){
				stack_element=((StackElement *)stack_element->var_ref);
			}

			if(idx_builtin_type == IDX_BUILTIN_TYPE_STACK_ELEMENT){//*ScriptClass::STACK_ELEMENT_PTR)){// && (stack_element->properties & MSK_STACK_ELEMENT_PROPERTY_PTR_STK)){ // set directly stackvar
				val_ret=(intptr_t)stack_element;
			}else{

				switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(stack_element->properties)){
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
					if(idx_builtin_type == IDX_BUILTIN_TYPE_BOOL_C){// *ScriptClass::k_str_bool_type){
						val_ret=(intptr_t)(stack_element->stk_value);
					}else if(idx_builtin_type == IDX_BUILTIN_TYPE_BOOL_PTR_C){//*ScriptClass::k_str_bool_type_ptr){
						val_ret=(intptr_t)(&stack_element->stk_value);
					}else{
						error="cannot convert \""+zs_rtti::demangle((k_str_bool_type_ptr))+"\" to \""+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,idx_builtin_type))+"\"";

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
						error="cannot convert \""+zs_rtti::demangle((k_str_float_type_ptr))+"\" to \""+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,idx_builtin_type))+"\"";
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
					case IDX_BUILTIN_TYPE_FLOAT_PTR_C:
						{
							float aux_tr=((intptr_t)stack_element->stk_value);
							//float *aux_dst = ((float *)&val_ret);
							memcpy((float *)&val_ret,&aux_tr,sizeof(float));
							//int *aux_src=(int *)&stack_element->stk_value;
							//*aux_dst = (float)(*aux_src);
						}
						break;

					default:
						error= "cannot convert \"int\" to \""+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,idx_builtin_type))+"\"";
						return false;
					}
					break;

				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
					if(idx_builtin_type == IDX_BUILTIN_TYPE_STRING_PTR_C){
						ScriptObjectString *str_var_ref=(ScriptObjectString *)stack_element->var_ref;
						if(stack_element->var_ref == 0){ // if not created try to create a tmp scriptvar it will be removed...
							if(stack_element->stk_value != NULL){
								str_var_ref= new ScriptObjectString(this);
								str_var_ref->str_value=(const char *)stack_element->stk_value;
								str_var_ref->initSharedPtr(); // init but not assigned so it can be removed...
							}else
							{ // pass param std::string ...
								error= "(std::string *) stk_value and var_ref are NULL";
								return false;
							}
						}

						val_ret=(intptr_t)(&str_var_ref->str_value);

					}else if (idx_builtin_type == IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C){
						val_ret=(intptr_t)(stack_element->stk_value);
					}else{
						error= "cannot convert \""+zs_rtti::demangle((k_str_string_type_ptr))+"\" to \""+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,idx_builtin_type))+"\"";
						return false;
					}
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION:
					val_ret=(intptr_t)stack_element->var_ref;
					break;
				default: // script variable by default ...

					script_variable=(ScriptObject *)stack_element->var_ref;
					ScriptClass *c_class=NULL;

					if(script_variable==NULL){
						error="Variable is not defined";
						return false;
					}

					if(script_variable->idx_class==IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_STRING){
						val_ret=(intptr_t)(&((ScriptObjectString *)script_variable)->str_value);
						if(idx_builtin_type == IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C){
							val_ret=(intptr_t)((ScriptObjectString *)script_variable)->str_value.c_str();
						}
					}else if(

					   (script_variable->idx_class==IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_VECTOR
					|| script_variable->idx_class==IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT)){

						if(idx_builtin_type==script_variable->idx_class){
							val_ret=(intptr_t)script_variable->getNativeObject();
						}

					}else if((c_class=script_variable->getNativeClass())!=NULL){ // get the pointer directly ...

						if(c_class->idx_class==idx_builtin_type){
							val_ret=(intptr_t)script_variable->getNativeObject();
						}
						else if((val_ret=script_class_factory->doCast((intptr_t)script_variable->getNativeObject(),c_class->idx_class,idx_builtin_type))==0){//c_class->idx_class==idx_builtin_type){
							error = "cannot convert \""+zs_rtti::demangle(script_variable->getNativePointerClassName())+"\" to \""+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,idx_builtin_type))+"\"";
							return false;
						}
					}else{ // ScriptObject ?
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
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptObject *calling_obj,ScriptFunction *fun_obj)
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
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptObject *calling_obj,ScriptFunction *fun_obj)
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
		auto ZetScript::bindScriptFunctionBuilder(void **f ,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type
		{
			//return NULL;

			using Param1 = typename T::template Argument<0>::type;
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());

			*f=((void *)(new std::function<void (Param1)>(
				[&,calling_obj,fun_obj, idx_param1](Param1 p1){

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
		auto ZetScript::bindScriptFunctionBuilder(void **f ,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type
		{
			using Param1 = typename T::template Argument<0>::type;

			int idx_return = script_class_factory->getIdxClassFromItsNativeType(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());


			*f=((void *)(new std::function<R (Param1)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1](Param1 p1){

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
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;


			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param2).name());

			*f=((void *)(new std::function<void (Param1,Param2)>(
				[&,calling_obj,fun_obj, idx_param1, idx_param2](Param1 p1,Param2 p2){

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
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type
		{
			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;


			int idx_return = script_class_factory->getIdxClassFromItsNativeType(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param2).name());

			*f=((void *)(new std::function<R (Param1,Param2)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2](Param1 p1,Param2 p2){

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
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;


			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param3).name());


			*f=((void *)(new std::function<void (Param1,Param2,Param3)>(
				[&,calling_obj,fun_obj,idx_param1, idx_param2, idx_param3](Param1 p1,Param2 p2,Param3 p3){


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
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type
		{
			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;

			int idx_return = script_class_factory->getIdxClassFromItsNativeType(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param3).name());

			*f=((void *)(new std::function<R (Param1,Param2,Param3)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3](Param1 p1,Param2 p2,Param3 p3){
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
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;

			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param3).name());
			int idx_param4 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param4).name());


			*f=((void *)(new std::function<void (Param1,Param2,Param3,Param4)>(
				[&,calling_obj,fun_obj, idx_param1, idx_param2, idx_param3, idx_param4](Param1 p1,Param2 p2,Param3 p3,Param4 p4){

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
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;

			int idx_return = script_class_factory->getIdxClassFromItsNativeType(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param3).name());
			int idx_param4 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param4).name());

			*f=((void *)(new std::function<R (Param1,Param2,Param3,Param4)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4](Param1 p1,Param2 p2,Param3 p3,Param4 p4){
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
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;



			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param3).name());
			int idx_param4 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param4).name());
			int idx_param5 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param5).name());


			*f=((void *)(new std::function<void (Param1,Param2,Param3,Param4,Param5)>(
				[&,calling_obj,fun_obj,idx_param1, idx_param2, idx_param3, idx_param4, idx_param5](Param1 p1,Param2 p2,Param3 p3,Param4 p4,Param5 p5){

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
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;

			int idx_return = script_class_factory->getIdxClassFromItsNativeType(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param3).name());
			int idx_param4 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param4).name());
			int idx_param5 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param5).name());

			*f=((void *)(new std::function<R (Param1,Param2,Param3,Param4,Param5)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5](Param1 p1,Param2 p2,Param3 p3,Param4 p4,Param5 p5){

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
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 6)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;
			using Param6 = typename T::template Argument<5>::type;


			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param3).name());
			int idx_param4 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param4).name());
			int idx_param5 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param5).name());
			int idx_param6 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param6).name());




			*f=((void *)(new std::function<void (Param1,Param2,Param3,Param4,Param5,Param6)>(
				[&,calling_obj,fun_obj, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5, idx_param6](Param1 p1,Param2 p2,Param3 p3,Param4 p4,Param5 p5,Param6 p6){

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
		auto ZetScript::bindScriptFunctionBuilder(void **f,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 6)>::type
		{
			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;
			using Param6 = typename T::template Argument<5>::type;

			int idx_return = script_class_factory->getIdxClassFromItsNativeType(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param3).name());
			int idx_param4 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param4).name());
			int idx_param5 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param5).name());
			int idx_param6 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param6).name());

			*f=((void *)(new std::function<R (Param1,Param2,Param3,Param4,Param5,Param6)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5, idx_param6](Param1 p1,Param2 p2,Param3 p3,Param4 p4,Param5 p5,Param6 p6){

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
		 auto ZetScript::bindScriptFunctionBuilderBase(void **f, ScriptObject *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity > 0)>::type
		{
			 bindScriptFunctionBuilder<typename F::return_type, F,  typename F::template Argument<Is>::type...>(f,calling_obj,fun_obj);
		}



		 template <typename F, std::size_t... Is>
		 auto ZetScript::bindScriptFunctionBuilderBase(void **f, ScriptObject *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity == 0)>::type
		{
			 bindScriptFunctionBuilder<typename F::return_type, F>(f,calling_obj,fun_obj);
		}

		template <  typename F>
		std::function<F> * ZetScript::bindScriptFunction(ScriptFunction *fun,ScriptObject *calling_obj){

			std::string return_type;
			std::vector<std::string> params;
			std::vector<std::string> arg;
			int idx_return_type=-1;
			void *ptr=NULL;


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


		template <  typename F>
		std::function<F> * ZetScript::bindScriptFunction(const std::string & function_access)
		{
			ScriptFunction * fun_obj=NULL;
			ScriptObject *calling_obj=NULL;
			std::vector<std::string> access_var = zs_strutils::split(function_access,'.');
			ScriptFunction * main_function = script_class_factory->getMainFunction();
			StackElement *se=NULL;
			StackElement *is;


			// 1. some variable in main function ...
			if(access_var.size()>1){
				for(unsigned i=0; i < access_var.size()-1; i++){
					std::string symbol_to_find=access_var[i];
					if(i==0){ // get variable through main_class.main_function (global element)
						for(unsigned j = 0; j < main_function->registered_symbols->count && calling_obj==NULL; j++){
							Symbol * registered_symbol=(Symbol *)main_function->registered_symbols->items[j];
							if(registered_symbol->name==symbol_to_find
							&& registered_symbol->scope == MAIN_SCOPE(this)){
								StackElement *stk = virtual_machine->getStackElement(j); // main_function->object_info.local_symbols.variable[j].
								if(stk!=NULL){
									if(stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_OBJECT){
										calling_obj=(ScriptObject *)stk->var_ref;
									}
								}
								else{
									THROW_RUNTIME_ERROR("cannot access i (%i)",j);
								}
							}
						}

						if(calling_obj == NULL){
							THROW_RUNTIME_ERROR("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
						}

					}else{ // we have got the calling_obj from last iteration ...
						se = calling_obj->getProperty(symbol_to_find);
						if(se!=NULL){
							if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_OBJECT){
								calling_obj=(ScriptObject *)se->var_ref;
							}else{
								THROW_RUNTIME_ERROR("error evaluating \"%s\". Variable name \"%s\" not script variable",function_access.c_str(),symbol_to_find.c_str());
							}
						}
						else{
							THROW_RUNTIME_ERROR("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
						}
					}
				}

				is=calling_obj->getProperty(access_var[access_var.size()-1]);
				if(is!=NULL){
					if(is->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION){
						fun_obj=(ScriptFunction *)is->var_ref;
					}
				}else{
					THROW_RUNTIME_ERROR("error evaluating \"%s\". Cannot find function \"%s\"",function_access.c_str(),access_var[access_var.size()-1].c_str());
				}

			}else{ // some function in main function
				std::string symbol_to_find=access_var[0];
				for(unsigned i = 0; i < main_function->registered_symbols->count && fun_obj==NULL; i++){
					Symbol *symbol=(Symbol *)main_function->registered_symbols->items[i];
					if(symbol->properties & SYMBOL_PROPERTY_IS_FUNCTION){
						ScriptFunction *aux_fun_obj=(ScriptFunction *)symbol->ref_ptr;
						if(		aux_fun_obj->symbol.name  == symbol_to_find
						  && aux_fun_obj->symbol.scope == MAIN_SCOPE(this)){
							fun_obj=aux_fun_obj;
						}
					}
				}
			}

			if(fun_obj==NULL){
				THROW_RUNTIME_ERROR("error evaluating \"%s\". Variable name \"%s\" is not function type",function_access.c_str(),access_var[access_var.size()-1].c_str());
			}


			return bindScriptFunction<F>(fun_obj,calling_obj);

		}
}
