#include "CZetScript.h"

namespace zetscript{



	CCommonClassFunctionBase::CCommonClassFunctionBase(unsigned char _idxClass, short _idxScope) {
		//n_statments=0;

		//n_statment_op=0;

		idxScope=_idxScope;
		idxClass=_idxClass;
	}

	/*
	 *  It has no sense since it's resolved at eval
	 *  super.fun(1,2,3)*3; fun is evaluated pushing its args and then solve the function because we have the name and the args (is trivial)
	 *
	 *
	bool CCommonClassFunctionBase::searchVarFunctionSymbol(string symbol_to_find, tInstruction *iao, int current_function, bool & symbol_not_found, unsigned int param_scope_type, int n_args_to_find){

		int idx=0;
		symbol_not_found = true;

		//PASTNode ast_node = AST_NODE(iao->idxAstNode);
		//string symbol_to_find =ast_node->symbol_value;//CCompiler::makeSymbolRef(ast_node->symbol_value, ast_node->idxScope);



		unsigned int iao_scope = GET_INS_PROPERTY_SCOPE_TYPE(iao->properties);

		if(iao_scope & INS_PROPERTY_SUPER_SCOPE){ // try deduce local/global

			if(current_function > 0){ // minimum have to have a 1

				CScriptFunction *sfo =  GET_SCRIPT_FUNCTION(m_function[current_function]);//.object_info;
				symbol_to_find = sfo->symbol_info.symbol_ref;
				short idxScope=CCompiler::getIdxScopeFromSymbolRef(symbol_to_find);

				if(idxScope == -1){
					return false;
				}

				int idx_super=-1;
				bool is_c=false;

				for(int i = current_function-1; i >= 0 && idx_super==-1; i--){
					CScriptFunction * sfo = GET_SCRIPT_FUNCTION(scope_info->local_symbols.function[i]);
					string symbol_ref=CCompiler::getSymbolNameFromSymbolRef(sfo->symbol_info.symbol_ref);
					string symbol_ref_to_find=CCompiler::getSymbolNameFromSymbolRef(symbol_to_find);

					if((symbol_ref == symbol_ref_to_find) && ((int)sfo->m_arg.size() == n_args_to_find)){ // match name and args ...
						is_c = (sfo->symbol_info.properties & PROPERTY_C_OBJECT_REF) != 0;
						idx_super=i;
					}
				}

				if(idx_super!= -1){
					 REMOVE_SCOPES(iao->properties);
					 iao->properties |= param_scope_type;
					 iao->index_op1 = LOAD_TYPE_FUNCTION;
					 iao->index_op2 = idx_super;
					 iao->properties |= (is_c ? INS_PROPERTY_DEDUCE_C_CALL : 0);
					 //iao->idxFunction = idxFunction;
					 return true;

				}
				else{ // symbol not found ...
					return false;
				}
			}
			else{
				return false;
			}
		}
		else{

			int idx_scope=ast_node->idxScope;
			bool partial_c_class= false;

			if(idxClass != ZS_INVALID_CLASS){
				CScriptClass *sc=CScriptClass::getScriptClassByIdx(symbol_info.idxClass);

				if(symbol_to_find == "this" && (iao_scope & INS_PROPERTY_THIS_SCOPE)){ // trivial is the first symbol we find...
					 REMOVE_SCOPES(iao->properties);
					 iao->properties |= INS_PROPERTY_THIS_SCOPE;
					 iao->index_op1 = LOAD_TYPE_VARIABLE;
					 iao->index_op2 = ZS_THIS_IDX;
					 return true;
				}

				 partial_c_class=(symbol_info.properties&SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) !=0;
				while( sc->idxBaseClass.size()>0 && !partial_c_class){

					sc=CScriptClass::getScriptClassByIdx(sc->idxBaseClass[0]); // get base class...
					partial_c_class|=(sc->metadata_info.object_info.symbol_info.properties&SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) !=0;
				}

				if(iao_scope & INS_PROPERTY_THIS_SCOPE){ // start from class scope to find its variable/function member...
					sc=CScriptClass::getScriptClassByIdx(info_function->symbol_info.idxClass);
					idx_scope=AST_NODE(sc->metadata_info.object_info.symbol_info.idxAstNode)->idxScope;
				}
			}


			bool variable_in_main_class=SCOPE_IN_MAIN_CLASS(ast_node->idxScope);

			 bool end=false;


			 while(!end){ // we try all scopes until match symbols at right scope...

				 symbol_to_find=CCompiler::makeSymbolRef(ast_node->symbol_value,idx_scope);

				 if((idx=CScriptFunction::getIdxFunctionObject(this,symbol_to_find,n_args_to_find,false))!=-1){
					 REMOVE_SCOPES(iao->properties);
					 iao->properties |= param_scope_type;
					 iao->index_op1 = LOAD_TYPE_FUNCTION;
					 iao->index_op2 = idx;
					 if((GET_FUNCTION_INFO(m_function[idx])->symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF){ // set as -1 to search the right signature ...
						 iao->index_op2 = -1;
					 }
					 return true;
				 }else {

					 if((idx=CScriptFunction::getIdxVariableSymbol(info_function,symbol_to_find, false))!=-1){
						 REMOVE_SCOPES(iao->properties);
						 iao->properties |= param_scope_type;
						 iao->index_op1 = LOAD_TYPE_VARIABLE;
						 iao->index_op2 = idx;
						 return true;
					 }
				 }

			 	if(idx_scope >= 0){ // var local or class member ?
					 CScope *sc=GET_SCOPE(idx_scope);
				 	 idx_scope=sc->getIdxParent();
			 	}

			 	if(idx_scope<0){

			 		idx_scope=ZS_UNDEFINED_IDX;

			 		if(partial_c_class){ // var in c scope ?

			 			idx_scope = IDX_C_CLASS_SCOPE;
			 			partial_c_class=false; // <-- set it false to tell that is already test

			 		}else{ // finally try global...

						 if(!variable_in_main_class){ // try global scope...
							 variable_in_main_class=true;
							 CScriptFunction *main=MAIN_SCRIPT_FUNCTION_OBJECT;
							 info_function=&main->object_info; // set main function as other...
							 idx_scope=0; // set scope global and try last...
							 param_scope_type=0; // override to global scope...
						 }
			 		}
			 	 }

			 	 end = idx_scope == ZS_UNDEFINED_IDX;


			 }
		}
		symbol_not_found = false;
		 return false;
	}
*/

	CScriptFunction * CCommonClassFunctionBase::registerFunction(const string & file, short line, short idxScope, const string & function_name, vector<tArgumentInfo> args, int idx_return_type,intptr_t ref_ptr, unsigned short properties){

			string symbol_ref = CEval::makeSymbolRef(function_name,idxScope);
			if(getFunctionByRef(symbol_ref,(char)args.size()) != NULL){
				THROW_RUNTIME_ERROR("Function \"%s\" already exist",function_name.c_str());
				return NULL;
			}

			CScriptFunction *sf =  NEW_SCRIPT_FUNCTION(idxClass,idxScope,  symbol_ref,  args,  idx_return_type,ref_ptr, properties);
			sf->idxLocalFunction=m_function.size();
			m_function.push_back(sf);

			return sf;
	}

	CScriptFunction * CCommonClassFunctionBase::registerFunction(const string & file, short line, const string & function_name, vector<tArgumentInfo> args, int idx_return_type,intptr_t ref_ptr, unsigned short properties){

		return registerFunction(file, line,this->idxScope, function_name,  args, idx_return_type,ref_ptr, properties);
	}

	CScriptFunction *	 CCommonClassFunctionBase::getFunction(const string & function_name, short idxScope, char n_args){

		if(m_function.size()>0){

			// from lat value to first to get last override function...
			for(int i = (int)(m_function.size()-1); i >= 0 ; i--){
				if(
						(m_function[i]->symbol_info->symbol.name == function_name)
					 && (m_function[i]->m_arg.size() ==  n_args)
					 && (idxScope ==  ZS_UNDEFINED_IDX?true:(idxScope == m_function[i]->symbol_info->symbol.idxScope))
					 ){

					return m_function[i];
				}
			}
		}

		return NULL;
	}

/*
	CScriptFunction *	 CCommonClassFunctionBase::getFunctionByName(const string & function_name, char n_args){

		if(m_function.size()>0){

			// from lat value to first to get last override function...
			for(int i = (int)(m_function.size()-1); i >= 0 ; i--){
				if((CEval::getSymbolNameFromSymbolRef(m_function[i]->symbol_info.symbol_ref) == function_name) && (m_function[i]->m_arg.size() ==  n_args)){
					return m_function[i];
				}
			}
		}

		return NULL;
	}
*/

	tVariableSymbolInfo * CCommonClassFunctionBase::registerVariable(const string & file, short line, short idxBlockScope,const string & variable_name, const string & c_type, intptr_t ref_ptr, unsigned short properties){
		tVariableSymbolInfo irs;
		//string symbol_ref=CEval::makeSymbolRef(variable_name,idxBlockScope);

		tSymbol * symbol;

		if((symbol=GET_SCOPE(idxBlockScope)->registerSymbol(file,line,variable_name /*,var_node*/))==NULL){
				return NULL;
		}


		if(getVariableByRef(irs.symbol_ref) != NULL){
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
			return registerVariable(this->idxScope,  variable_name,  c_type,  ref_ptr,   properties);
	}

	tVariableSymbolInfo *	 CCommonClassFunctionBase::getVariable(const string & var_name, short idxScope){

		if(m_variable.size()>0){

			// from lat value to first to get last override function...
			for(unsigned i = m_variable.size()-1; i >= 0 ; i--){
				if((m_variable[i].symbol.name == var_name)
				&& (idxScope ==  ZS_UNDEFINED_IDX?true:(idxScope == m_variable[i]->symbol_info->symbol.idxScope))
				  ){
					return &m_variable[i];
				}
			}
		}

		return NULL;
	}


	tVariableSymbolInfo *	 CCommonClassFunctionBase::getVariableByName(const string & var_name){

		if(m_variable.size()>0){

			// from lat value to first to get last override function...
			for(unsigned i = m_variable.size()-1; i >= 0 ; i--){
				if(CEval::getSymbolNameFromSymbolRef(m_variable[i].symbol_ref) == var_name){
					return &m_variable[i];
				}
			}
		}

		return NULL;
	}


}
