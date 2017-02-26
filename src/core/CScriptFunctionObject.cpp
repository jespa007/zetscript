#include "zs_core.h"

vector<CScriptFunctionObject *> 	* vec_script_function_object_node=NULL;

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





void CScriptFunctionObject::destroySingletons(){
	if(vec_script_function_object_node!=NULL){
		delete vec_script_function_object_node;
		vec_script_function_object_node=NULL;
	}
}


void CScriptFunctionObject::set(vector<CScriptFunctionObject *> 	& set_vec){
	*vec_script_function_object_node = set_vec;
}

vector<CScriptFunctionObject *> 	*CScriptFunctionObject::getVecScriptFunctionObjectNode(){
	return vec_script_function_object_node;
}


CScriptFunctionObject *		 CScriptFunctionObject::newScriptFunctionObject(){
	CScriptFunctionObject *script_function = new CScriptFunctionObject;
	vec_script_function_object_node->push_back(script_function);
	script_function->object_info.idxScriptFunctionObject = vec_script_function_object_node->size()-1;
	return script_function;
}


tInfoVariableSymbol * CScriptFunctionObject::newVariableSymbol(int idxFunction){

	if(idxFunction != -1 && idxFunction<(int)vec_script_function_object_node->size()){

		tInfoVariableSymbol irs;
		CScriptFunctionObject *irc = vec_script_function_object_node->at(idxFunction);

		irs.index = irc->object_info.local_symbols.m_registeredVariable.size();
		irc->object_info.local_symbols.m_registeredVariable.push_back(irs);
		return &irc->object_info.local_symbols.m_registeredVariable[irc->object_info.local_symbols.m_registeredVariable.size()-1];
	}
	else{
		print_error("idxScriptClass -1");
	}

	return false;

}


int CScriptFunctionObject::getIdxFunctionObject(int idxFunction,const string & function_name, bool show_msg){

	// from lat value to first to get last override function...

	tFunctionInfo *fi=&vec_script_function_object_node->at(idxFunction)->object_info;
	for(int i = fi->local_symbols.vec_idx_registeredFunction.size()-1; i >= 0 ; i--){
		if(GET_FUNCTION_INFO(fi->local_symbols.vec_idx_registeredFunction[i])->symbol_info.symbol_name == function_name){
			return i;
		}
	}

	if(show_msg){
		print_error_cr("function member %s::%s doesn't exist",fi->symbol_info.symbol_name.c_str(),function_name.c_str());
	}

	return -1;
}

int				CScriptFunctionObject::getIdxVariableSymbol(int idxFunction,const string & function_name, bool show_msg){
	// from lat value to first to get last override function...

	tFunctionInfo *fi=&vec_script_function_object_node->at(idxFunction)->object_info;
	for(int i = fi->local_symbols.vec_idx_registeredFunction.size()-1; i >= 0 ; i--){
		if(fi->local_symbols.m_registeredVariable[i].symbol_name == function_name){
			return i;
		}
	}

	if(show_msg){
		print_error_cr("variable member %s::%s doesn't exist",fi->symbol_info.symbol_name.c_str(),function_name.c_str());
	}

	return -1;
}



CScriptFunctionObject 	* CScriptFunctionObject::getScriptFunctionObject(int idx){
	if(idx < 0 || (unsigned)idx >= vec_script_function_object_node->size()){
		print_error_cr("CScriptFunctionObject node out of bound");
		return NULL;
	}

	return vec_script_function_object_node->at(idx);
}

tFunctionInfo 	* CScriptFunctionObject::getFunctionInfo(int idx){
	if(idx < 0 || (unsigned)idx >= vec_script_function_object_node->size()){
		print_error_cr("CScriptFunctionObject node out of bound");
		return NULL;
	}

	return &vec_script_function_object_node->at(idx)->object_info;
}
