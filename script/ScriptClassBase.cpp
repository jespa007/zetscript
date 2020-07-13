#include "zetscript.h"

namespace zetscript{

	ScriptClassBase::ScriptClassBase(ZetScript * _zs,unsigned char _idx_class) {
		idx_class=_idx_class;
		zs = _zs;
		scope_factory = zs->getScopeFactory();
		script_function_factory= zs->getScriptFunctionFactory();
		virtual_machine = zs->getVirtualMachine();
		script_class_factory=zs->getScriptClassFactory();


	}

	SymbolInfo * ScriptClassBase::registerVariable(const std::string & file, short line, short idx_scope_block,const std::string & variable_name, const std::string & c_type, intptr_t ref_ptr, unsigned short symbol_info_properties){
		SymbolInfo irs;
		//std::string symbol_ref=ScriptEval::makeSymbolRef(variable_name,idx_scope_block);

		Symbol * symbol=NULL;

		if((symbol=GET_SCOPE(this,idx_scope_block)->registerSymbol(file,line,variable_name /*,var_node*/))==NULL){
				return NULL;
		}


		if(getVariable(symbol->name,symbol->idx_scope) != NULL){
			THROW_RUNTIME_ERROR(zs_strutils::format("Variable \"%s\" already exist",variable_name.c_str()));
			return NULL;
		}

		irs.ref_ptr =ref_ptr;
		irs.symbol=symbol;
		irs.c_type = c_type;
		irs.symbol_info_properties = symbol_info_properties;

		irs.idx_symbol = (short)local_variable.size();

		local_variable.push_back(irs);

		return &local_variable[local_variable.size()-1];
	}

	SymbolInfo *	ScriptClassBase::registerVariable(const std::string & file, short line, const std::string & variable_name, const std::string & c_type, intptr_t ref_ptr, unsigned short symbol_info_properties)
	{
			return registerVariable(
					file
					,line
					,this->symbol_info.symbol->idx_scope
					,  variable_name
					,  c_type
					,  ref_ptr
					,   symbol_info_properties
			);
	}

	SymbolInfo *	 ScriptClassBase::getVariable(const std::string & var_name, short idx_scope){

		if(local_variable.size()>0){

			// from lat value to first to get last override function...
			for(int i = (int)local_variable.size()-1; i >= 0 ; i--){
				if((local_variable[i].symbol->name == var_name)
				&& (idx_scope ==  ZS_IDX_UNDEFINED?true:(idx_scope == local_variable[i].symbol->idx_scope))
				  ){
					return &local_variable[i];
				}
			}
		}

		return NULL;
	}



	ScriptFunction * ScriptClassBase::registerFunction(const std::string & file, short line, short idx_scope, const std::string & function_name, std::vector<ParamArgInfo> args, int idx_return_type,intptr_t ref_ptr, unsigned short symbol_info_properties){

			//std::string symbol_ref = ScriptEval::makeSymbolRef(function_name,idx_scope);
			if(getFunction(function_name,(char)args.size()) != NULL){
				THROW_RUNTIME_ERROR(zs_strutils::format("Function \"%s\" already exist",function_name.c_str()));
				return NULL;
			}

			ScriptFunction *sf =  script_function_factory->newScriptFunction(
					file
					,line
					,idx_class
					,idx_scope
					,local_function.size() // idx_symbol
					,function_name
					,args
					,idx_return_type
					,ref_ptr
					, symbol_info_properties
			);

			local_function.push_back(sf);

			return sf;
	}

	ScriptFunction * ScriptClassBase::registerFunction(const std::string & file, short line, const std::string & function_name, std::vector<ParamArgInfo> args, int idx_return_type,intptr_t ref_ptr, unsigned short symbol_info_properties){

		return registerFunction(file, line,this->symbol_info.symbol->idx_scope, function_name,  args, idx_return_type,ref_ptr, symbol_info_properties);
	}

	ScriptFunction *	 ScriptClassBase::getFunction(const std::string & function_name, short idx_scope, char n_args){

		if(local_function.size()>0){

			// from last value to first to get last override function...
			for(int i = (int)(local_function.size()-1); i >= 0 ; i--){
				if(
						(local_function[i]->symbol_info.symbol->name == function_name)
					 && (n_args == (int)local_function[i]->arg_info.size())
					 && (idx_scope ==  ZS_IDX_UNDEFINED?true:(idx_scope == local_function[i]->symbol_info.symbol->idx_scope))
					 ){

					return local_function[i];
				}
			}
		}

		return NULL;
	}


}
