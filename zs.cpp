/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

using namespace zetscript;


// template for last parameter argIdx == 1
template<typename _T,  typename... ArgTypes>
auto bind_script_function_builder( )
	-> typename std::enable_if<sizeof...(ArgTypes) == 1>::type
{
	//return NULL;
	using tReturn = typename _T::return_type;
	using tParam1 = typename _T::template argument<0>::type;

	auto f=new std::function(tReturn <tParam1>[](tParam1 p){}

}

// template when parameters argIdx > 1
template <typename _T, typename... ArgTypes>
auto bind_script_function_builder()
	-> typename std::enable_if<(sizeof...(ArgTypes) == 2)>::type
{
	using tReturn = typename _T::return_type;
	using tParam1 = typename _T::template argument<0>::type;
	using tParam2 = typename _T::template argument<1>::type;


	//return NULL;
}


// trivial case when parameters (argIdx == 0).
template <typename _T>
auto bind_script_function_builder()

{
	//return NULL;
	 // NO ARGS CASE
}
/*

// declare without params ()
template <   typename...ArgTypes, typename std::enable_if<sizeof...(ArgTypes) == N, int>::type = 0>
void bind_script_function_builder()

{

}

// declare with 1 param ()
template <    typename...ArgTypes>
void bind_script_function_builder()
{

}

// declare with 2 param ()
template <  typename...ArgTypes, typename std::enable_if<sizeof...(ArgTypes) == N, int>::type = 2>
void bind_script_function_builder()
{

	printf("hello2!!");

}
*/




/**
 * Register C Member function Class
 */

template <  typename _F>
std::function<_F> * bind_script_function()
{
	string return_type;
	vector<string> params;
	//CScriptFunctionObject *irs=NULL;
	vector<string> m_arg;
	int idx_return_type=-1;
	//_F function_type;
	//unsigned int ref_ptr=-1;
	//string str_classPtr = typeid( _T *).name();

	// 1. check all parameters ok.
	using Traits3 = function_traits<_F>;//decltype(function_type)>;

	// type param1...
	//using param1 = typename Traits3::template argument<0>::type;

	// unpack return and parameters in a type built at runtime...
	getParamsFunction<Traits3>(0,return_type, m_arg, make_index_sequence<Traits3::arity>{});

	// check valid parameters ...
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


	bind_script_function_builder<Traits3,make_index_sequence<Traits3::arity>>();




	/*if(m_arg.size()>0){
		printf("p:%s",typeid(param1).name());
		//Traits3::type[0];
	}*/

	return NULL;

}

int main(int argc, char * argv[]) {



	CZetScript *zetscript = CZetScript::getInstance();

	bind_script_function<void (int)>();

	return 0;


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
