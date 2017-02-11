#include "zg_core.h"

C_VariableFunctionFactory * C_VariableFunctionFactory::m_instance=NULL;

C_VariableFunctionFactory::C_VariableFunctionFactory(){
	/*c_void_function = new map<string,void *>();
	c_int_function = new map<string,void *>();
	class_function_member = new map<string,void *>();*/
}

C_VariableFunctionFactory * C_VariableFunctionFactory::getInstance(){

	if(m_instance== NULL){
		m_instance = new C_VariableFunctionFactory();
	}

	return m_instance;
}

void  C_VariableFunctionFactory::destroySingletons(){
	delete m_instance;
}


C_VariableFunctionFactory::~C_VariableFunctionFactory(){




	for(unsigned i = 0; i < c_void_function.size(); i++){
		delete_proxy_function<void>(c_void_function[i].n_args,c_void_function[i].fun_ptr);//=(int)new_proxy_function<void>(irs.m_arg.size(),function_ptr))==0){//(int)function_ptr;

	}

	for(unsigned i = 0; i < c_int_function.size(); i++){
		delete_proxy_function<int>(c_int_function[i].n_args,c_int_function[i].fun_ptr);
	}

	for(unsigned i = 0; i < class_function_member.size(); i++){
		delete ((std::function<void *(void *,PROXY_CREATOR)> *)	class_function_member[i]);
	}
}
