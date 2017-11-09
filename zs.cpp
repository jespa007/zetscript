/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

using namespace zetscript;

template<typename _T>
tStackElement var2stk(_T var){

	intptr_t var_trans = (intptr_t)var;
	string s_return_value;
	tStackElement callc_result={0};
	int idx_type=CScriptClass::getIdxClassFromIts_C_Type(typeid(_T).name());
	// save return type ...
	switch(idx_type){
	 case IDX_CLASS_VOID_C:
		break;
	 case IDX_CLASS_INT_PTR_C:
		 callc_result={STK_PROPERTY_TYPE_INTEGER,(void *)(*((intptr_t *)var_trans)),NULL};
		 break;
	 case IDX_CLASS_INT_C:
		 callc_result={STK_PROPERTY_TYPE_INTEGER,(void *)(((intptr_t)var_trans)),NULL};
		 break;
	 case IDX_CLASS_FLOAT_C:
		 callc_result.properties=STK_PROPERTY_TYPE_NUMBER;//{};
		 memcpy(&callc_result.stkValue,&var_trans,sizeof(float));

		 break;
	 case IDX_CLASS_FLOAT_PTR_C:
		 callc_result.properties=STK_PROPERTY_TYPE_NUMBER;//{};
		 memcpy(&callc_result.stkValue,&(*(float *)var_trans),sizeof(float));

		 break;

	 case IDX_CLASS_BOOL_PTR_C:
		 callc_result={STK_PROPERTY_TYPE_BOOLEAN,(void *)(*((bool *)var_trans)),NULL};
		 break;
	 case IDX_CLASS_BOOL_C:
		 callc_result={STK_PROPERTY_TYPE_BOOLEAN,(void *)(((bool)var_trans)),NULL};
		 break;
	 case CONST_CONST_CHAR_PTR_TYPE_STR:
		 callc_result={STK_PROPERTY_TYPE_STRING,(void *)var_trans,NULL};//new string(*((string *)result))};
		 break;
	 case IDX_CLASS_STRING_PTR_C:
		 s_return_value = *((string *)var_trans);
		 callc_result={STK_PROPERTY_TYPE_STRING,(void *)s_return_value.c_str(),NULL};//new string(*((string *)result))};
		 break;
	 default:
		 callc_result = {STK_PROPERTY_TYPE_SCRIPTVAR,NULL,CScriptClass::instanceScriptVariableByIdx(idx_type,(void *)var_trans)};
		 break;
	}

	return callc_result;

}

template<typename _T>
void stk2var(tStackElement *stk_ret, int idx_ret, void **result){

	void * val_ret=0;
	string type_return_str = typeid(_T).name();
	CScriptVariable *script_variable=NULL;


	// save return type ...
	if(stk_ret->properties & STK_PROPERTY_IS_STACKVAR){
		stk_ret=((tStackElement *)stk_ret->varRef);
	}

	if((type_return_str == *CScriptClass::STACK_ELEMENT_PTR)){// && (stk_ret->properties & STK_PROPERTY_IS_STACKVAR)){ // set directly stackvar
		val_ret=stk_ret;
	}else{

		switch(GET_INS_PROPERTY_VAR_TYPE(stk_ret->properties)){
		case STK_PROPERTY_TYPE_BOOLEAN:
			if(type_return_str == *CScriptClass::BOOL_TYPE_STR){
				val_ret=(stk_ret->stkValue);
			}else if(type_return_str != *CScriptClass::BOOL_PTR_TYPE_STR){
				val_ret=(&stk_ret->stkValue);
			}else{
				zs_print_error_cr("cannot convert %s into %s",

										demangle((*CScriptClass::STRING_PTR_TYPE_STR)).c_str(),
										demangle(type_return_str).c_str()
										);
				return;
			}

			break;
		case STK_PROPERTY_TYPE_NUMBER:
			if(type_return_str == *CScriptClass::FLOAT_TYPE_STR){
				val_ret=stk_ret->stkValue;
			}else if(type_return_str == *CScriptClass::FLOAT_PTR_TYPE_STR){
				val_ret=(&stk_ret->stkValue);
			}else{
				zs_print_error_cr( " cannot convert %s into %s",
										demangle((*CScriptClass::STRING_PTR_TYPE_STR)).c_str(),
										demangle(type_return_str).c_str()
										);
				return;
			}
			break;
		case STK_PROPERTY_TYPE_INTEGER:
			if(type_return_str == *CScriptClass::INT_TYPE_STR){
				val_ret=(stk_ret->stkValue);
			}else if(type_return_str == *CScriptClass::INT_PTR_TYPE_STR){
				val_ret=(&stk_ret->stkValue);
			}else{
				zs_print_error_cr( "cannot convert %s into %s",
							demangle((*CScriptClass::STRING_PTR_TYPE_STR)).c_str(),
							demangle(type_return_str).c_str()
						);
				return;
			}
			break;

		case STK_PROPERTY_TYPE_STRING:
			if(type_return_str == *CScriptClass::STRING_PTR_TYPE_STR){
				if(stk_ret->varRef != 0){
					val_ret=(&((CStringScriptVariable *)(stk_ret->varRef))->m_strValue);
				}
				else{ // pass param string ...
					zs_print_error_cr("(string *)Expected varRef not NULL");
					return;
				}

			}else if (type_return_str == *CScriptClass::CONST_CHAR_PTR_TYPE_STR){
				val_ret=(stk_ret->stkValue);
			}else{
				zs_print_error_cr(
						demangle((*CScriptClass::STRING_PTR_TYPE_STR)).c_str(),
						demangle(type_return_str).c_str()
						);
				return;
			}


			break;
		default: // script variable by default ...

			script_variable=(CScriptVariable *)stk_ret->varRef;
			CScriptClass *c_class=NULL;

			if(script_variable==NULL){

				zs_print_error_cr( "Variable is not defined");
				return;
			}

			if(script_variable->idxScriptClass==IDX_CLASS_STRING){
				val_ret=(&script_variable->m_strValue);
			}else if(

			   (script_variable->idxScriptClass==IDX_CLASS_VECTOR
			|| script_variable->idxScriptClass==IDX_CLASS_STRUCT)){

				if(type_return_str==script_variable->getPointer_C_ClassName()){
					val_ret=script_variable->get_C_Object();
				}

			}else if((c_class=script_variable->get_C_Class())!=NULL){ // get the pointer directly ...
				fntConversionType paramConv=0;
				if(c_class->classPtrType==type_return_str){
					val_ret=script_variable->get_C_Object();
				}else if((paramConv=CScriptClass::getConversionType(c_class->classPtrType,type_return_str))!=0){
					val_ret=(void *)paramConv(script_variable);
				}else { // try get C object ..

					zs_print_error_cr("cannot convert %s into %s",
								script_variable->getPointer_C_ClassName().c_str(),
								type_return_str.c_str()
						);
					return;
				}
			}else{ // CScriptVariable ?
				zs_print_error_cr(" Error calling function, no C-object parameter! Unexpected script variable (%s)!"

						,script_variable->getClassName().c_str());
				return;
			}
			break;
		}
	}

	*result = val_ret;


}

// we implement as many interface C++ functions as max parameters we can manage (6)

// trivial case when parameters (argIdx == 0).
template <typename _T>
auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunctionObject *fun_obj)
{

	using tReturn = typename _T::return_type;
	int idx_return_type=CScriptClass::getIdxClassFromIts_C_Type(typeid(tReturn).name());
	 // NO ARGS CASE
	if(idx_return_type == IDX_CLASS_VOID_C){
		*f=((void *)(new std::function<void ()>(
			[&,calling_obj,fun_obj,idx_return_type](){

				CURRENT_VM->execute(
								fun_obj,
								calling_obj);

			}
		)));
	}
	else{ // return type...

		*f=((void *)(new std::function<tReturn ()>(
			[&,calling_obj,fun_obj,idx_return_type](){

					tReturn ret_value;

					tStackElement *stk = CURRENT_VM->execute(
							fun_obj,
							calling_obj);

					stk2var<tReturn>(stk,idx_return_type, (void **)(&ret_value));
					return ret_value;
			}
		)));
	}


}

// template for last parameter argIdx == 1
template<typename _T,  typename... ArgTypes>
auto bind_script_function_builder(void **f ,CScriptVariable *calling_obj,CScriptFunctionObject *fun_obj)
	-> typename std::enable_if<sizeof...(ArgTypes) == 1>::type
{
	//return NULL;
	using tReturn = typename _T::return_type;
	using tParam1 = typename _T::template argument<0>::type;
	int idx_return_type=CScriptClass::getIdxClassFromIts_C_Type(typeid(tReturn).name());

	if(idx_return_type == IDX_CLASS_VOID_C){

		*f=((void *)(new std::function<void (tParam1)>(
			[&,calling_obj,fun_obj,idx_return_type](tParam1 p1){

				vector<tStackElement> args={
						 var2stk<tParam1>(p1)
				};


				CURRENT_VM->execute(
							fun_obj,
							calling_obj,
							args);
			}

		)));
	}
	else{
		*f=((void *)(new std::function<tReturn (tParam1)>(
			[&,calling_obj,fun_obj,idx_return_type](tParam1 p1){
					vector<tStackElement> args={
							 var2stk<tParam1>(p1)
					};

					tReturn ret_value;

					tStackElement *stk = CURRENT_VM->execute(
												fun_obj,
												calling_obj,
												args);

					stk2var<tReturn>(stk,idx_return_type, (void **)(&ret_value));
										return ret_value;

			}
		)));
	}

}

// template when parameters argIdx == 2
template <typename _T, typename... ArgTypes>
auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunctionObject *fun_obj)
	-> typename std::enable_if<(sizeof...(ArgTypes) == 2)>::type
{
	using tReturn = typename _T::return_type;
	using tParam1 = typename _T::template argument<0>::type;
	using tParam2 = typename _T::template argument<1>::type;

	int idx_return_type=CScriptClass::getIdxClassFromIts_C_Type(typeid(tReturn).name());

	if(idx_return_type == IDX_CLASS_VOID_C){

		*f=((void *)(new std::function<void (tParam1,tParam2)>(
			[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2){

				vector<tStackElement> args={
						 var2stk<tParam1>(p1)
						,var2stk<tParam2>(p2)
				};


				CURRENT_VM->execute(
								fun_obj,
								calling_obj,
								args);
			}

		)));
	}
	else{
		*f=((void *)(new std::function<tReturn (tParam1,tParam2)>(
			[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2){
					vector<tStackElement> args={
							 var2stk<tParam1>(p1)
							,var2stk<tParam2>(p2)
					};


					tStackElement *stk = CURRENT_VM->execute(
												fun_obj,
												calling_obj,
												args);

					return (tReturn)(stk2var<tReturn>(stk,idx_return_type));

			}
		)));
	}


	//return NULL;
}

// template when parameters argIdx == 3
template <typename _T, typename... ArgTypes>
auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunctionObject *fun_obj)
	-> typename std::enable_if<(sizeof...(ArgTypes) == 3)>::type
{
	using tReturn = typename _T::return_type;
	using tParam1 = typename _T::template argument<0>::type;
	using tParam2 = typename _T::template argument<1>::type;
	using tParam3 = typename _T::template argument<2>::type;

	int idx_return_type=CScriptClass::getIdxClassFromIts_C_Type(typeid(tReturn).name());


	if(idx_return_type == IDX_CLASS_VOID_C){

		*f=((void *)(new std::function<void (tParam1,tParam2,tParam3)>(
			[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2,tParam3 p3){

				vector<tStackElement> args={
						 var2stk<tParam1>(p1)
						,var2stk<tParam2>(p2)
						,var2stk<tParam3>(p3)
				};


				CURRENT_VM->execute(
									fun_obj,
									calling_obj,
									args);
			}

		)));
	}
	else{
		*f=((void *)(new std::function<tReturn (tParam1,tParam2,tParam3)>(
			[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2,tParam3 p3){
					vector<tStackElement> args={
							 var2stk<tParam1>(p1)
							,var2stk<tParam2>(p2)
							,var2stk<tParam3>(p3)
					};


					tStackElement *stk = CURRENT_VM->execute(
												fun_obj,
												calling_obj,
												args);

					return (tReturn)(stk2var<tReturn>(stk,idx_return_type));

			}
		)));
	}
}

// template when parameters argIdx == 4
template <typename _T, typename... ArgTypes>
auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunctionObject *fun_obj)
	-> typename std::enable_if<(sizeof...(ArgTypes) == 4)>::type
{
	using tReturn = typename _T::return_type;
	using tParam1 = typename _T::template argument<0>::type;
	using tParam2 = typename _T::template argument<1>::type;
	using tParam3 = typename _T::template argument<2>::type;
	using tParam4 = typename _T::template argument<3>::type;

	int idx_return_type=CScriptClass::getIdxClassFromIts_C_Type(typeid(tReturn).name());

	if(idx_return_type == IDX_CLASS_VOID_C){

		*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4)>(
			[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4){

				vector<tStackElement> args={
						 var2stk<tParam1>(p1)
						,var2stk<tParam2>(p2)
						,var2stk<tParam3>(p3)
						,var2stk<tParam4>(p4)
				};


				CURRENT_VM->execute(
									fun_obj,
									calling_obj,
									args);
			}

		)));
	}
	else{
		*f=((void *)(new std::function<tReturn (tParam1,tParam2,tParam3,tParam4)>(
			[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4){
					vector<tStackElement> args={
							 var2stk<tParam1>(p1)
							,var2stk<tParam2>(p2)
							,var2stk<tParam3>(p3)
							,var2stk<tParam4>(p4)
					};


					tStackElement *stk = CURRENT_VM->execute(
												fun_obj,
												calling_obj,
												args);

					return (tReturn)(stk2var<tReturn>(stk,idx_return_type));

			}
		)));
	}
}


// template when parameters argIdx == 5
template <typename _T, typename... ArgTypes>
auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunctionObject *fun_obj)
	-> typename std::enable_if<(sizeof...(ArgTypes) == 5)>::type
{
	using tReturn = typename _T::return_type;
	using tParam1 = typename _T::template argument<0>::type;
	using tParam2 = typename _T::template argument<1>::type;
	using tParam3 = typename _T::template argument<2>::type;
	using tParam4 = typename _T::template argument<3>::type;
	using tParam5 = typename _T::template argument<4>::type;

	int idx_return_type=CScriptClass::getIdxClassFromIts_C_Type(typeid(tReturn).name());

	if(idx_return_type == IDX_CLASS_VOID_C){

		*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4,tParam5)>(
			[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5){

				vector<tStackElement> args={
						 var2stk<tParam1>(p1)
						,var2stk<tParam2>(p2)
						,var2stk<tParam3>(p3)
						,var2stk<tParam4>(p4)
						,var2stk<tParam5>(p5)

				};


				CURRENT_VM->execute(
								fun_obj,
								calling_obj,
								args);
			}

		)));
	}
	else{
		*f=((void *)(new std::function<tReturn (tParam1,tParam2,tParam3,tParam4,tParam5)>(
			[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5){
					vector<tStackElement> args={
							 var2stk<tParam1>(p1)
							,var2stk<tParam2>(p2)
							,var2stk<tParam3>(p3)
							,var2stk<tParam4>(p4)
							,var2stk<tParam5>(p5)

					};


					tStackElement *stk = CURRENT_VM->execute(
												fun_obj,
												calling_obj,
												args);

					return (tReturn)(stk2var<tReturn>(stk,idx_return_type));

			}
		)));
	}
	//return NULL;
}

// template when parameters argIdx == 6
template <typename _T, typename... ArgTypes>
auto bind_script_function_builder(void **f,CScriptVariable *calling_obj,CScriptFunctionObject *fun_obj)
	-> typename std::enable_if<(sizeof...(ArgTypes) == 6)>::type
{
	using tReturn = typename _T::return_type;
	using tParam1 = typename _T::template argument<0>::type;
	using tParam2 = typename _T::template argument<1>::type;
	using tParam3 = typename _T::template argument<2>::type;
	using tParam4 = typename _T::template argument<3>::type;
	using tParam5 = typename _T::template argument<4>::type;
	using tParam6 = typename _T::template argument<5>::type;

	int idx_return_type=CScriptClass::getIdxClassFromIts_C_Type(typeid(tReturn).name());

	if(idx_return_type == IDX_CLASS_VOID_C){

		*f=((void *)(new std::function<void (tParam1,tParam2,tParam3,tParam4,tParam5,tParam6)>(
			[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5,tParam6 p6){

				vector<tStackElement> args={
						 var2stk<tParam1>(p1)
						,var2stk<tParam2>(p2)
						,var2stk<tParam3>(p3)
						,var2stk<tParam4>(p4)
						,var2stk<tParam5>(p5)
						,var2stk<tParam6>(p6)
				};


				CURRENT_VM->execute(
								fun_obj,
								calling_obj,
								args);
			}

		)));
	}
	else{
		*f=((void *)(new std::function<tReturn (tParam1,tParam2,tParam3,tParam4,tParam5,tParam6)>(
			[&,calling_obj,fun_obj,idx_return_type](tParam1 p1,tParam2 p2,tParam3 p3,tParam4 p4,tParam5 p5,tParam6 p6){
					vector<tStackElement> args={
							 var2stk<tParam1>(p1)
							,var2stk<tParam2>(p2)
							,var2stk<tParam3>(p3)
							,var2stk<tParam4>(p4)
							,var2stk<tParam5>(p5)
							,var2stk<tParam6>(p6)
					};


					tStackElement *stk = CURRENT_VM->execute(
												fun_obj,
												calling_obj,
												args);

					return (tReturn)(stk2var<tReturn>(stk,idx_return_type));

			}
		)));
	}
	//return NULL;
}


 template <typename _T, std::size_t... Is>
void bind_script_function_builder_base(void **f, CScriptVariable *calling_obj,CScriptFunctionObject *fun_obj,index_sequence<Is...>)
{

		//returnType = typeid(typename F::return_type).name();
		//getArgTypes<F::arity, F, typename F::template argument<Is>::type...>(typeParams);
	 bind_script_function_builder<_T, typename _T::template argument<Is>::type...>(f,calling_obj,fun_obj);
}

			// 3. build custom function in function of parameter number ...
			//bind_script_function_builder_base<Traits3>(&ptr,make_index_sequence<Traits3::arity>{});

/**
 * Main bind function
 */

template <  typename _F>
std::function<_F> * bind_function(const string & function_access)
{
	string return_type;
	vector<string> params;
	vector<string> m_arg;
	int idx_return_type=-1;
	void *ptr;
	CScriptFunctionObject * fun=NULL;
	CScriptVariable *calling_obj=NULL;

	if(CZetScript::getInstance()->getScriptObjectFromFunctionAccess(function_access,&calling_obj,&fun))
	{

		// 1. check all parameters ok.
		using Traits3 = function_traits<_F>;//decltype(function_type)>;
		getParamsFunction<Traits3>(0,return_type, m_arg, make_index_sequence<Traits3::arity>{});

		// 2. check valid parameters ...
		if((idx_return_type=CScriptClass::getIdxClassFromIts_C_Type(return_type)) == -1){
			zs_print_error_cr("Return type \"%s\" for bind function not registered",demangle(return_type).c_str());
			return NULL;
		}

		for(unsigned int i = 0; i < m_arg.size(); i++){
			if(CScriptClass::getIdxClassFromIts_C_Type(m_arg[i])==-1){
				zs_print_error_cr("Argument (%i) type \"%s\" for bind function not registered",i,demangle(m_arg[i]).c_str());
				return NULL;
			}

		}

		// 3. build custom function in function of parameter number ...
		bind_script_function_builder_base<Traits3>(&ptr,calling_obj,fun,make_index_sequence<Traits3::arity>{});
		return (std::function<_F> *)ptr;

	}

	return NULL;

}

void test(){

	CZetScript *zetscript = CZetScript::getInstance();
	if(!zetscript->eval(
		"function fun(arg1)\n"
		"{\n"
			"print(\"hello!!!\"+arg1);"
			"return 1.5;"
		"}"
	)){
			fprintf(stderr,"%s\n",ZS_GET_ERROR_MSG());
	}

	auto fun = bind_function<float (int)>("fun");

	float f= (*fun)(100);

	printf("return %f\n",f);

}

int main(int argc, char * argv[]) {


	test();

	return 0;

	CZetScript *zetscript = CZetScript::getInstance();



	if (argc < 2) {
		printf("Put file to evaluate.\n");
		printf("\n");
		printf("Example:\n");
		printf("\n");
		printf("file.zs");
		printf("\n");
		printf("\n");
		return 0;
	}


	if(!zetscript->eval_file(argv[1])){
		fprintf(stderr,"%s\n",ZS_GET_ERROR_MSG());
	}

	CZetScript::destroy();

#if defined(__DEBUG__) && defined(__MEMMANAGER__)
	MEM_ViewStatus();
#endif

	return 0;
}
