#include "CZetScript.h"

namespace zetscript{



	CBaseClassFunctionData::CBaseClassFunctionData() {
		//n_statments=0;

		//n_statment_op=0;

		idxScope=ZS_UNDEFINED_IDX;
		idxScriptClass=ZS_UNDEFINED_IDX;
	}

	/*
	 *  It has no sense since it's resolved at eval
	 *  super.fun(1,2,3)*3; fun is evaluated pushing its args and then solve the function because we have the name and the args (is trivial)
	 *
	 *
	bool CBaseClassFunctionData::searchVarFunctionSymbol(string symbol_to_find, tInstruction *iao, int current_function, bool & symbol_not_found, unsigned int param_scope_type, int n_args_to_find){

		int idx=0;
		symbol_not_found = true;

		//PASTNode ast_node = AST_NODE(iao->idxAstNode);
		//string symbol_to_find =ast_node->symbol_value;//CCompiler::makeSymbolRef(ast_node->symbol_value, ast_node->idxScope);



		unsigned int iao_scope = GET_INS_PROPERTY_SCOPE_TYPE(iao->instruction_properties);

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
					 REMOVE_SCOPES(iao->instruction_properties);
					 iao->instruction_properties |= param_scope_type;
					 iao->index_op1 = LOAD_TYPE_FUNCTION;
					 iao->index_op2 = idx_super;
					 iao->instruction_properties |= (is_c ? INS_PROPERTY_DEDUCE_C_CALL : 0);
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

			if(idxScriptClass != ZS_INVALID_CLASS){
				CScriptClass *sc=CScriptClass::getScriptClassByIdx(symbol_info.idxScriptClass);

				if(symbol_to_find == "this" && (iao_scope & INS_PROPERTY_THIS_SCOPE)){ // trivial is the first symbol we find...
					 REMOVE_SCOPES(iao->instruction_properties);
					 iao->instruction_properties |= INS_PROPERTY_THIS_SCOPE;
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
					sc=CScriptClass::getScriptClassByIdx(info_function->symbol_info.idxScriptClass);
					idx_scope=AST_NODE(sc->metadata_info.object_info.symbol_info.idxAstNode)->idxScope;
				}
			}


			bool variable_in_main_class=SCOPE_IN_MAIN_CLASS(ast_node->idxScope);

			 bool end=false;


			 while(!end){ // we try all scopes until match symbols at right scope...

				 symbol_to_find=CCompiler::makeSymbolRef(ast_node->symbol_value,idx_scope);

				 if((idx=CScriptFunction::getIdxFunctionObject(this,symbol_to_find,n_args_to_find,false))!=-1){
					 REMOVE_SCOPES(iao->instruction_properties);
					 iao->instruction_properties |= param_scope_type;
					 iao->index_op1 = LOAD_TYPE_FUNCTION;
					 iao->index_op2 = idx;
					 if((GET_FUNCTION_INFO(m_function[idx])->symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF){ // set as -1 to search the right signature ...
						 iao->index_op2 = -1;
					 }
					 return true;
				 }else {

					 if((idx=CScriptFunction::getIdxVariableSymbol(info_function,symbol_to_find, false))!=-1){
						 REMOVE_SCOPES(iao->instruction_properties);
						 iao->instruction_properties |= param_scope_type;
						 iao->index_op1 = LOAD_TYPE_VARIABLE;
						 iao->index_op2 = idx;
						 return true;
					 }
				 }

			 	if(idx_scope >= 0){ // var local or class member ?
					 CScope *sc=SCOPE_NODE(idx_scope);
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

	int CBaseClassFunctionData::registerFunction(short idxBlockScope, const string & function_name, vector<tArgumentInfo> args, int idx_return_type,intptr_t ref_ptr, unsigned short properties){
			CScriptFunction *irs = NEW_SCRIPT_FUNCTION(idxBlockScope,idxScriptClass);

			irs->m_arg = args;
			irs->idx_return_type = idx_return_type;
			irs->symbol_info.ref_ptr = ref_ptr;


			irs->symbol_info.symbol_ref = CEval::makeSymbolRef(function_name,idxBlockScope); // <-- defined as global
			irs->symbol_info.properties = properties;

			irs->symbol_info.idxSymbol = (short)(m_function.size());
			m_function.push_back(irs->idxScriptFunctionObject);

			return m_function.size()-1;
	}

	int CBaseClassFunctionData::registerFunction( const string & function_name, vector<tArgumentInfo> args, int idx_return_type,intptr_t ref_ptr, unsigned short properties){


		return registerFunction(this->idxScope, function_name,  args, idx_return_type,ref_ptr, properties);
	}


	int 	CBaseClassFunctionData::registerVariable(short idxBlockScope,const string & variable_name,const string & variable_ref, const string & c_type, intptr_t ref_ptr, unsigned short properties){
		tVariableSymbolInfo irs;
		irs.symbol_ref=CEval::makeSymbolRef(variable_name,idxBlockScope);

		irs.ref_ptr =ref_ptr;
		irs.c_type = c_type;
		irs.properties = properties;

		irs.idxSymbol = (short)m_variable.size();

		m_variable.push_back(irs);

		return m_variable.size()-1;
	}

	int 	CBaseClassFunctionData::registerVariable(const string & variable_name,const string & variable_ref, const string & c_type, intptr_t ref_ptr, unsigned short properties){
	{
			return registerVariable(this->idxScope,  variable_name,   variable_ref,    c_type,  ref_ptr,   properties);
	}

	int CBaseClassFunctionData::getVariable(const string & symbol_ref){

		if(m_variable.size()>0){

			// from lat value to first to get last override function...
			for(unsigned i = m_variable.size()-1; i >= 0 ; i--){
				if(m_variable[i].symbol_ref == symbol_ref){
					return i;
				}
			}
		}

		return ZS_UNDEFINED_IDX;
	}



}
