/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

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
		script_function->idxScriptFunctionObject = vec_script_function_object_node->size()-1;
		return script_function;
	}

	bool CScriptFunctionObject::checkCanRegister_C_Function(const char *f){

		int size = vec_script_function_object_node->size();

		if(size>=3){ //0 is main function (reserved). We check when >= 3 to check previous one (i.e from 1)
			if((

				((*vec_script_function_object_node)[size-1]->symbol_info.properties&PROPERTY_C_OBJECT_REF)!=PROPERTY_C_OBJECT_REF)
			){
				THROW_RUNTIME_ERROR("function \"%s\" should register after C functions. Register after script functions is not allowed",f);
				return false;
			}

		}

		return true;
	}


	tVariableSymbolInfo * CScriptFunctionObject::newVariableSymbol(int idxFunction){

		if(idxFunction != -1 && idxFunction<(int)vec_script_function_object_node->size()){


			tVariableSymbolInfo irs;
			CScriptFunctionObject *irc = vec_script_function_object_node->at(idxFunction);


			irs.idxSymbol = (short)irc->scope_info.local_symbols.m_registeredVariable.size();


			irc->scope_info.local_symbols.m_registeredVariable.push_back(irs);
			return &irc->scope_info.local_symbols.m_registeredVariable[irc->scope_info.local_symbols.m_registeredVariable.size()-1];
		}
		else{
			THROW_RUNTIME_ERROR("idxScriptClass -1");
		}

		return NULL;

	}


	int CScriptFunctionObject::getIdxFunctionObject(CScriptFunctionObject * in_sfo,const string & function_name,char n_args_to_find, bool show_msg){

		// from lat value to first to get last override function...
		/*if(idxFunction == -1){
			THROW_RUNTIME_ERROR("-1 given");
			return -1;
		}*/

		//tFunctionInfo *fi=&vec_script_function_object_node->at(idxFunction)->function_info;
		for(int i = in_sfo->scope_info.local_symbols.vec_idx_registeredFunction.size()-1; i >= 0 ; i--){
			CScriptFunctionObject * sfo = GET_SCRIPT_FUNCTION_OBJECT(in_sfo->scope_info.local_symbols.vec_idx_registeredFunction[i]);


			if(sfo->symbol_info.symbol_ref == function_name && (n_args_to_find==(int)sfo->m_arg.size() || (n_args_to_find==-1)) ){

				return i;
			}
		}

		if(show_msg){
			THROW_RUNTIME_ERROR("function member %s::%s doesn't exist",in_sfo->symbol_info.symbol_ref.c_str(),function_name.c_str());
		}

		return -1;
	}

	int				CScriptFunctionObject::getIdxVariableSymbol(CScriptFunctionObject * in_sfo,const string & variable_name, bool show_msg){
		// from lat value to first to get last override function...


		for(int i = in_sfo->scope_info.local_symbols.m_registeredVariable.size()-1; i >= 0 ; i--){
			if(in_sfo->scope_info.local_symbols.m_registeredVariable[i].symbol_ref == variable_name){
				return i;
			}
		}

		if(show_msg){
			THROW_RUNTIME_ERROR("variable member %s::%s doesn't exist",in_sfo->symbol_info.symbol_ref.c_str(),variable_name.c_str());
		}

		return -1;
	}



	CScriptFunctionObject 	* CScriptFunctionObject::getScriptFunctionObject(int idx){
		if(idx < 0 || (unsigned)idx >= vec_script_function_object_node->size()){
			THROW_RUNTIME_ERROR("CScriptFunctionObject node out of bound");
			return NULL;
		}

		return vec_script_function_object_node->at(idx);
	}


}
