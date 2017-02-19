#include "zs_factory.h"

vector<tScriptFunctionObject *> 	* vec_script_function_object_node=NULL;

/*
//	 _____           _       _    ______                _   _
//	/  ___|         (_)     | |   |  ___|              | | (_)
//	\ `--.  ___ _ __ _ _ __ | |_  | |_ _   _ _ __   ___| |_ _  ___  _ __
//	 `--. \/ __| '__| | '_ \| __| |  _| | | | '_ \ / __| __| |/ _ \| '_ \
//	/\__/ / (__| |  | | |_) | |_  | | | |_| | | | | (__| |_| | (_) | | | |
//	\____/ \___|_|  |_| .__/ \__| \_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|
//                    | |
//                    |_|
// 	_________________________________________________
//  	__________________________________
//
*/


void CScriptFunctionObjectFactory::createSingletons(){
	if(vec_script_function_object_node==NULL){
		vec_script_function_object_node = new vector<tScriptFunctionObject *>();
	}
}

void CScriptFunctionObjectFactory::destroySingletons(){
	if(vec_script_function_object_node!=NULL){
		delete vec_script_function_object_node;
		vec_script_function_object_node=NULL;
	}
}


void CScriptFunctionObjectFactory::set(vector<tScriptFunctionObject *> 	& set_vec){
	*vec_script_function_object_node = set_vec;
}

vector<tScriptFunctionObject *> 	*CScriptFunctionObjectFactory::getVecScriptFunctionObjectNode(){
	return vec_script_function_object_node;
}


tScriptFunctionObject *		 CScriptFunctionObjectFactory::newFunctionSymbol(){
	tScriptFunctionObject *script_function = new tScriptFunctionObject;
	vec_script_function_object_node->push_back(script_function);
	script_function->object_info.idxFunctionSymbol = vec_script_function_object_node->size()-1;
	return script_function;
}

tScriptFunctionObject 	* CScriptFunctionObjectFactory::getScriptFunctionObject(int idx){
	if(idx < 0 || (unsigned)idx >= vec_script_function_object_node->size()){
		print_error_cr("tScriptFunctionObject node out of bound");
		return NULL;
	}

	return vec_script_function_object_node->at(idx);
}

tFunctionInfo 	* CScriptFunctionObjectFactory::getFunctionInfo(int idx){
	if(idx < 0 || (unsigned)idx >= vec_script_function_object_node->size()){
		print_error_cr("tScriptFunctionObject node out of bound");
		return NULL;
	}

	return &vec_script_function_object_node->at(idx)->object_info;
}
