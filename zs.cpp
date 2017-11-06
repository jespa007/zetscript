/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

using namespace zetscript;


/**
 * Register C Member function Class
 */
template <  typename _F>
std::function<_F> * bind_staticFunctionMemberInt()
{
	string return_type;
	vector<string> params;
	//CScriptFunctionObject *irs=NULL;
	vector<string> m_arg;
	int idx_return_type=-1;
	//_F function_type;
	//unsigned int ref_ptr=-1;
	//string str_classPtr = typeid( _T *).name();

	/*int idxRegisterdClass = getIdx_C_RegisteredClass(str_classPtr);

	if(idxRegisterdClass == -1){
		return false;
	}*/

	// 1. check all parameters ok.
	using Traits3 = function_traits<_F>;//decltype(function_type)>;
	getParamsFunction<Traits3>(0,return_type, m_arg, make_index_sequence<Traits3::arity>{});


	// check valid parameters ...
	if((idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
		zs_print_error_cr("Return type \"%s\" for bind function not registered",demangle(return_type).c_str());
		return NULL;
	}

	for(unsigned int i = 0; i < m_arg.size(); i++){
		if(getIdxClassFromIts_C_Type(m_arg[i])==-1){
			zs_print_error_cr("Argument (%i) type \"%s\" for bind function not registered",i,demangle(m_arg[i]).c_str());
			return NULL;
		}

	}

	return true;

}

int main(int argc, char * argv[]) {

	auto f=bind_staticFunctionMemberInt<void ()>();

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
