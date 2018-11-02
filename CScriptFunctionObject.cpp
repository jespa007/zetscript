/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

namespace zetscript{

	vector<CScriptFunction *> 	* vec_script_function_object_node=NULL;



	void CScriptFunction::setVectorScriptFunctionObjectNode(vector<CScriptFunction *> 	* set_vec){
		vec_script_function_object_node = set_vec;
	}

	vector<CScriptFunction *> 	*CScriptFunction::getVectorScriptFunctionObjectNode(){
		return vec_script_function_object_node;
	}


	CScriptFunction *		 CScriptFunction::newScriptFunctionObject(short idxScope, short idxScriptClass){
		CScriptFunction *script_function = new CScriptFunction(idxScope,idxScriptClass);
		vec_script_function_object_node->push_back(script_function);
		script_function->idxScriptFunctionObject = vec_script_function_object_node->size()-1;
		return script_function;
	}

	bool CScriptFunction::checkCanRegister_C_Function(const char *f){

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

	int CScriptFunction::registerFunction(short idxScope, const string & function_name,  vector<tArgumentInfo>  args, int idx_return_type,intptr_t ref_ptr, unsigned short properties){

		CScriptFunction *irs = NEW_SCRIPT_FUNCTION_OBJECT(idxScope,symbol_info.idxScriptClass);

		irs->m_arg = args;
		irs->idx_return_type = idx_return_type;
		irs->symbol_info.ref_ptr = ref_ptr;


		irs->symbol_info.symbol_ref = CCompiler::makeSymbolRef(function_name,idxScope); // <-- defined as global
		irs->symbol_info.properties = properties;

		irs->symbol_info.idxSymbol = (short)(scope_info.local_symbols.vec_idx_registeredFunction.size());
		scope_info.local_symbols.vec_idx_registeredFunction.push_back(irs->idxScriptFunctionObject);

		return scope_info.local_symbols.vec_idx_registeredFunction.size()-1;
	}

	int CScriptFunction::registerLocalFunction(const string & function_name,  vector<tArgumentInfo>  args, int idx_return_type,intptr_t ref_ptr, unsigned short properties){

		return newFunction(scope_info.idxScope,function_name,  args, idx_return_type,ref_ptr, properties);
	}


	int CScriptFunction::registerLocalVariable(const string & variable_name, const string & c_type, intptr_t ref_ptr, unsigned short properties){

		tVariableSymbolInfo irs;
		irs.symbol_ref=CCompiler::makeSymbolRef(variable_name,idxScope);

		irs.ref_ptr =ref_ptr;
		irs.c_type = c_type;
		irs.properties = properties;

		irs.idxSymbol = (short)scope_info.local_symbols.m_registeredVariable.size();

		scope_info.local_symbols.m_registeredVariable.push_back(irs);

		return scope_info.local_symbols.m_registeredVariable.size()-1;

	}


	int CScriptFunction::getLocalFunction(const string & function_ref,char n_args){

		for(int i = scope_info.local_symbols.vec_idx_registeredFunction.size()-1; i >= 0 ; i--){
			CScriptFunction * sfo = GET_SCRIPT_FUNCTION_OBJECT(scope_info.local_symbols.vec_idx_registeredFunction[i]);

			if(sfo->symbol_info.symbol_ref == function_ref && (n_args==(int)sfo->m_arg.size() || (n_args==-1)) ){

				return i;
			}
		}

		return -1;
	}

	int				CScriptFunction::getLocalVariable(const string & variable_ref){
		// from lat value to first to get last override function...
		for(int i = scope_info.local_symbols.m_registeredVariable.size()-1; i >= 0 ; i--){
			if(scope_info.local_symbols.m_registeredVariable[i].symbol_ref == variable_ref){
				return i;
			}
		}

		return -1;
	}



	CScriptFunction 	* CScriptFunction::getScriptFunctionObject(int idx){
		if(idx < 0 || (unsigned)idx >= vec_script_function_object_node->size()){
			THROW_RUNTIME_ERROR("CScriptFunction node out of bound");
			return NULL;
		}

		return vec_script_function_object_node->at(idx);
	}


}
