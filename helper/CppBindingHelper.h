/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */


namespace zetscript{





// Helpers...




	inline tStackElement var2stk(intptr_t var_trans, int idx_type){
		//intptr_t var_trans = (intptr_t)input_var;
			string s_return_value;
			tStackElement callc_result={STK_PROPERTY_TYPE_UNDEFINED,0,0};
			//int idx_type=CScriptClass::getIdxClassFromIts_C_Type(typeid(_T).name());
			// save return type ...
			switch(idx_type){
			 case IDX_CLASS_VOID_C:
				break;
			 case IDX_CLASS_INT_PTR_C:
				 if(var_trans==0) return callc_result;
				 callc_result={STK_PROPERTY_TYPE_INTEGER,(void *)(*((intptr_t *)var_trans)),NULL};
				 break;
			 case IDX_CLASS_UNSIGNED_INT_C:
			 case IDX_CLASS_INTPTR_T_C:
			 case IDX_CLASS_INT_C:
				 callc_result={STK_PROPERTY_TYPE_INTEGER,(void *)(((intptr_t)var_trans)),NULL};
				 break;
			 case IDX_CLASS_FLOAT_C:
				 callc_result.properties=STK_PROPERTY_TYPE_NUMBER;//{};
				 memcpy(&callc_result.stkValue,&var_trans,sizeof(float));

				 break;
			 case IDX_CLASS_FLOAT_PTR_C:
				 if(var_trans==0) return callc_result;
				 callc_result.properties=STK_PROPERTY_TYPE_NUMBER;//{};
				 memcpy(&callc_result.stkValue,&(*(float *)var_trans),sizeof(float));
				 break;
			 case IDX_CLASS_BOOL_PTR_C:
				 if(var_trans==0) return callc_result;
				 callc_result={STK_PROPERTY_TYPE_BOOLEAN,(void *)(*((bool *)var_trans)),NULL};
				 break;
			 case IDX_CLASS_BOOL_C:
				 callc_result={STK_PROPERTY_TYPE_BOOLEAN,(void *)(((bool)var_trans)),NULL};
				 break;
			 case IDX_CLASS_CONST_CHAR_PTR_C:
				 if(var_trans==0) return callc_result;
				 callc_result={STK_PROPERTY_TYPE_STRING,(void *)var_trans,NULL};//new string(*((string *)result))};
				 break;
			 case IDX_CLASS_STRING_PTR_C:
				 if(var_trans==0) return callc_result;
				 callc_result={STK_PROPERTY_TYPE_STRING,(void *)((string *)var_trans)->c_str(),NULL};//new string(*((string *)result))};
				 break;
			 case IDX_STACK_ELEMENT:
				 if(var_trans==0) return callc_result;
				 callc_result=*((tStackElement *)var_trans);//{STK_PROPERTY_TYPE_STRING,(void *)((string *)var_trans)->c_str(),NULL};//new string(*((string *)result))};
				 break;
			 default:
				 if(var_trans==0) return callc_result;
				 callc_result = {STK_PROPERTY_TYPE_SCRIPTVAR,NULL,CScriptClass::instanceScriptVariableByIdx(idx_type,(void *)var_trans)};
				 break;
			}

			return callc_result;
	}

	inline bool stk2var(tStackElement *stk_src, int idx_dst_type, intptr_t *result, string & error){
		intptr_t val_ret=0;

		CScriptVariable *script_variable=NULL;


		// save return type ...
		if(stk_src->properties & STK_PROPERTY_IS_STACKVAR){
			stk_src=((tStackElement *)stk_src->varRef);
		}

		if(idx_dst_type == IDX_STACK_ELEMENT){//*CScriptClass::STACK_ELEMENT_PTR)){// && (stk_src->properties & STK_PROPERTY_IS_STACKVAR)){ // set directly stackvar
			val_ret=(intptr_t)stk_src;
		}else{

			switch(GET_INS_PROPERTY_VAR_TYPE(stk_src->properties)){
			case STK_PROPERTY_TYPE_BOOLEAN:
				if(idx_dst_type == IDX_CLASS_BOOL_C){// *CScriptClass::BOOL_TYPE_STR){
					val_ret=(intptr_t)(stk_src->stkValue);
				}else if(idx_dst_type == IDX_CLASS_BOOL_PTR_C){//*CScriptClass::BOOL_PTR_TYPE_STR){
					val_ret=(intptr_t)(&stk_src->stkValue);
				}else{
					error="cannot convert "+demangle((*CScriptClass::STRING_PTR_TYPE_STR))+" into %s"+demangle(GET_IDX_2_CLASS_C_STR(idx_dst_type)).c_str();

					return false;
				}

				break;
			case STK_PROPERTY_TYPE_NUMBER:
				switch(idx_dst_type){
				case IDX_CLASS_FLOAT_C:
					memcpy(&val_ret,&stk_src->stkValue,sizeof(float));
					break;
				case IDX_CLASS_FLOAT_PTR_C:
					val_ret=(intptr_t)(&stk_src->stkValue);
					break;
				case IDX_CLASS_UNSIGNED_INT_C:
				case IDX_CLASS_INTPTR_T_C:
				case IDX_CLASS_INT_C:
					{
						int *aux_dst = ((int *)&val_ret);
						float *aux_src=(float *)&stk_src->stkValue;
						*aux_dst=(int)(*aux_src);
					}
					break;
				default:
					error="cannot convert "+demangle((*CScriptClass::STRING_PTR_TYPE_STR))+" into %s"+demangle(GET_IDX_2_CLASS_C_STR(idx_dst_type));
					return false;
				}
				break;
			case STK_PROPERTY_TYPE_INTEGER:
				switch(idx_dst_type){
				case IDX_CLASS_UNSIGNED_INT_C:
				case IDX_CLASS_INTPTR_T_C:
				case IDX_CLASS_INT_C:
					val_ret=(intptr_t)(stk_src->stkValue);
					break;
				case IDX_CLASS_INT_PTR_C:
					val_ret=(intptr_t)(&stk_src->stkValue);
					break;
				case IDX_CLASS_FLOAT_C:
					{
						float *aux_dst = ((float *)&val_ret);
						int *aux_src=(int *)&stk_src->stkValue;
						*aux_dst = (float)(*aux_src);
					}
					break;
				default:
					error= "cannot convert "+demangle((*CScriptClass::STRING_PTR_TYPE_STR))+" into "+demangle(GET_IDX_2_CLASS_C_STR(idx_dst_type));
					return false;
				}
				break;

			case STK_PROPERTY_TYPE_STRING:
				if(idx_dst_type == IDX_CLASS_STRING_PTR_C){
					if(stk_src->varRef != 0){
						val_ret=(intptr_t)(&((CStringScriptVariable *)(stk_src->varRef))->m_strValue);
					}
					else{ // pass param string ...
						error= "(string *)Expected varRef not NULL";
						return false;
					}

				}else if (idx_dst_type == IDX_CLASS_CONST_CHAR_PTR_C){
					val_ret=(intptr_t)(stk_src->stkValue);
				}else{
					error= "cannot convert "+demangle((*CScriptClass::STRING_PTR_TYPE_STR))+" into "+demangle(GET_IDX_2_CLASS_C_STR(idx_dst_type));
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

				if(script_variable->idxClass==IDX_CLASS_STRING){

					val_ret=(intptr_t)(&script_variable->m_strValue);
					if(idx_dst_type == IDX_CLASS_CONST_CHAR_PTR_C){
						val_ret=(intptr_t)script_variable->m_strValue.c_str();
					}
				}else if(

				   (script_variable->idxClass==IDX_CLASS_VECTOR
				|| script_variable->idxClass==IDX_CLASS_STRUCT)){

					if(idx_dst_type==script_variable->idxClass){
						val_ret=(intptr_t)script_variable->get_C_Object();
					}

				}else if((c_class=script_variable->get_C_Class())!=NULL){ // get the pointer directly ...

					if(c_class->idxClass==idx_dst_type){
						val_ret=(intptr_t)script_variable->get_C_Object();
					}
					else if((val_ret=CScriptClass::doCast((intptr_t)script_variable->get_C_Object(),c_class->idxClass,idx_dst_type))==0){//c_class->idxClass==idx_dst_type){
						error = "cannot convert "+demangle(script_variable->getPointer_C_ClassName())+" into "+demangle(GET_IDX_2_CLASS_C_STR(idx_dst_type));
						return false;
					}
				}else{ // CScriptVariable ?
					error = " Error calling function, no C-object parameter! Unexpected script variable ("+demangle(script_variable->getClassName())+")";
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
	template <typename _R,typename _T>
	auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
	->typename std::enable_if<std::is_same<_R,void>::value>::type
	{


		*f=((void *)(new std::function<void ()>(
			[&,calling_obj,fun_obj](){
				bool error=false;
				CURRENT_VM->execute(
									fun_obj,
									calling_obj,
									error);
				if(error){

					THROW_SCRIPT_ERROR();

				}
			}
		)));
	}

	template <typename _R,typename _T>
	auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
	->typename std::enable_if<!std::is_same<_R,void>::value>::type
	{
		int idx_return = CScriptClass::getIdxClassFromIts_C_Type(typeid(_R).name());

		*f=((void *)(new std::function<_R ()>(
			[&,calling_obj,fun_obj,idx_return](){
					string error_str;
					bool error=false;
					_R ret_value;

					tStackElement stk = CURRENT_VM->execute(
							fun_obj,
							calling_obj,
							error);


					if(error){
						THROW_SCRIPT_ERROR();//THROW_RUNTIME_ERROR(string("run-time error"));
					}


					if(!stk2var(&stk, idx_return, (intptr_t *)(&ret_value),error_str)){
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
	template<typename _R,typename _T,  typename... ArgTypes>
	auto bind_script_function_builder(void **f ,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		-> typename std::enable_if<(std::is_same<_R,void>::value) && (sizeof...(ArgTypes) == 1)>::type
	{
		//return NULL;

		using tParam1 = typename _T::template argument<0>::type;
		int idx_param1 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name());

		*f=((void *)(new std::function<void (tParam1)>(
			[&,calling_obj,fun_obj, idx_param1](tParam1 p1){

				bool error=false;
				vector<tStackElement> args={
						 var2stk((intptr_t)p1,idx_param1)

				};

				CURRENT_VM->execute(
							fun_obj,
							calling_obj,
							error,
							args);
				if(error){
					THROW_SCRIPT_ERROR();//(string("run-time error"));
				}
			}
		)));
	}

	template<typename _R,typename _T,  typename... ArgTypes>
	auto bind_script_function_builder(void **f ,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		-> typename std::enable_if<(!std::is_same<_R,void>::value) && (sizeof...(ArgTypes) == 1)>::type
	{
		using tParam1 = typename _T::template argument<0>::type;

		int idx_return = CScriptClass::getIdxClassFromIts_C_Type(typeid(_R).name());
		int idx_param1 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name());


		*f=((void *)(new std::function<_R (tParam1)>(
			[&,calling_obj,fun_obj,idx_return, idx_param1](tParam1 p1){

					_R ret_value;
					string error_str;
					bool error=false;

					vector<tStackElement> args={
							 var2stk((intptr_t)p1,idx_param1)
					};

					tStackElement stk = CURRENT_VM->execute(
												fun_obj,
												calling_obj,
												error,
												args);

					if(error){
						THROW_SCRIPT_ERROR();//(string("run-time error"));
					}

					if(!stk2var(&stk,idx_return, (intptr_t*)(&ret_value),error_str)){
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
	template <typename _R,typename _T, typename... ArgTypes>
	auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		-> typename std::enable_if<(std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type
	{

		using tParam1 = typename _T::template argument<0>::type;
		using tParam2 = typename _T::template argument<1>::type;


		int idx_param1 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name());
		int idx_param2 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name());

		*f=((void *)(new std::function<void (tParam1,tParam2)>(
			[&,calling_obj,fun_obj, idx_param1, idx_param2](tParam1 p1,tParam2 p2){

				bool error=false;

				vector<tStackElement> args={
						 var2stk((intptr_t)p1,idx_param1)
						,var2stk((intptr_t)p2,idx_param2)

				};

				CURRENT_VM->execute(
								fun_obj,
								calling_obj,
								error,
								args);


				if(error){
					THROW_SCRIPT_ERROR();//(string("run-time error"));
				}
			}

		)));
	}

	template <typename _R,typename _T, typename... ArgTypes>
	auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		-> typename std::enable_if<(!std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type
	{
		using tParam1 = typename _T::template argument<0>::type;
		using tParam2 = typename _T::template argument<1>::type;


		int idx_return = CScriptClass::getIdxClassFromIts_C_Type(typeid(_R).name());
		int idx_param1 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name());
		int idx_param2 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name());

		*f=((void *)(new std::function<_R (tParam1,tParam2)>(
			[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2](tParam1 p1,tParam2 p2){

					_R ret_value;
					string error_str;
					bool error=false;

					vector<tStackElement> args={
							 var2stk((intptr_t)p1,idx_param1)
							,var2stk((intptr_t)p2,idx_param2)

					};

					tStackElement stk = CURRENT_VM->execute(
												fun_obj,
												calling_obj,
												error,
												args);

					if(error){
						THROW_SCRIPT_ERROR();//(string("run-time error"));
					}

					if(!stk2var(&stk, idx_return, (intptr_t*)(&ret_value),error_str)){
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
	template <typename _R,typename _T, typename... ArgTypes>
	auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		-> typename std::enable_if<(std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type
	{

		using tParam1 = typename _T::template argument<0>::type;
		using tParam2 = typename _T::template argument<1>::type;
		using tParam3 = typename _T::template argument<2>::type;


		int idx_param1 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name());
		int idx_param2 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name());
		int idx_param3 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam3).name());


		*f=((void *)(new std::function<void (tParam1,tParam2,tParam3)>(
			[&,calling_obj,fun_obj,idx_param1, idx_param2, idx_param3](tParam1 p1,tParam2 p2,tParam3 p3){

				bool error=false;

				vector<tStackElement> args={
						 var2stk((intptr_t)p1,idx_param1)
						,var2stk((intptr_t)p2,idx_param2)
						,var2stk((intptr_t)p3,idx_param3)
				};

				CURRENT_VM->execute(
								fun_obj,
								calling_obj,
								error,
								args);

				if(error){
					THROW_SCRIPT_ERROR();//(string("run-time error"));
				}
			}

		)));
	}

	template <typename _R,typename _T, typename... ArgTypes>
	auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		-> typename std::enable_if<(!std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type
	{
		using tParam1 = typename _T::template argument<0>::type;
		using tParam2 = typename _T::template argument<1>::type;
		using tParam3 = typename _T::template argument<2>::type;

		int idx_return = CScriptClass::getIdxClassFromIts_C_Type(typeid(_R).name());
		int idx_param1 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name());
		int idx_param2 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name());
		int idx_param3 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam3).name());

		*f=((void *)(new std::function<_R (tParam1,tParam2,tParam3)>(
			[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3](tParam1 p1,tParam2 p2,tParam3 p3){
				_R ret_value;
				string error_str;
				bool error=false;

				vector<tStackElement> args={
						 var2stk((intptr_t)p1,idx_param1)
						,var2stk((intptr_t)p2,idx_param2)
						,var2stk((intptr_t)p3,idx_param3)
				};

				tStackElement stk = CURRENT_VM->execute(
											fun_obj,
											calling_obj,
											error,
											args);

				if(error){
					THROW_SCRIPT_ERROR();//(string("run-time error"));
				}

				if(!stk2var(&stk, idx_return, (intptr_t *)(&ret_value),error_str)){
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
	template <typename _R,typename _T, typename... ArgTypes>
	auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		-> typename std::enable_if<(std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type
	{

		using tParam1 = typename _T::template argument<0>::type;
		using tParam2 = typename _T::template argument<1>::type;
		using tParam3 = typename _T::template argument<2>::type;
		using tParam4 = typename _T::template argument<3>::type;

		int idx_param1 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name());
		int idx_param2 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name());
		int idx_param3 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam3).name());
		int idx_param4 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam4).name());


		*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4)>(
			[&,calling_obj,fun_obj, idx_param1, idx_param2, idx_param3, idx_param4](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4){

				bool error=false;

				vector<tStackElement> args={
						 var2stk((intptr_t)p1,idx_param1)
						,var2stk((intptr_t)p2,idx_param2)
						,var2stk((intptr_t)p3,idx_param3)
						,var2stk((intptr_t)p4,idx_param4)

				};

				CURRENT_VM->execute(
								fun_obj,
								calling_obj,
								error,
								args);

				if(error){
					THROW_SCRIPT_ERROR();//(string("run-time error"));
				}
			}

		)));
	}

	template <typename _R,typename _T, typename... ArgTypes>
	auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		-> typename std::enable_if<(!std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type
	{

		using tParam1 = typename _T::template argument<0>::type;
		using tParam2 = typename _T::template argument<1>::type;
		using tParam3 = typename _T::template argument<2>::type;
		using tParam4 = typename _T::template argument<3>::type;

		int idx_return = CScriptClass::getIdxClassFromIts_C_Type(typeid(_R).name());
		int idx_param1 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name());
		int idx_param2 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name());
		int idx_param3 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam3).name());
		int idx_param4 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam4).name());

		*f=((void *)(new std::function<_R (tParam1,tParam2,tParam3,tParam4)>(
			[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4){
					_R ret_value;
					bool error=false;
					string error_str;

					vector<tStackElement> args={
							 var2stk((intptr_t)p1,idx_param1)
							,var2stk((intptr_t)p2,idx_param2)
							,var2stk((intptr_t)p3,idx_param3)
							,var2stk((intptr_t)p4,idx_param4)

					};

					tStackElement stk = CURRENT_VM->execute(
												fun_obj,
												calling_obj,
												error,
												args);

					if(error){
						THROW_SCRIPT_ERROR();//(string("run-time error"));
					}

					if(!stk2var(&stk, idx_return, (intptr_t*)(&ret_value),error_str)){
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
	template <typename _R,typename _T, typename... ArgTypes>
	auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		-> typename std::enable_if<(std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type
	{

		using tParam1 = typename _T::template argument<0>::type;
		using tParam2 = typename _T::template argument<1>::type;
		using tParam3 = typename _T::template argument<2>::type;
		using tParam4 = typename _T::template argument<3>::type;
		using tParam5 = typename _T::template argument<4>::type;


		int idx_param1 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name());
		int idx_param2 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name());
		int idx_param3 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam3).name());
		int idx_param4 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam4).name());
		int idx_param5 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam5).name());


		*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4,tParam5)>(
			[&,calling_obj,fun_obj,idx_param1, idx_param2, idx_param3, idx_param4, idx_param5](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5){

				bool error=false;
				vector<tStackElement> args={
						 var2stk((intptr_t)p1,idx_param1)
						,var2stk((intptr_t)p2,idx_param2)
						,var2stk((intptr_t)p3,idx_param3)
						,var2stk((intptr_t)p4,idx_param4)
						,var2stk((intptr_t)p5,idx_param5)

				};

				CURRENT_VM->execute(
								fun_obj,
								calling_obj,
								error,
								args);
				if(error){
					THROW_SCRIPT_ERROR();//(string("run-time error"));
				}
			}

		)));
	}


	template <typename _R,typename _T, typename... ArgTypes>
	auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		-> typename std::enable_if<(!std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type
	{

		using tParam1 = typename _T::template argument<0>::type;
		using tParam2 = typename _T::template argument<1>::type;
		using tParam3 = typename _T::template argument<2>::type;
		using tParam4 = typename _T::template argument<3>::type;
		using tParam5 = typename _T::template argument<4>::type;

		int idx_return = CScriptClass::getIdxClassFromIts_C_Type(typeid(_R).name());
		int idx_param1 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name());
		int idx_param2 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name());
		int idx_param3 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam3).name());
		int idx_param4 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam4).name());
		int idx_param5 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam5).name());

		*f=((void *)(new std::function<_R (tParam1,tParam2,tParam3,tParam4,tParam5)>(
			[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5){


				_R ret_value;
				bool error=false;
				string error_str;

				vector<tStackElement> args={
						 var2stk((intptr_t)p1,idx_param1)
						,var2stk((intptr_t)p2,idx_param2)
						,var2stk((intptr_t)p3,idx_param3)
						,var2stk((intptr_t)p4,idx_param4)
						,var2stk((intptr_t)p5,idx_param5)

				};

				tStackElement stk = CURRENT_VM->execute(
											fun_obj,
											calling_obj,
											error,
											args);

				if(error){
					THROW_SCRIPT_ERROR();//(string("run-time error"));
				}

				if(!stk2var(&stk, idx_return, (intptr_t*)(&ret_value),error_str)){
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
	template <typename _R,typename _T, typename... ArgTypes>
	auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		-> typename std::enable_if<(std::is_same<_R,void>::value) && (sizeof...(ArgTypes) == 6)>::type
	{

		using tParam1 = typename _T::template argument<0>::type;
		using tParam2 = typename _T::template argument<1>::type;
		using tParam3 = typename _T::template argument<2>::type;
		using tParam4 = typename _T::template argument<3>::type;
		using tParam5 = typename _T::template argument<4>::type;
		using tParam6 = typename _T::template argument<5>::type;


		int idx_param1 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name());
		int idx_param2 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name());
		int idx_param3 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam3).name());
		int idx_param4 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam4).name());
		int idx_param5 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam5).name());
		int idx_param6 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam6).name());




		*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4,tParam5,tParam6)>(
			[&,calling_obj,fun_obj, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5, idx_param6](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5,tParam6 p6){
				bool error=false;
				vector<tStackElement> args={
						 var2stk((intptr_t)p1,idx_param1)
						,var2stk((intptr_t)p2,idx_param2)
						,var2stk((intptr_t)p3,idx_param3)
						,var2stk((intptr_t)p4,idx_param4)
						,var2stk((intptr_t)p5,idx_param5)
						,var2stk((intptr_t)p6,idx_param6)

				};

				CURRENT_VM->execute(
								fun_obj,
								calling_obj,
								error,
								args);
				if(error){

					THROW_SCRIPT_ERROR();//(string("run-time error"));
				}
			}

		)));
	}

	template <typename _R,typename _T, typename... ArgTypes>
	auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunction *fun_obj)
		-> typename std::enable_if<(!std::is_same<_R,void>::value) &&(sizeof...(ArgTypes) == 6)>::type
	{
		using tParam1 = typename _T::template argument<0>::type;
		using tParam2 = typename _T::template argument<1>::type;
		using tParam3 = typename _T::template argument<2>::type;
		using tParam4 = typename _T::template argument<3>::type;
		using tParam5 = typename _T::template argument<4>::type;
		using tParam6 = typename _T::template argument<5>::type;

		int idx_return = CScriptClass::getIdxClassFromIts_C_Type(typeid(_R).name());
		int idx_param1 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam1).name());
		int idx_param2 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam2).name());
		int idx_param3 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam3).name());
		int idx_param4 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam4).name());
		int idx_param5 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam5).name());
		int idx_param6 = CScriptClass::getIdxClassFromIts_C_Type(typeid(tParam6).name());

		*f=((void *)(new std::function<_R (tParam1,tParam2,tParam3,tParam4,tParam5,tParam6)>(
			[&,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5, idx_param6](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5,tParam6 p6){

					_R ret_value;
					bool error=false;
					string error_str;

					vector<tStackElement> args={
							 var2stk((intptr_t)p1,idx_param1)
							,var2stk((intptr_t)p2,idx_param2)
							,var2stk((intptr_t)p3,idx_param3)
							,var2stk((intptr_t)p4,idx_param4)
							,var2stk((intptr_t)p5,idx_param5)
							,var2stk((intptr_t)p6,idx_param6)
					};

					tStackElement stk = CURRENT_VM->execute(
												fun_obj,
												calling_obj,
												error,
												args);

					if(error){
						THROW_SCRIPT_ERROR();//(string("run-time error"));
					}

					if(!stk2var(&stk, idx_return, (intptr_t *)(&ret_value),error_str)){
						THROW_RUNTIME_ERROR("run-time error converting result value:%s",error_str.c_str());
					}
					return ret_value;

			}
		)));
	}


	//
	//
	//--------------------------------------------------------------------------------------------------------------------

	 template <typename _F, std::size_t... Is>
	 auto bind_script_function_builder_base(void **f, CScriptVariable *calling_obj,CScriptFunction *fun_obj,index_sequence<Is...>)
	 -> typename std::enable_if<(_F::arity > 0)>::type
	{
		 bind_script_function_builder<typename _F::return_type, _F,  typename _F::template argument<Is>::type...>(f,calling_obj,fun_obj);
	}

	 template <typename _F, std::size_t... Is>
	 auto bind_script_function_builder_base(void **f, CScriptVariable *calling_obj,CScriptFunction *fun_obj,index_sequence<Is...>)
	 -> typename std::enable_if<(_F::arity == 0)>::type
	{
		 bind_script_function_builder<typename _F::return_type, _F>(f,calling_obj,fun_obj);
	}




	template <  typename _F>
	std::function<_F> * bind_function(const string & function_access)
	{
		string return_type;
		vector<string> params;
		vector<string> m_arg;
		int idx_return_type=-1;
		void *ptr;
		CScriptFunction * fun=NULL;
		CScriptVariable *calling_obj=NULL;

		// get function symbol ref from global scope ...


		if(CZetScript::getInstance()->getScriptObjectFromFunctionAccess(function_access,&calling_obj,&fun))
		{

			// 1. check all parameters ok.
			using Traits3 = function_traits<_F>;//decltype(function_type)>;
			getParamsFunction<Traits3>(0,return_type, m_arg, make_index_sequence<Traits3::arity>{});

			// 2. check valid parameters ...
			if((idx_return_type=CScriptClass::getIdxClassFromIts_C_Type(return_type)) == -1){
				THROW_RUNTIME_ERROR("Return type \"%s\" for bind function not registered",demangle(return_type).c_str());
				return NULL;
			}

			for(unsigned int i = 0; i < m_arg.size(); i++){
				if(CScriptClass::getIdxClassFromIts_C_Type(m_arg[i])==-1){
					THROW_RUNTIME_ERROR("Argument (%i) type \"%s\" for bind function not registered",CZetScriptUtils::intToString(i),demangle(m_arg[i]).c_str());
					return NULL;
				}
			}

			// 3. build custom function in function of parameter number ...
			bind_script_function_builder_base<Traits3>(&ptr,calling_obj,fun,make_index_sequence<Traits3::arity>{});
			return (std::function<_F> *)ptr;

		}

		return NULL;
	}

	template<typename _T>
	vector<_T> vscript2vector(CVectorScriptVariable *v_in){
		vector<_T> v_out;
		const char * dst_convert_type = typeid(_T).name();
		float aux_flt;
		vector<tStackElement> * m_variable = v_in->getVectorVariable();

		if(v_in){
			for(unsigned i = 0; i < m_variable->size(); i++){

				tStackElement sv=m_variable->at(i);

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
							THROW_RUNTIME_ERROR("Error trying to cast element on vector<float>");
							return v_out;
						}
						break;
					case STK_PROPERTY_TYPE_BOOLEAN:
					case STK_PROPERTY_TYPE_STRING:
					case STK_PROPERTY_TYPE_FUNCTION:
						THROW_RUNTIME_ERROR("type not implemented to cast. Sorry.");
						return v_out;
						break;
					case STK_PROPERTY_TYPE_INTEGER:
						if(STRCMP(dst_convert_type, ==,typeid(int).name()) || STRCMP(dst_convert_type, ==,typeid(float).name())){// typeid(int).name()) || ){
							v_out.push_back((intptr_t)sv.stkValue);
						}else{
							THROW_RUNTIME_ERROR("Error trying to cast element on vector<int>");
							return v_out;
						}
						break;
				}
			}
		}
		return v_out;
	}

	template<typename _T>
	CVectorScriptVariable * vector2vscript(const vector<_T> & v){
		CVectorScriptVariable *vsv = new CVectorScriptVariable();

		for ( unsigned i = 0; i < v.size(); i++){
			tStackElement *stk = vsv->push();
			//intptr_t uvar = (intptr_t)(v[i]);
			*stk = var2stk((intptr_t)(v[i]),CScriptClass::getIdxClassFromIts_C_Type(typeid(_T).name()));
		}

		return vsv;
	}


}
