namespace zetscript{

		// Helpers...
		 StackElement ZetScript::convertVarToStackElement(zs_int ptr_var, short idx_builtin_type_var){
			//zs_int ptr_var = (zs_int)input_var;
				std::string s_return_value;
				StackElement stk_result=k_stk_undefined;
				ScriptObjectString *so=NULL;

				//int idx_builtin_type=getIdxClassFromItsNativeType(typeid(T).name());
				// save return type ...
				switch(idx_builtin_type_var){
				 case IDX_BUILTIN_TYPE_VOID_C:
					break;
				 case IDX_BUILTIN_TYPE_ZS_INT_PTR_C:
					 if(ptr_var==0) return stk_result;
					 stk_result={(*((zs_int *)ptr_var)),STK_PROPERTY_ZS_INT};
					 break;
				 case IDX_BUILTIN_TYPE_ZS_INT_C:
					 stk_result={(((zs_int)ptr_var)),STK_PROPERTY_ZS_INT};
					 break;
				 case IDX_BUILTIN_TYPE_ZS_FLOAT_C:
					 stk_result.properties=STK_PROPERTY_ZS_FLOAT;//{};
					 ZS_FLOAT_COPY(&stk_result.value,&ptr_var);
					 break;
				 case IDX_BUILTIN_TYPE_ZS_FLOAT_PTR_C:
					 if(ptr_var==0) return stk_result;
					 stk_result.properties=STK_PROPERTY_ZS_FLOAT;//{};
					 ZS_FLOAT_COPY(&stk_result.value,&(*(zs_float *)ptr_var));
					 break;
				 case IDX_BUILTIN_TYPE_BOOL_PTR_C:
					 if(ptr_var==0) return stk_result;
					 stk_result={(*((bool *)ptr_var)),STK_PROPERTY_BOOL};
					 break;
				 case IDX_BUILTIN_TYPE_BOOL_C:
					 stk_result={(((bool)ptr_var)),STK_PROPERTY_BOOL};
					 break;
				 case IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C:
				 case IDX_BUILTIN_TYPE_STRING_PTR_C:

					 if(ptr_var!=0) return stk_result;

					 so=ZS_NEW_OBJECT_STRING(this);
					 if(idx_builtin_type_var==IDX_BUILTIN_TYPE_STRING_PTR_C){
						so->value=(void *)ptr_var;

					 }

					 stk_result={(intptr_t)so,STK_PROPERTY_SCRIPT_OBJECT};
					 break;
				 case IDX_BUILTIN_TYPE_STACK_ELEMENT:
					 if(ptr_var==0) return stk_result;
					 stk_result=*((StackElement *)ptr_var);
					 break;
				 case IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR:
				 case IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR_ITERATOR:
				 case IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT:
				 case IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT_ITERATOR:
				 case IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING:
				 case IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING_ITERATOR:
					 if(ptr_var==0) return stk_result;
					stk_result = {
						 (intptr_t)ptr_var
						 ,STK_PROPERTY_SCRIPT_OBJECT
					 };

					 break;
				 default:
					 //if(ptr_var==0) return stk_result;
					 stk_result = {
							 (intptr_t) script_class_factory->instanceScriptObjectByClassIdx(idx_builtin_type_var,(void *)ptr_var)
							 ,STK_PROPERTY_SCRIPT_OBJECT
					 };
					 break;
				}

				return stk_result;
		}

		bool ZetScript::convertStackElementToVar(StackElement *stack_element, int idx_builtin_type, zs_int *ptr_var, std::string & error){
			zs_int val_ret=0;
			//ScriptObjectString *so=NULL;

			ScriptObject *script_object=NULL;


			// save return type ...
			if(stack_element->properties & STK_PROPERTY_PTR_STK){
				stack_element=((StackElement *)stack_element->value);
			}

			if(idx_builtin_type == IDX_BUILTIN_TYPE_STACK_ELEMENT){
				val_ret=(zs_int)stack_element;
			}else{

				switch(GET_STK_PROPERTY_TYPES(stack_element->properties)){
				case STK_PROPERTY_BOOL:
					if(idx_builtin_type == IDX_BUILTIN_TYPE_BOOL_C){// *ScriptClass::k_str_bool_type){
						val_ret=(zs_int)(stack_element->value);
					}else if(idx_builtin_type == IDX_BUILTIN_TYPE_BOOL_PTR_C){//*ScriptClass::k_str_bool_type_ptr){
						val_ret=(zs_int)(&stack_element->value);
					}else{
						error="cannot convert '"+zs_rtti::demangle((k_str_bool_type_ptr))+"' to '"+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,idx_builtin_type))+"'";

						return false;
					}

					break;
				case STK_PROPERTY_ZS_FLOAT:
					switch(idx_builtin_type){
					case IDX_BUILTIN_TYPE_ZS_FLOAT_C:
						ZS_FLOAT_COPY(&val_ret,&stack_element->value);
						break;
					case IDX_BUILTIN_TYPE_ZS_FLOAT_PTR_C:
						val_ret=(zs_int)(&stack_element->value);
						break;
					case IDX_BUILTIN_TYPE_ZS_INT_C:
						{
							zs_int *aux_dst = ((zs_int *)&val_ret);
							zs_float *aux_src=(zs_float *)&stack_element->value;
							*aux_dst=(zs_int)(*aux_src);
						}
						break;
					default:
						error="cannot convert '"+zs_rtti::demangle((k_str_float_type_ptr))+"' to '"+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,idx_builtin_type))+"'";
						return false;
					}
					break;
				case STK_PROPERTY_ZS_INT:
					switch(idx_builtin_type){
					case IDX_BUILTIN_TYPE_ZS_INT_C:
						val_ret=(zs_int)(stack_element->value);
						break;
					case IDX_BUILTIN_TYPE_ZS_INT_PTR_C:
						val_ret=(zs_int)(&stack_element->value);
						break;
					case IDX_BUILTIN_TYPE_ZS_FLOAT_PTR_C:
						ZS_FLOAT_COPY(&val_ret,stack_element->value);
						break;
					default:
						error= "cannot convert 'int' to '"+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,idx_builtin_type))+"'";
						return false;
					}
					break;
				case STK_PROPERTY_FUNCTION:
					val_ret=(zs_int)stack_element->value;
					break;
				default: // script variable by default ...

					if(stack_element->properties & STK_PROPERTY_SCRIPT_OBJECT){
						script_object=(ScriptObject *)stack_element->value;
						ScriptClass *c_class=NULL;
						val_ret=(zs_int)script_object;;

						if(script_object==NULL){
							error="Variable is not defined";
							return false;
						}

						if(idx_builtin_type!=script_object->idx_script_class){

							if(script_object->idx_script_class == IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING){ // string
								if(stack_element->value == 0){ // if not created try to create a tmp scriptvar it will be removed...
									error= "internal error var_ref is NULL";
									return false;
								}

								if(idx_builtin_type == IDX_BUILTIN_TYPE_STRING_PTR_C){
									val_ret=(zs_int)(((ScriptObjectString *)script_object)->value);
								}else if (idx_builtin_type == IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C){
									val_ret=(zs_int)(((std::string *)(((ScriptObjectString *)script_object)))->c_str());
								}else{
									error= "cannot convert '"+zs_rtti::demangle((k_str_string_type_ptr))+"' to '"+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,idx_builtin_type))+"'";
									return false;
								}
							}else if(script_object->idx_script_class>=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_CLASS){
								ScriptObjectClass *script_object_class = (ScriptObjectClass *)script_object;
								c_class=script_object_class->getNativeScriptClass(); // get the pointer directly ...

								if(c_class != NULL){
									if((val_ret=script_class_factory->doCast((zs_int)script_object_class->getNativeObject(),c_class->idx_class,idx_builtin_type))==0){//c_class->idx_class==idx_builtin_type){
										error = "cannot convert '"+zs_rtti::demangle(script_object_class->getNativePointerClassName())+"' to '"+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,idx_builtin_type))+"'";
										return false;
									}
								}else{ // ScriptObjectClass ?
									error = " Error calling function, no C-object parameter! Unexpected script variable ("+zs_rtti::demangle(script_object->getClassName())+")";
									return false;
								}
							}else{ // cannot convert...
								error = "cannot convert '"+zs_rtti::demangle(script_object->getClassName())+"' to '"+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,idx_builtin_type))+"'";
								return false;
							}
						}else{ // get native object...
							val_ret=(zs_int)script_object->getNativeObject();
						}
					}else{
						error= zs_strutils::format("Cannot know how to convert type '%s'",zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this,idx_builtin_type)).c_str());
						return false;
					}
					break;
				}
			}

			*ptr_var = val_ret;

			return true;
		}

		void ZetScript::unrefLifetimeObject(ScriptObject *so){
			vm_unref_lifetime_object(this->virtual_machine,so);
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 0 PARAMS
		//
		template <typename R,typename T>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<std::is_same<R,void>::value>::type
		{


			*ptr_fun=((void *)(new std::function<void ()>(
				[&,file,line,calling_obj,fun_obj](){
					bool error=false;
					vm_execute(
						virtual_machine
						,calling_obj
						,fun_obj
						,NULL
						,0
						,VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE
						,file
						,line
					);

				}
			)));
		}

		template <typename R,typename T>
		auto ZetScript::bindScriptFunctionBuilder(const char *file,int line,void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<!std::is_same<R,void>::value>::type
		{
			int idx_return = script_class_factory->getIdxClassFromItsNativeType(typeid(R).name());

			*ptr_fun=((void *)(new std::function<R ()>(
				[&,file,line,calling_obj,fun_obj,idx_return](){
						std::string error_str;
						R ret_value;

						StackElement stk = vm_execute(
								virtual_machine
								,calling_obj
								,fun_obj
								,NULL
								,0
								,VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE
								,file
								,line
						);

						if(!convertStackElementToVar(&stk, idx_return, (zs_int *)(&ret_value),error_str)){
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
		auto ZetScript::bindScriptFunctionBuilder(const char *file,int line,void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type
		{
			//return NULL;

			using Param1 = typename T::template Argument<0>::type;
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());

			*ptr_fun=((void *)(new std::function<void (Param1)>(
				[&,file,line,calling_obj,fun_obj, idx_param1](Param1 p1){

					StackElement args[1]={
							 convertVarToStackElement((zs_int)p1,idx_param1)
					};

					vm_execute(
								virtual_machine
								,calling_obj
								,fun_obj
								,args
								,1
								,VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE
								,file
								,line);
				}
			)));
		}

		template<typename R,typename T,  typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file,int line,void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type
		{
			using Param1 = typename T::template Argument<0>::type;

			int idx_return = script_class_factory->getIdxClassFromItsNativeType(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());


			*ptr_fun=((void *)(new std::function<R (Param1)>(
				[&,file,line,calling_obj,fun_obj,idx_return, idx_param1](Param1 p1){

						R ret_value;
						std::string error_str;

						StackElement args[1]={
								convertVarToStackElement((zs_int)p1,idx_param1)
						};

						StackElement stk = vm_execute(
								virtual_machine
								,calling_obj
								,fun_obj
								,args
								,1
								,VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE
								,file
								,line);

						if(!convertStackElementToVar(&stk,idx_return, (zs_int*)(&ret_value),error_str)){
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
		auto ZetScript::bindScriptFunctionBuilder(const char *file,int line,void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;


			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param2).name());

			*ptr_fun=((void *)(new std::function<void (Param1,Param2)>(
				[&,file,line,calling_obj,fun_obj, idx_param1, idx_param2](Param1 p1,Param2 p2){

					StackElement args[2]={
							 convertVarToStackElement((zs_int)p1,idx_param1)
							,convertVarToStackElement((zs_int)p2,idx_param2)

					};

					vm_execute(
						virtual_machine
						,calling_obj
						,fun_obj
						,args
						,2
						,VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE
						,file
						,line
					);

				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file,int line,void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type
		{
			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;


			int idx_return = script_class_factory->getIdxClassFromItsNativeType(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param2).name());

			*ptr_fun=((void *)(new std::function<R (Param1,Param2)>(
				[&,file,line,calling_obj,fun_obj,idx_return, idx_param1, idx_param2](Param1 p1,Param2 p2){

						R ret_value;
						std::string error_str;

						StackElement args[2]={
								 convertVarToStackElement((zs_int)p1,idx_param1)
								,convertVarToStackElement((zs_int)p2,idx_param2)

						};

						StackElement stk = vm_execute(
							virtual_machine
							,calling_obj
							,fun_obj
							,args
							,2
							,VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE
							,file
							,line
						);

						if(!convertStackElementToVar(&stk, idx_return, (zs_int*)(&ret_value),error_str)){
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
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;


			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param3).name());


			*ptr_fun=((void *)(new std::function<void (Param1,Param2,Param3)>(
				[&,file,line,calling_obj,fun_obj,idx_param1, idx_param2, idx_param3](Param1 p1,Param2 p2,Param3 p3){


					StackElement args[3]={
							 convertVarToStackElement((zs_int)p1,idx_param1)
							,convertVarToStackElement((zs_int)p2,idx_param2)
							,convertVarToStackElement((zs_int)p3,idx_param3)
					};

					vm_execute(
							virtual_machine
							,calling_obj
							,fun_obj
							,args
							,3
							,VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE
							,file
							,line
					);

				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type
		{
			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;

			int idx_return = script_class_factory->getIdxClassFromItsNativeType(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param1).name());
			int idx_param2 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param2).name());
			int idx_param3 = script_class_factory->getIdxClassFromItsNativeType(typeid(Param3).name());

			*ptr_fun=((void *)(new std::function<R (Param1,Param2,Param3)>(
				[&,file,line,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3](Param1 p1,Param2 p2,Param3 p3){
					R ret_value;
					std::string error_str;

					StackElement args[3]={
							 convertVarToStackElement((zs_int)p1,idx_param1)
							,convertVarToStackElement((zs_int)p2,idx_param2)
							,convertVarToStackElement((zs_int)p3,idx_param3)
					};

					StackElement stk = vm_execute(
						virtual_machine
						,calling_obj
						,fun_obj
						,args
						,3
						,VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE
						,file
						,line
					);

					if(!convertStackElementToVar(&stk, idx_return, (zs_int *)(&ret_value),error_str)){
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
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
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


			*ptr_fun=((void *)(new std::function<void (Param1,Param2,Param3,Param4)>(
				[&,file,line,calling_obj,fun_obj, idx_param1, idx_param2, idx_param3, idx_param4](Param1 p1,Param2 p2,Param3 p3,Param4 p4){

					StackElement args[4]={
							 convertVarToStackElement((zs_int)p1,idx_param1)
							,convertVarToStackElement((zs_int)p2,idx_param2)
							,convertVarToStackElement((zs_int)p3,idx_param3)
							,convertVarToStackElement((zs_int)p4,idx_param4)
					};

					vm_execute(
						virtual_machine
						,calling_obj
						,fun_obj
						,args
						,4
						,VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE
						,file
						,line
					);

				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
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

			*ptr_fun=((void *)(new std::function<R (Param1,Param2,Param3,Param4)>(
				[&,file,line,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4](Param1 p1,Param2 p2,Param3 p3,Param4 p4){
						R ret_value;
						std::string error_str;

						StackElement args[4]={
								 convertVarToStackElement((zs_int)p1,idx_param1)
								,convertVarToStackElement((zs_int)p2,idx_param2)
								,convertVarToStackElement((zs_int)p3,idx_param3)
								,convertVarToStackElement((zs_int)p4,idx_param4)
						};

						StackElement stk = vm_execute(
								virtual_machine
								,calling_obj
								,fun_obj
								,args
								,4
								,VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE
								,file
								,line
								);

						if(!convertStackElementToVar(&stk, idx_return, (zs_int*)(&ret_value),error_str)){
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
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
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


			*ptr_fun=((void *)(new std::function<void (Param1,Param2,Param3,Param4,Param5)>(
				[&,file,line,calling_obj,fun_obj,idx_param1, idx_param2, idx_param3, idx_param4, idx_param5](Param1 p1,Param2 p2,Param3 p3,Param4 p4,Param5 p5){

					StackElement args[5]={
							 convertVarToStackElement((zs_int)p1,idx_param1)
							,convertVarToStackElement((zs_int)p2,idx_param2)
							,convertVarToStackElement((zs_int)p3,idx_param3)
							,convertVarToStackElement((zs_int)p4,idx_param4)
							,convertVarToStackElement((zs_int)p5,idx_param5)

					};

					vm_execute(
							virtual_machine
							,calling_obj
							,fun_obj
							,args
							,5
							,VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE
							,file
							,line);
				}

			)));
		}


		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
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

			*ptr_fun=((void *)(new std::function<R (Param1,Param2,Param3,Param4,Param5)>(
				[&,file,line,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5](Param1 p1,Param2 p2,Param3 p3,Param4 p4,Param5 p5){

					R ret_value;
					std::string error_str;

					StackElement args[5]={
							 convertVarToStackElement((zs_int)p1,idx_param1)
							,convertVarToStackElement((zs_int)p2,idx_param2)
							,convertVarToStackElement((zs_int)p3,idx_param3)
							,convertVarToStackElement((zs_int)p4,idx_param4)
							,convertVarToStackElement((zs_int)p5,idx_param5)

					};

					StackElement stk = vm_execute(
							virtual_machine
							,calling_obj
							,fun_obj
							,args
							,5
							,VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE
							,file
							,line);

					if(!convertStackElementToVar(&stk, idx_return, (zs_int*)(&ret_value),error_str)){
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
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
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




			*ptr_fun=((void *)(new std::function<void (Param1,Param2,Param3,Param4,Param5,Param6)>(
				[&,file,line,calling_obj,fun_obj, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5, idx_param6](Param1 p1,Param2 p2,Param3 p3,Param4 p4,Param5 p5,Param6 p6){

					StackElement args[6]={
							 convertVarToStackElement((zs_int)p1,idx_param1)
							,convertVarToStackElement((zs_int)p2,idx_param2)
							,convertVarToStackElement((zs_int)p3,idx_param3)
							,convertVarToStackElement((zs_int)p4,idx_param4)
							,convertVarToStackElement((zs_int)p5,idx_param5)
							,convertVarToStackElement((zs_int)p6,idx_param6)
					};

					vm_execute(
							 virtual_machine
							,calling_obj
							,fun_obj
							,args
							,6
							,VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE
							,file
							,line);
				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
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

			*ptr_fun=((void *)(new std::function<R (Param1,Param2,Param3,Param4,Param5,Param6)>(
				[&,file,line,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5, idx_param6](Param1 p1,Param2 p2,Param3 p3,Param4 p4,Param5 p5,Param6 p6){

						R ret_value;
						std::string error_str;

						StackElement args[6]={
								 convertVarToStackElement((zs_int)p1,idx_param1)
								,convertVarToStackElement((zs_int)p2,idx_param2)
								,convertVarToStackElement((zs_int)p3,idx_param3)
								,convertVarToStackElement((zs_int)p4,idx_param4)
								,convertVarToStackElement((zs_int)p5,idx_param5)
								,convertVarToStackElement((zs_int)p6,idx_param6)
						};

						StackElement stk = vm_execute(
								virtual_machine,
								calling_obj,
								fun_obj,
								args,
								6
								,VM_EXECUTE_PROPERTY_CALL_FROM_NATIVE
								,file
								,line);

						if(!convertStackElementToVar(&stk, idx_return, (zs_int *)(&ret_value),error_str)){
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
		 auto ZetScript::bindScriptFunctionBuilderBase(const char *file, int line,void **ptr_fun, ScriptObjectClass *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity > 0)>::type
		{
			 bindScriptFunctionBuilder<typename F::return_type, F,  typename F::template Argument<Is>::type...>(file,line,ptr_fun,calling_obj,fun_obj);
		}



		 template <typename F, std::size_t... Is>
		 auto ZetScript::bindScriptFunctionBuilderBase(const char *file, int line, void **ptr_fun, ScriptObjectClass *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity == 0)>::type
		{
			 bindScriptFunctionBuilder<typename F::return_type, F>(file,line,ptr_fun,calling_obj,fun_obj);
		}

		template <  typename F>
		std::function<F> * ZetScript::bindScriptFunction(ScriptFunction *fun,ScriptObjectClass *calling_obj, const char *file, int line){

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
				THROW_RUNTIME_ERROR("Return type '%s' for bind function not registered",zs_rtti::demangle(return_type).c_str());
				return NULL;
			}

			for(unsigned int i = 0; i < arg.size(); i++){
				if(script_class_factory->getIdxClassFromItsNativeType(arg[i])==-1){
					THROW_RUNTIME_ERROR("Argument %i type '%s' for bind function not registered"
							,i+1
							,zs_rtti::demangle(arg[i]).c_str());
					return NULL;
				}
			}

			// 3. build custom function in function of parameter number ...
			bindScriptFunctionBuilderBase<Traits3>(file,line,&ptr,calling_obj,fun,MakeIndexSequence<Traits3::arity>{});
			return (std::function<F> *)ptr;

		}

		template <  typename F>
		std::function<F> * ZetScript::bindScriptFunction(ScriptFunction *fun, const char *file, int line){
			return bindScriptFunction<F>(fun,NULL, file, line);
		}


		template <  typename F>
		std::function<F> * ZetScript::bindScriptFunction(const std::string & function_access, const char *file, int line)
		{
			ScriptFunction * fun_obj=NULL;
			ScriptObjectClass *calling_obj=NULL;
			std::vector<std::string> access_var = zs_strutils::split(function_access,'.');
			ScriptFunction * main_function = script_class_factory->getMainFunction();
			StackElement *se=NULL;
			Symbol *symbol_sfm=NULL;


			// 1. some variable in main function ...
			if(access_var.size()>1){
				for(unsigned i=0; i < access_var.size()-1; i++){
					std::string symbol_to_find=access_var[i];
					if(i==0){ // get variable through main_class.main_function (global element)
						for(unsigned j = 0; j < main_function->symbol_registered_functions->size() && calling_obj==NULL; j++){
							Symbol * registered_symbol=(Symbol *)main_function->symbol_registered_functions->at(j);
							if(registered_symbol->name==symbol_to_find
							&& registered_symbol->scope == MAIN_SCOPE(this)){
								StackElement *stk = vm_get_stack_element_at(virtual_machine,j); // main_function->object_info.local_symbols.variable[j].
								if(stk!=NULL){
									if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){
										calling_obj=(ScriptObjectClass *)stk->value;
									}
								}
								else{
									THROW_SCRIPT_ERROR_FILE_LINE(file,line,"Cannot bind script function '%s': cannot access i (%i)",function_access.c_str(),j);
								}
							}
						}

						if(calling_obj == NULL){
							THROW_SCRIPT_ERROR_FILE_LINE(file,line,"Cannot bind script function '%s': Variable name '%s' doesn't exist",function_access.c_str(),symbol_to_find.c_str());
						}

					}else{ // we have got the calling_obj from last iteration ...
						se = calling_obj->getProperty(symbol_to_find);
						if(se!=NULL){
							if(se->properties & STK_PROPERTY_SCRIPT_OBJECT){
								calling_obj=(ScriptObjectClass *)se->value;
							}else{
								THROW_SCRIPT_ERROR_FILE_LINE(file,line,"Cannot bind script function '%s': Variable name '%s' not script variable",function_access.c_str(),symbol_to_find.c_str());
							}
						}
						else{
							THROW_SCRIPT_ERROR_FILE_LINE(file,line,"Cannot bind script function '%s': Variable name '%s' doesn't exist",function_access.c_str(),symbol_to_find.c_str());
						}
					}
				}

				symbol_sfm=calling_obj->getScriptClass()->getSymbolMemberFunction(access_var[access_var.size()-1]);
				if(symbol_sfm!=NULL){
					if(symbol_sfm->properties & (SYMBOL_PROPERTY_FUNCTION | SYMBOL_PROPERTY_MEMBER_FUNCTION)){
						fun_obj=(ScriptFunction *)symbol_sfm->ref_ptr;
					}
				}else{
					THROW_SCRIPT_ERROR_FILE_LINE(file,line,"Cannot bind script function '%s': Cannot find function '%s'",function_access.c_str(),access_var[access_var.size()-1].c_str());
				}

			}else{ // some function in main function
				std::string symbol_to_find=access_var[0];
				for(unsigned i = 0; i < main_function->symbol_registered_functions->size() && fun_obj==NULL; i++){
					Symbol *symbol=(Symbol *)main_function->symbol_registered_functions->at(i);
					ScriptFunction *aux_fun_obj=(ScriptFunction *)symbol->ref_ptr;
					if(		aux_fun_obj->symbol.name  == symbol_to_find
					  && aux_fun_obj->symbol.scope == MAIN_SCOPE(this)){
						fun_obj=aux_fun_obj;
					}

				}
			}

			if(fun_obj==NULL){
				THROW_SCRIPT_ERROR_FILE_LINE(file,line,"Cannot bind script function '%s': Variable name '%s' is not function type",function_access.c_str(),access_var[access_var.size()-1].c_str());
			}

			try{
				return bindScriptFunction<F>(fun_obj,calling_obj,file,line);
			}catch(std::exception & ex){
				THROW_SCRIPT_ERROR_FILE_LINE(file,line,"Cannot bind script function '%s': %s",function_access.c_str(),ex.what());
			}

			return NULL;

		}
}
