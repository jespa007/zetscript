/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zs_core.h"

namespace zetscript{

	vector<CScriptFunctionObject *> 	* vec_script_function_object_node=NULL;



	void CScriptFunctionObject::setVectorScriptFunctionObjectNode(vector<CScriptFunctionObject *> 	* set_vec){
		vec_script_function_object_node = set_vec;
	}

	vector<CScriptFunctionObject *> 	*CScriptFunctionObject::getVectorScriptFunctionObjectNode(){
		return vec_script_function_object_node;
	}


	CScriptFunctionObject *		 CScriptFunctionObject::newScriptFunctionObject(){
		CScriptFunctionObject *script_function = new CScriptFunctionObject();
		vec_script_function_object_node->push_back(script_function);
		script_function->object_info.idxScriptFunctionObject = vec_script_function_object_node->size()-1;
		return script_function;
	}


	tInfoVariableSymbol * CScriptFunctionObject::newVariableSymbol(int idxFunction){

		if(idxFunction != -1 && idxFunction<(int)vec_script_function_object_node->size()){


			tInfoVariableSymbol irs;
			CScriptFunctionObject *irc = vec_script_function_object_node->at(idxFunction);


			irs.idxSymbol = (short)irc->object_info.local_symbols.m_registeredVariable.size();


			irc->object_info.local_symbols.m_registeredVariable.push_back(irs);
			return &irc->object_info.local_symbols.m_registeredVariable[irc->object_info.local_symbols.m_registeredVariable.size()-1];
		}
		else{
			zs_print_error("idxScriptClass -1");
		}

		return NULL;

	}


	int CScriptFunctionObject::getIdxFunctionObject(tFunctionInfo * info_function,const string & function_name,char n_args_to_find, bool show_msg){

		// from lat value to first to get last override function...
		/*if(idxFunction == -1){
			zs_print_error_cr("-1 given");
			return -1;
		}*/

		//tFunctionInfo *fi=&vec_script_function_object_node->at(idxFunction)->object_info;
		for(int i = info_function->local_symbols.vec_idx_registeredFunction.size()-1; i >= 0 ; i--){
			CScriptFunctionObject * sfo = GET_SCRIPT_FUNCTION_OBJECT(info_function->local_symbols.vec_idx_registeredFunction[i]);


			if(sfo->object_info.symbol_info.symbol_name == function_name && (n_args_to_find==(int)sfo->m_arg.size() || (n_args_to_find==-1)) ){

				return i;
			}
		}

		if(show_msg){
			zs_print_error_cr("function member %s::%s doesn't exist",info_function->symbol_info.symbol_name.c_str(),function_name.c_str());
		}

		return -1;
	}

	int				CScriptFunctionObject::getIdxVariableSymbol(tFunctionInfo * info_function,const string & function_name, bool show_msg){
		// from lat value to first to get last override function...


		for(int i = info_function->local_symbols.m_registeredVariable.size()-1; i >= 0 ; i--){
			if(info_function->local_symbols.m_registeredVariable[i].symbol_name == function_name){
				return i;
			}
		}

		if(show_msg){
			zs_print_error_cr("variable member %s::%s doesn't exist",info_function->symbol_info.symbol_name.c_str(),function_name.c_str());
		}

		return -1;
	}



	CScriptFunctionObject 	* CScriptFunctionObject::getScriptFunctionObject(int idx){
		if(idx < 0 || (unsigned)idx >= vec_script_function_object_node->size()){
			zs_print_error_cr("CScriptFunctionObject node out of bound");
			return NULL;
		}

		return vec_script_function_object_node->at(idx);
	}

	tFunctionInfo 	* CScriptFunctionObject::getFunctionInfo(int idx){
		if(idx < 0 || (unsigned)idx >= vec_script_function_object_node->size()){
			zs_print_error_cr("CScriptFunctionObject node out of bound");
			return NULL;
		}

		return &vec_script_function_object_node->at(idx)->object_info;
	}
}
