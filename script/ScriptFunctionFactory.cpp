#include "zetscript.h"

namespace zetscript{


	ScriptFunctionFactory::ScriptFunctionFactory(ZetScript *_zs){
		zs = _zs;
		scope_factory = _zs->getScopeFactory();
	}

	std::vector<ScriptFunction *> 	*ScriptFunctionFactory::getScriptFunctions(){
		return &script_functions;
	}

	ScriptFunction *		 ScriptFunctionFactory::newScriptFunction(const std::string & file
			, short line
			, unsigned char idx_class
			, short idx_scope
			, short idx_symbol
			, const std::string & function_name
			, std::vector<ParamArgInfo> args
			, int idx_return_type
			,intptr_t ref_ptr
			, unsigned short symbol_info_properties){

		if((symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTY_C_OBJECT_REF){
			if(script_functions.size() > 1){ // if greather than 1 check if node consecutive...
				if(!((script_functions.at(script_functions.size()-1)->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTY_C_OBJECT_REF)){ // non consecutive c node..
						THROW_RUNTIME_ERROR("C Functions should be added after global scope and consecutuve C scope node.");
						return NULL;
				}
			}
		}

		Symbol *symbol;
		if((symbol=GET_SCOPE(this,idx_scope)->registerSymbol(file,line,function_name,args.size()))==NULL){
				return NULL;
		}

		ScriptFunction *irs = new ScriptFunction(
				zs
				,idx_class
				/*,script_functions.size()
				,args
				,idx_return_type
				,symbol_info_ref_ptr
				,symbol_info_symbol
				,symbol_info_properties
				,symbol_info_idx_symbol*/
		);

		irs->arg_info = args;
		irs->idx_return_type = idx_return_type;
		irs->symbol_info.ref_ptr = ref_ptr;
		irs->symbol_info.idx_symbol = idx_symbol;
		irs->symbol_info.symbol = symbol;
		irs->symbol_info.symbol_info_properties = symbol_info_properties;


		irs->idx_script_function=script_functions.size();
		script_functions.push_back(irs);

		return irs;
	}

	ScriptFunction 	* ScriptFunctionFactory::getScriptFunction(int idx){
		if(idx < 0 || (unsigned)idx >= script_functions.size()){
			THROW_RUNTIME_ERROR("ScriptFunction node out of bound");
			return NULL;
		}

		return script_functions.at(idx);
	}

	bool ScriptFunctionFactory::checkCanregister_C_Function(const std::string &function_name){

		int size = script_functions.size();

		if(size>=3){ //0 is main function (reserved). We check when >= 3 to check previous one (i.e from 1)
			if((

				((script_functions)[size-1]->symbol_info.symbol_info_properties&SYMBOL_INFO_PROPERTY_C_OBJECT_REF)!=SYMBOL_INFO_PROPERTY_C_OBJECT_REF)
			){
				THROW_RUNTIME_ERROR(zs_strutils::format("function \"%s\" should register after C functions. Register after script functions is not allowed",function_name.c_str()));
				return false;
			}

		}

		return true;
	}

	void ScriptFunctionFactory::clear(){
		bool end=false;
		do{
			ScriptFunction * info_function = script_functions.at(script_functions.size()-1);
			end=(info_function->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTY_C_OBJECT_REF || script_functions.size()==1;

			if(!end){

				if (info_function->instruction != NULL) {
					//for (PtrInstruction stat = info_function->object_info.instruction; *stat != NULL; stat++) {

						//free(*stat);
					//}

					free(info_function->instruction);
					info_function->instruction=NULL;
				}

				// unloading scope ...
				if (info_function->scope_block_vars != NULL) {
					for (unsigned j = 0; j < info_function->n_scope_block_vars; j++) {
						free(info_function->scope_block_vars[j].idx_local_var);
					}

					free(info_function->scope_block_vars);
					info_function->scope_block_vars=NULL;
				}

				script_functions.pop_back();
				delete info_function;

			}

		}while(!end);

	}


	ScriptFunctionFactory::~ScriptFunctionFactory(){
		for(unsigned i = 0;i < script_functions.size();i++){
				//ZS_PRINT_DEBUG("* Erasing function %s...", script_functions.at(i)->object_info.symbol_info.symbol_ref.c_str());
				ScriptFunction * info_function = script_functions.at(i);

				if (info_function->instruction != NULL) {

					free(info_function->instruction);
					info_function->instruction=NULL;
				}

				// unloading scope ...
				if (info_function->scope_block_vars != NULL) {
					for (unsigned j = 0; j < info_function->n_scope_block_vars; j++) {
						free(info_function->scope_block_vars[j].idx_local_var);
					}

					free(info_function->scope_block_vars);
					info_function->scope_block_vars=NULL;
				}

				delete info_function;
			}

			script_functions.clear();
	}

};
