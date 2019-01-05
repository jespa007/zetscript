#include "CZetScript.h"

namespace zetscript{

	CCommonClassFunctionBase::CCommonClassFunctionBase(unsigned char _idxClass) {
		idxClass=_idxClass;
	}

	tVariableSymbolInfo * CCommonClassFunctionBase::registerVariable(const string & file, short line, short idxBlockScope,const string & variable_name, const string & c_type, intptr_t ref_ptr, unsigned short properties){
		tVariableSymbolInfo irs;
		//string symbol_ref=CEval::makeSymbolRef(variable_name,idxBlockScope);

		tSymbol * symbol;

		if((symbol=GET_SCOPE(idxBlockScope)->registerSymbol(file,line,variable_name /*,var_node*/))==NULL){
				return NULL;
		}


		if(getVariable(irs.symbol->name,irs.symbol->idxScope) != NULL){
			THROW_RUNTIME_ERROR("Variable \"%s\" already exist",variable_name.c_str());
			return NULL;
		}

		irs.ref_ptr =ref_ptr;
		irs.symbol=symbol;
		irs.c_type = c_type;
		irs.properties = properties;

		irs.idxSymbol = (short)m_variable.size();

		m_variable.push_back(irs);

		return &m_variable[m_variable.size()-1];
	}

	tVariableSymbolInfo *	CCommonClassFunctionBase::registerVariable(const string & file, short line, const string & variable_name, const string & c_type, intptr_t ref_ptr, unsigned short properties)
	{
			return registerVariable(file,line,this->symbol_info.symbol->idxScope,  variable_name,  c_type,  ref_ptr,   properties);
	}

	tVariableSymbolInfo *	 CCommonClassFunctionBase::getVariable(const string & var_name, short idxScope){

		if(m_variable.size()>0){

			// from lat value to first to get last override function...
			for(unsigned i = m_variable.size()-1; i >= 0 ; i--){
				if((m_variable[i].symbol->name == var_name)
				&& (idxScope ==  ZS_UNDEFINED_IDX?true:(idxScope == m_variable[i].symbol->idxScope))
				  ){
					return &m_variable[i];
				}
			}
		}

		return NULL;
	}



	CScriptFunction * CCommonClassFunctionBase::registerFunction(const string & file, short line, short idxScope, const string & function_name, vector<tArgumentInfo> args, int idx_return_type,intptr_t ref_ptr, unsigned short properties){

			//string symbol_ref = CEval::makeSymbolRef(function_name,idxScope);
			if(getFunction(function_name,(char)args.size()) != NULL){
				THROW_RUNTIME_ERROR("Function \"%s\" already exist",function_name.c_str());
				return NULL;
			}

			CScriptFunction *sf =  NEW_SCRIPT_FUNCTION(file,line,idxClass,idxScope,  function_name,  args,  idx_return_type,ref_ptr, properties);
			sf->idxClass = this->idxClass;
			sf->idxLocalFunction=m_function.size();
			m_function.push_back(sf);

			return sf;
	}

	CScriptFunction * CCommonClassFunctionBase::registerFunction(const string & file, short line, const string & function_name, vector<tArgumentInfo> args, int idx_return_type,intptr_t ref_ptr, unsigned short properties){

		return registerFunction(file, line,this->symbol_info.symbol->idxScope, function_name,  args, idx_return_type,ref_ptr, properties);
	}

	CScriptFunction *	 CCommonClassFunctionBase::getFunction(const string & function_name, short idxScope, char n_args){

		if(m_function.size()>0){

			// from last value to first to get last override function...
			for(int i = (int)(m_function.size()-1); i >= 0 ; i--){
				if(
						(m_function[i]->symbol_info.symbol->name == function_name)
					 && (m_function[i]->m_arg.size() ==  n_args)
					 && (idxScope ==  ZS_UNDEFINED_IDX?true:(idxScope == m_function[i]->symbol_info.symbol->idxScope))
					 ){

					return m_function[i];
				}
			}
		}

		return NULL;
	}


}
