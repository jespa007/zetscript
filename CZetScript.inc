namespace zetscript{



		// Helpers...
		inline StackElement CZetScript::var_2_stk(intptr_t var_trans, int idx_type){
			//intptr_t var_trans = (intptr_t)input_var;
				std::string s_return_value;
				StackElement callc_result={0,0,STK_PROPERTY_TYPE_UNDEFINED};
				//int idx_type=getIdxClassFromIts_C_Type(typeid(T).name());
				// save return type ...
				switch(idx_type){
				 case IDX_TYPE_VOID_C:
					break;
				 case IDX_TYPE_INT_PTR_C:
					 if(var_trans==0) return callc_result;
					 callc_result={(void *)(*((intptr_t *)var_trans)),NULL,STK_PROPERTY_TYPE_INTEGER};
					 break;
				 case IDX_TYPE_UNSIGNED_INT_C:
				 case IDX_TYPE_INTPTR_T_C:
				 case IDX_TYPE_INT_C:
					 callc_result={(void *)(((intptr_t)var_trans)),NULL,STK_PROPERTY_TYPE_INTEGER};
					 break;
				 case IDX_TYPE_FLOAT_C:
					 callc_result.properties=STK_PROPERTY_TYPE_NUMBER;//{};
					 memcpy(&callc_result.stkValue,&var_trans,sizeof(float));

					 break;
				 case IDX_TYPE_FLOAT_PTR_C:
					 if(var_trans==0) return callc_result;
					 callc_result.properties=STK_PROPERTY_TYPE_NUMBER;//{};
					 memcpy(&callc_result.stkValue,&(*(float *)var_trans),sizeof(float));
					 break;
				 case IDX_TYPE_BOOL_PTR_C:
					 if(var_trans==0) return callc_result;
					 callc_result={(void *)(*((bool *)var_trans)),NULL,STK_PROPERTY_TYPE_BOOLEAN};
					 break;
				 case IDX_TYPE_BOOL_C:
					 callc_result={(void *)(((bool)var_trans)),NULL,STK_PROPERTY_TYPE_BOOLEAN};
					 break;
				 case IDX_TYPE_CONST_CHAR_PTR_C:
					 if(var_trans==0) return callc_result;
					 callc_result={(void *)var_trans,NULL,STK_PROPERTY_TYPE_STRING};//new std::string(*((std::string *)result))};
					 break;
				 case IDX_TYPE_STRING_PTR_C:
					 if(var_trans==0) return callc_result;
					 callc_result={(void *)((std::string *)var_trans)->c_str(),NULL,STK_PROPERTY_TYPE_STRING};//new std::string(*((std::string *)result))};
					 break;
				 case IDX_STACK_ELEMENT:
					 if(var_trans==0) return callc_result;
					 callc_result=*((StackElement *)var_trans);//{STK_PROPERTY_TYPE_STRING,(void *)((std::string *)var_trans)->c_str(),NULL};//new std::string(*((std::string *)result))};
					 break;
				 default:
					 if(var_trans==0) return callc_result;
					 callc_result = {NULL,script_class_factory->instanceScriptVariableByIdx(idx_type,(void *)var_trans),STK_PROPERTY_TYPE_SCRIPTVAR};
					 break;
				}

				return callc_result;
		}

		inline bool CZetScript::stk_2_var(StackElement *stk_src, int idx_dst_type, intptr_t *result, std::string & error){
			intptr_t val_ret=0;

			CScriptVariable *script_variable=NULL;


			// save return type ...
			if(stk_src->properties & STK_PROPERTY_IS_STACKVAR){
				stk_src=((StackElement *)stk_src->varRef);
			}

			if(idx_dst_type == IDX_STACK_ELEMENT){//*CScriptClass::STACK_ELEMENT_PTR)){// && (stk_src->properties & STK_PROPERTY_IS_STACKVAR)){ // set directly stackvar
				val_ret=(intptr_t)stk_src;
			}else{

				switch(GET_INS_PROPERTY_VAR_TYPE(stk_src->properties)){
				case STK_PROPERTY_TYPE_BOOLEAN:
					if(idx_dst_type == IDX_TYPE_BOOL_C){// *CScriptClass::STR_BOOL_TYPE){
						val_ret=(intptr_t)(stk_src->stkValue);
					}else if(idx_dst_type == IDX_TYPE_BOOL_PTR_C){//*CScriptClass::STR_BOOL_TYPE_PTR){
						val_ret=(intptr_t)(&stk_src->stkValue);
					}else{
						error="cannot convert "+rtti::demangle((STR_STRING_TYPE_PTR))+" into %s"+rtti::demangle(GET_IDX_2_CLASS_C_STR(idx_dst_type)).c_str();

						return false;
					}

					break;
				case STK_PROPERTY_TYPE_NUMBER:
					switch(idx_dst_type){
					case IDX_TYPE_FLOAT_C:
						memcpy(&val_ret,&stk_src->stkValue,sizeof(float));
						break;
					case IDX_TYPE_FLOAT_PTR_C:
						val_ret=(intptr_t)(&stk_src->stkValue);
						break;
					case IDX_TYPE_UNSIGNED_INT_C:
					case IDX_TYPE_INTPTR_T_C:
					case IDX_TYPE_INT_C:
						{
							int *aux_dst = ((int *)&val_ret);
							float *aux_src=(float *)&stk_src->stkValue;
							*aux_dst=(int)(*aux_src);
						}
						break;
					default:
						error="cannot convert "+rtti::demangle((STR_STRING_TYPE_PTR))+" into %s"+rtti::demangle(GET_IDX_2_CLASS_C_STR(idx_dst_type));
						return false;
					}
					break;
				case STK_PROPERTY_TYPE_INTEGER:
					switch(idx_dst_type){
					case IDX_TYPE_UNSIGNED_INT_C:
					case IDX_TYPE_INTPTR_T_C:
					case IDX_TYPE_INT_C:
						val_ret=(intptr_t)(stk_src->stkValue);
						break;
					case IDX_TYPE_INT_PTR_C:
						val_ret=(intptr_t)(&stk_src->stkValue);
						break;
					case IDX_TYPE_FLOAT_C:
						{
							float *aux_dst = ((float *)&val_ret);
							int *aux_src=(int *)&stk_src->stkValue;
							*aux_dst = (float)(*aux_src);
						}
						break;
					default:
						error= "cannot convert "+rtti::demangle((STR_STRING_TYPE_PTR))+" into "+rtti::demangle(GET_IDX_2_CLASS_C_STR(idx_dst_type));
						return false;
					}
					break;

				case STK_PROPERTY_TYPE_STRING:
					if(idx_dst_type == IDX_TYPE_STRING_PTR_C){
						if(stk_src->varRef != 0){
							val_ret=(intptr_t)(&((CStringScriptVariable *)(stk_src->varRef))->m_strValue);
						}
						else{ // pass param std::string ...
							error= "(std::string *)Expected varRef not NULL";
							return false;
						}

					}else if (idx_dst_type == IDX_TYPE_CONST_CHAR_PTR_C){
						val_ret=(intptr_t)(stk_src->stkValue);
					}else{
						error= "cannot convert "+rtti::demangle((STR_STRING_TYPE_PTR))+" into "+rtti::demangle(GET_IDX_2_CLASS_C_STR(idx_dst_type));
						return false;
					}


					break;
				default: // script variable by default ...

					script_variable=(CScriptVariable *)stk_src->varRef;
					CScriptClass *c_class=NULL;

					if(script_variable==NULL){

						error="Variable is not defined";
						return false;
					}

					if(script_variable->idx_class==IDX_CLASS_STRING){

						val_ret=(intptr_t)(&script_variable->m_strValue);
						if(idx_dst_type == IDX_TYPE_CONST_CHAR_PTR_C){
							val_ret=(intptr_t)script_variable->m_strValue.c_str();
						}
					}else if(

					   (script_variable->idx_class==IDX_CLASS_VECTOR
					|| script_variable->idx_class==IDX_CLASS_DICTIONARY)){

						if(idx_dst_type==script_variable->idx_class){
							val_ret=(intptr_t)script_variable->get_C_Object();
						}

					}else if((c_class=script_variable->get_C_Class())!=NULL){ // get the pointer directly ...

						if(c_class->idx_class==idx_dst_type){
							val_ret=(intptr_t)script_variable->get_C_Object();
						}
						else if((val_ret=script_class_factory->doCast((intptr_t)script_variable->get_C_Object(),c_class->idx_class,idx_dst_type))==0){//c_class->idx_class==idx_dst_type){
							error = "cannot convert "+rtti::demangle(script_variable->getPointer_C_ClassName())+" into "+rtti::demangle(GET_IDX_2_CLASS_C_STR(idx_dst_type));
							return false;
						}
					}else{ // CScriptVariable ?
						error = " Error calling function, no C-object parameter! Unexpected script variable ("+rtti::demangle(script_variable->getClassName())+")";
						return false;
					}
					break;
				}
			}

			*result = val_ret;

			return true;
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 0 PARAMS
		//
		template <typename R,typename T>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		->typename std::enable_if<std::is_same<R,void>::value>::type
		{


			*f=((void *)(new std::function<void ()>(
				[&,calling_obj,fun_obj](){
					bool error=false;
					virtual_machine->execute(
										fun_obj,
										calling_obj,
										error);
					if(error){

						THROW_SCRIPT_ERROR();

					}
				}
			)));
		}

		template <typename R,typename T>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		->typename std::enable_if<!std::is_same<R,void>::value>::type
		{
			int idx_return = script_class_factory->getIdxClassFromIts_C_Type(typeid(R).name());

			*f=((void *)(new std::function<R ()>(
				[&,calling_obj,fun_obj,idx_return](){
						std::string error_str;
						bool error=false;
						R ret_value;

						StackElement stk = virtual_machine->execute(
								fun_obj,
								calling_obj,
								error);


						if(error){
							THROW_SCRIPT_ERROR();//THROW_RUNTIME_ERROR(std::string("run-time error"));
						}


						if(!stk_2_var(&stk, idx_return, (intptr_t *)(&ret_value),error_str)){
							THROW_RUNTIME_ERROR(string::sformat("run-time error converting result value:%s",error_str.c_str()));
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
		auto CZetScript::bindScriptFunctionBuilder(void **f ,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type
		{
			//return NULL;

			using tParam1 = typename T::template argument<0>::type;
			int idx_param1 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam1).name());

			*f=((void *)(new std::function<void (tParam1)>(
				[&,calling_obj,fun_obj, idx_param1](tParam1 p1){

					bool error=false;
					std::vector<StackElement> args={
							 var_2_stk((intptr_t)p1,idx_param1)

					};

					virtual_machine->execute(
								fun_obj,
								calling_obj,
								error,
								args);
					if(error){
						THROW_SCRIPT_ERROR();//(std::string("run-time error"));
					}
				}
			)));
		}

		template<typename R,typename T,  typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f ,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type
		{
			using tParam1 = typename T::template argument<0>::type;

			int idx_return = script_class_factory->getIdxClassFromIts_C_Type(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam1).name());


			*f=((void *)(new std::function<R (tParam1)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1](tParam1 p1){

						R ret_value;
						std::string error_str;
						bool error=false;

						std::vector<StackElement> args={
								 var_2_stk((intptr_t)p1,idx_param1)
						};

						StackElement stk = virtual_machine->execute(
													fun_obj,
													calling_obj,
													error,
													args);

						if(error){
							THROW_SCRIPT_ERROR();//(std::string("run-time error"));
						}

						if(!stk_2_var(&stk,idx_return, (intptr_t*)(&ret_value),error_str)){
							THROW_RUNTIME_ERROR(string::sformat("run-time error converting result value:%s",error_str.c_str()));
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
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;


			int idx_param1 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam2).name());

			*f=((void *)(new std::function<void (tParam1,tParam2)>(
				[&,calling_obj,fun_obj, idx_param1, idx_param2](tParam1 p1,tParam2 p2){

					bool error=false;

					std::vector<StackElement> args={
							 var_2_stk((intptr_t)p1,idx_param1)
							,var_2_stk((intptr_t)p2,idx_param2)

					};

					virtual_machine->execute(
									fun_obj,
									calling_obj,
									error,
									args);


					if(error){
						THROW_SCRIPT_ERROR();//(std::string("run-time error"));
					}
				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type
		{
			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;


			int idx_return = script_class_factory->getIdxClassFromIts_C_Type(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam2).name());

			*f=((void *)(new std::function<R (tParam1,tParam2)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2](tParam1 p1,tParam2 p2){

						R ret_value;
						std::string error_str;
						bool error=false;

						std::vector<StackElement> args={
								 var_2_stk((intptr_t)p1,idx_param1)
								,var_2_stk((intptr_t)p2,idx_param2)

						};

						StackElement stk = virtual_machine->execute(
													fun_obj,
													calling_obj,
													error,
													args);

						if(error){
							THROW_SCRIPT_ERROR();//(std::string("run-time error"));
						}

						if(!stk_2_var(&stk, idx_return, (intptr_t*)(&ret_value),error_str)){
							THROW_RUNTIME_ERROR(string::sformat("run-time error converting result value:%s",error_str.c_str()));
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
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;


			int idx_param1 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam2).name());
			int idx_param3 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam3).name());


			*f=((void *)(new std::function<void (tParam1,tParam2,tParam3)>(
				[&,calling_obj,fun_obj,idx_param1, idx_param2, idx_param3](tParam1 p1,tParam2 p2,tParam3 p3){

					bool error=false;

					std::vector<StackElement> args={
							 var_2_stk((intptr_t)p1,idx_param1)
							,var_2_stk((intptr_t)p2,idx_param2)
							,var_2_stk((intptr_t)p3,idx_param3)
					};

					virtual_machine->execute(
									fun_obj,
									calling_obj,
									error,
									args);

					if(error){
						THROW_SCRIPT_ERROR();//(std::string("run-time error"));
					}
				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type
		{
			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;

			int idx_return = script_class_factory->getIdxClassFromIts_C_Type(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam2).name());
			int idx_param3 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam3).name());

			*f=((void *)(new std::function<R (tParam1,tParam2,tParam3)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3](tParam1 p1,tParam2 p2,tParam3 p3){
					R ret_value;
					std::string error_str;
					bool error=false;

					std::vector<StackElement> args={
							 var_2_stk((intptr_t)p1,idx_param1)
							,var_2_stk((intptr_t)p2,idx_param2)
							,var_2_stk((intptr_t)p3,idx_param3)
					};

					StackElement stk = virtual_machine->execute(
												fun_obj,
												calling_obj,
												error,
												args);

					if(error){
						THROW_SCRIPT_ERROR();//(std::string("run-time error"));
					}

					if(!stk_2_var(&stk, idx_return, (intptr_t *)(&ret_value),error_str)){
						THROW_RUNTIME_ERROR(string::sformat("run-time error converting result value:%s",error_str.c_str()));
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
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;

			int idx_param1 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam2).name());
			int idx_param3 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam3).name());
			int idx_param4 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam4).name());


			*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4)>(
				[&,calling_obj,fun_obj, idx_param1, idx_param2, idx_param3, idx_param4](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4){

					bool error=false;

					std::vector<StackElement> args={
							 var_2_stk((intptr_t)p1,idx_param1)
							,var_2_stk((intptr_t)p2,idx_param2)
							,var_2_stk((intptr_t)p3,idx_param3)
							,var_2_stk((intptr_t)p4,idx_param4)

					};

					virtual_machine->execute(
									fun_obj,
									calling_obj,
									error,
									args);

					if(error){
						THROW_SCRIPT_ERROR();//(std::string("run-time error"));
					}
				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;

			int idx_return = script_class_factory->getIdxClassFromIts_C_Type(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam2).name());
			int idx_param3 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam3).name());
			int idx_param4 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam4).name());

			*f=((void *)(new std::function<R (tParam1,tParam2,tParam3,tParam4)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4){
						R ret_value;
						bool error=false;
						std::string error_str;

						std::vector<StackElement> args={
								 var_2_stk((intptr_t)p1,idx_param1)
								,var_2_stk((intptr_t)p2,idx_param2)
								,var_2_stk((intptr_t)p3,idx_param3)
								,var_2_stk((intptr_t)p4,idx_param4)

						};

						StackElement stk = virtual_machine->execute(
													fun_obj,
													calling_obj,
													error,
													args);

						if(error){
							THROW_SCRIPT_ERROR();//(std::string("run-time error"));
						}

						if(!stk_2_var(&stk, idx_return, (intptr_t*)(&ret_value),error_str)){
							THROW_RUNTIME_ERROR(string::sformat("run-time error converting result value:%s",error_str.c_str()));
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
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;
			using tParam5 = typename T::template argument<4>::type;



			int idx_param1 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam2).name());
			int idx_param3 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam3).name());
			int idx_param4 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam4).name());
			int idx_param5 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam5).name());


			*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4,tParam5)>(
				[&,calling_obj,fun_obj,idx_param1, idx_param2, idx_param3, idx_param4, idx_param5](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5){

					bool error=false;
					std::vector<StackElement> args={
							 var_2_stk((intptr_t)p1,idx_param1)
							,var_2_stk((intptr_t)p2,idx_param2)
							,var_2_stk((intptr_t)p3,idx_param3)
							,var_2_stk((intptr_t)p4,idx_param4)
							,var_2_stk((intptr_t)p5,idx_param5)

					};

					virtual_machine->execute(
									fun_obj,
									calling_obj,
									error,
									args);
					if(error){
						THROW_SCRIPT_ERROR();//(std::string("run-time error"));
					}
				}

			)));
		}


		template <typename R,typename T, typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;
			using tParam5 = typename T::template argument<4>::type;

			int idx_return = script_class_factory->getIdxClassFromIts_C_Type(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam2).name());
			int idx_param3 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam3).name());
			int idx_param4 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam4).name());
			int idx_param5 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam5).name());

			*f=((void *)(new std::function<R (tParam1,tParam2,tParam3,tParam4,tParam5)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5){


					R ret_value;
					bool error=false;
					std::string error_str;

					std::vector<StackElement> args={
							 var_2_stk((intptr_t)p1,idx_param1)
							,var_2_stk((intptr_t)p2,idx_param2)
							,var_2_stk((intptr_t)p3,idx_param3)
							,var_2_stk((intptr_t)p4,idx_param4)
							,var_2_stk((intptr_t)p5,idx_param5)

					};

					StackElement stk = virtual_machine->execute(
												fun_obj,
												calling_obj,
												error,
												args);

					if(error){
						THROW_SCRIPT_ERROR();//(std::string("run-time error"));
					}

					if(!stk_2_var(&stk, idx_return, (intptr_t*)(&ret_value),error_str)){
						THROW_RUNTIME_ERROR(string::sformat("run-time error converting result value:%s",error_str.c_str()));
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
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 6)>::type
		{

			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;
			using tParam5 = typename T::template argument<4>::type;
			using tParam6 = typename T::template argument<5>::type;


			int idx_param1 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam2).name());
			int idx_param3 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam3).name());
			int idx_param4 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam4).name());
			int idx_param5 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam5).name());
			int idx_param6 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam6).name());




			*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4,tParam5,tParam6)>(
				[&,calling_obj,fun_obj, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5, idx_param6](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5,tParam6 p6){
					bool error=false;
					std::vector<StackElement> args={
							 var_2_stk((intptr_t)p1,idx_param1)
							,var_2_stk((intptr_t)p2,idx_param2)
							,var_2_stk((intptr_t)p3,idx_param3)
							,var_2_stk((intptr_t)p4,idx_param4)
							,var_2_stk((intptr_t)p5,idx_param5)
							,var_2_stk((intptr_t)p6,idx_param6)

					};

					virtual_machine->execute(
									fun_obj,
									calling_obj,
									error,
									args);
					if(error){

						THROW_SCRIPT_ERROR();//(std::string("run-time error"));
					}
				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto CZetScript::bindScriptFunctionBuilder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 6)>::type
		{
			using tParam1 = typename T::template argument<0>::type;
			using tParam2 = typename T::template argument<1>::type;
			using tParam3 = typename T::template argument<2>::type;
			using tParam4 = typename T::template argument<3>::type;
			using tParam5 = typename T::template argument<4>::type;
			using tParam6 = typename T::template argument<5>::type;

			int idx_return = script_class_factory->getIdxClassFromIts_C_Type(typeid(R).name());
			int idx_param1 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam1).name());
			int idx_param2 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam2).name());
			int idx_param3 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam3).name());
			int idx_param4 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam4).name());
			int idx_param5 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam5).name());
			int idx_param6 = script_class_factory->getIdxClassFromIts_C_Type(typeid(tParam6).name());

			*f=((void *)(new std::function<R (tParam1,tParam2,tParam3,tParam4,tParam5,tParam6)>(
				[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5, idx_param6](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5,tParam6 p6){

						R ret_value;
						bool error=false;
						std::string error_str;

						std::vector<StackElement> args={
								 var_2_stk((intptr_t)p1,idx_param1)
								,var_2_stk((intptr_t)p2,idx_param2)
								,var_2_stk((intptr_t)p3,idx_param3)
								,var_2_stk((intptr_t)p4,idx_param4)
								,var_2_stk((intptr_t)p5,idx_param5)
								,var_2_stk((intptr_t)p6,idx_param6)
						};

						StackElement stk = virtual_machine->execute(
													fun_obj,
													calling_obj,
													error,
													args);

						if(error){
							THROW_SCRIPT_ERROR();//(std::string("run-time error"));
						}

						if(!stk_2_var(&stk, idx_return, (intptr_t *)(&ret_value),error_str)){
							THROW_RUNTIME_ERROR(string::sformat("run-time error converting result value:%s",error_str.c_str()));
						}
						return ret_value;

				}
			)));
		}


		//
		//
		//--------------------------------------------------------------------------------------------------------------------

		 template <typename F, std::size_t... Is>
		 auto CZetScript::bindScriptFunctionBuilderBase(void **f, CScriptVariable *calling_obj,CScriptFunction *fun_obj,index_sequence<Is...>)
		 -> typename std::enable_if<(F::arity > 0)>::type
		{
			 bindScriptFunctionBuilder<typename F::return_type, F,  typename F::template argument<Is>::type...>(f,calling_obj,fun_obj);
		}

		 template <typename F, std::size_t... Is>
		 auto CZetScript::bindScriptFunctionBuilderBase(void **f, CScriptVariable *calling_obj,CScriptFunction *fun_obj,index_sequence<Is...>)
		 -> typename std::enable_if<(F::arity == 0)>::type
		{
			 bindScriptFunctionBuilder<typename F::return_type, F>(f,calling_obj,fun_obj);
		}


		bool CZetScript::getScriptObject(const std::string &function_access,CScriptVariable **calling_obj,CScriptFunction **fun_obj ){

			//ZS_CLEAR_ERROR_MSG();

			std::vector<std::string> access_var = string::split(function_access,'.');
			//CScriptFunction * main_function = main_function;

			/*if(main_function == NULL){
				string::sformat("main_function is not initialized");
				return false;
			}*/
			CScriptFunction * main_function = script_class_factory->getMainFunction();
			*calling_obj = NULL;
			FunctionSymbol *is=NULL;
			StackElement *se=NULL;
			*fun_obj=NULL;

			// 1. some variable in main function ...
			if(access_var.size()>1){
				for(unsigned i=0; i < access_var.size()-1; i++){

					std::string symbol_to_find=access_var[i];
					if(i==0){ // get variable through main_class.main_function (global element)
						//symbol_to_find= CEval::makeSymbolRef(symbol_to_find,IDX_GLOBAL_SCOPE);
						for(unsigned j = 0; j < main_function->local_variable.size() && *calling_obj==NULL; j++){
							if(main_function->local_variable[j].symbol->name==symbol_to_find
							&& main_function->local_variable[j].symbol->idxScope == IDX_GLOBAL_SCOPE){
								StackElement *stk = virtual_machine->getStackElement(j); // main_function->object_info.local_symbols.variable[j].
								if(stk!=NULL){
									if(stk->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
										*calling_obj=(CScriptVariable *)stk->varRef;
									}
								}
								else{
									string::sformat("cannot access i (%i)",j);
									return false;
								}
							}
						}

						if((*calling_obj) == NULL){
							string::sformat("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
							return false;
						}

					}else{ // we have got the calling_obj from last iteration ...
						se = (*calling_obj)->getVariableSymbol(symbol_to_find);

						if(se!=NULL){

							if(se->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
								*calling_obj=(CScriptVariable *)se->varRef;
							}else{
								string::sformat("error evaluating \"%s\". Variable name \"%s\" not script variable",function_access.c_str(),symbol_to_find.c_str());
								return false;
							}
						}
						else{
							string::sformat("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
							return false;
						}
					}
				}

				is=(*calling_obj)->getFunctionSymbol(access_var[access_var.size()-1]);
				if(is!=NULL){
					if(is->object.properties & STK_PROPERTY_TYPE_FUNCTION){
						*fun_obj=(CScriptFunction *)is->object.stkValue;
					}
				}else{

					string::sformat("error evaluating \"%s\". Cannot find function \"%s\"",function_access.c_str(),access_var[access_var.size()-1].c_str());
					return false;
				}

			}else{ // some function in main function
				//*calling_obj = m_mainObject;
				std::string symbol_to_find=access_var[0];
				for(unsigned i = 0; i < main_function->local_function.size() && *fun_obj==NULL; i++){
					CScriptFunction *aux_fun_obj=main_function->local_function[i];
					if(		aux_fun_obj->symbol_info.symbol->name  == symbol_to_find
					  && aux_fun_obj->symbol_info.symbol->idxScope == IDX_GLOBAL_SCOPE){
						*fun_obj=aux_fun_obj;
					}
				}
			}

			if(*fun_obj==NULL){
				THROW_RUNTIME_ERROR(string::sformat("error evaluating \"%s\". Variable name \"%s\" is not function type",function_access.c_str(),access_var[access_var.size()-1].c_str()));
				return false;
			}

			return true;
		}


		template <  typename F>
		std::function<F> * CZetScript::bind_function(const std::string & function_access)
		{
			std::string return_type;
			std::vector<std::string> params;
			std::vector<std::string> arg;
			int idx_return_type=-1;
			void *ptr;
			CScriptFunction * fun=NULL;
			CScriptVariable *calling_obj=NULL;

			// get function symbol ref from global scope ...


			if(getScriptObject(function_access,&calling_obj,&fun))
			{

				// 1. check all parameters ok.
				using Traits3 = function_traits<F>;//decltype(function_type)>;
				get_params_function<Traits3>(0,return_type, arg, make_index_sequence<Traits3::arity>{});

				// 2. check valid parameters ...
				if((idx_return_type=script_class_factory->getIdxClassFromIts_C_Type(return_type)) == -1){
					THROW_RUNTIME_ERROR(string::sformat("Return type \"%s\" for bind function not registered",rtti::demangle(return_type).c_str()));
					return NULL;
				}

				for(unsigned int i = 0; i < arg.size(); i++){
					if(script_class_factory->getIdxClassFromIts_C_Type(arg[i])==-1){
						THROW_RUNTIME_ERROR(string::sformat("Argument (%i) type \"%s\" for bind function not registered",i,rtti::demangle(arg[i]).c_str()));
						return NULL;
					}
				}

				// 3. build custom function in function of parameter number ...
				bindScriptFunctionBuilderBase<Traits3>(&ptr,calling_obj,fun,make_index_sequence<Traits3::arity>{});
				return (std::function<F> *)ptr;

			}

			return NULL;
		}

		template<typename T>
		std::vector<T> CZetScript::vscript2vector(CVectorScriptVariable *v_in){
			std::vector<T> v_out;
			const char * dst_convert_type = typeid(T).name();
			float aux_flt;
			std::vector<StackElement> * variable = v_in->getVectorVariable();

			if(v_in){
				for(unsigned i = 0; i < variable->size(); i++){

					StackElement sv=variable->at(i);

					switch(sv.properties & MASK_VAR_PRIMITIVE_TYPES)
					{
						default:
						case STK_PROPERTY_TYPE_UNDEFINED:
						case STK_PROPERTY_TYPE_NULL:
						case STK_PROPERTY_TYPE_NUMBER:
							if(STRCMP(dst_convert_type, ==,typeid(float).name())){
								memcpy(&aux_flt, &sv.stkValue, sizeof(float));
								v_out.push_back(aux_flt);
							}else if(STRCMP(dst_convert_type, ==,typeid(int).name())){
								v_out.push_back((intptr_t)sv.stkValue);
							}else{
								THROW_RUNTIME_ERROR("Error trying to cast element on std::vector<float>");
								return v_out;
							}
							break;
						case STK_PROPERTY_TYPE_BOOLEAN:
						case STK_PROPERTY_TYPE_STRING:
						case STK_PROPERTY_TYPE_FUNCTION:
							THROW_RUNTIME_ERROR("type not implemented yet");
							return v_out;
							break;
						case STK_PROPERTY_TYPE_INTEGER:
							if(STRCMP(dst_convert_type, ==,typeid(int).name()) || STRCMP(dst_convert_type, ==,typeid(float).name())){// typeid(int).name()) || ){
								v_out.push_back((intptr_t)sv.stkValue);
							}else{
								THROW_RUNTIME_ERROR("Error trying to cast element on std::vector<int>");
								return v_out;
							}
							break;
					}
				}
			}
			return v_out;
		}

		template<typename T>
		CVectorScriptVariable * CZetScript::vector_2_vscript(const std::vector<T> & v){
			CVectorScriptVariable *vsv = new CVectorScriptVariable(this);

			for ( unsigned i = 0; i < v.size(); i++){
				StackElement *stk = vsv->push();
				//intptr_t uvar = (intptr_t)(v[i]);
				*stk = var_2_stk((intptr_t)(v[i]),script_class_factory->getIdxClassFromIts_C_Type(typeid(T).name()));
			}

			return vsv;
		}

}
