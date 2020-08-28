#include "zetscript.h"

namespace zetscript{


	ScriptFunctionFactory::ScriptFunctionFactory(ZetScript *_zs){
		zs = _zs;
		scope_factory = _zs->getScopeFactory();
		script_functions = new zs_vector();
	}

	zs_vector 	*ScriptFunctionFactory::getScriptFunctions(){
		return script_functions;
	}

	ScriptFunction *		 ScriptFunctionFactory::newScriptFunction(
			//--- Register data
			  Scope *scope
			, const std::string & file
			, short line
			//--- Function data
			, unsigned char idx_class
			, short _idx_position
			, const std::string & function_name
			, std::vector<FunctionParam> args
			, int idx_return_type
			, intptr_t ref_ptr
			, unsigned short symbol_properties
		){

		if((symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) == SYMBOL_PROPERTY_C_OBJECT_REF){
			if(script_functions->count > 1){ // if greather than 1 check if node consecutive...
				ScriptFunction *sc = (ScriptFunction *)script_functions->items[script_functions->count-1];
				if(!((sc->symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) == SYMBOL_PROPERTY_C_OBJECT_REF)){ // non consecutive c node..
						THROW_RUNTIME_ERROR("C Functions should be added after global scope and consecutuve C scope node.");
						return NULL;
				}
			}
		}

		Symbol *symbol;
		if((symbol=scope->registerSymbol(
				file
				,line
				,function_name
				,args.size()
		))==NULL){
			return NULL;
		}

		short idx_script_function = script_functions->count;

		// sets local function information...
		symbol->ref_ptr = ref_ptr;		  // ptr function
		symbol->idx_position = _idx_position; // idx local/member function
		symbol->symbol_properties = symbol_properties;

		ScriptFunction *script_function = new ScriptFunction(
				zs
				,idx_class
				,idx_script_function
				,args
				,idx_return_type
				,symbol
				/*,script_functions.size()
				,args
				,idx_return_type
				,symbol_info_ref_ptr
				,symbol_info_symbol
				,symbol_properties
				,symbol_info_idx_symbol*/
		);



		//irs->symbol=symbol;

		//irs->idx_script_function=script_functions.size(); // idx script function ref
		script_functions->push_back((intptr_t)script_function);

		return script_function;
	}

	ScriptFunction 	* ScriptFunctionFactory::getScriptFunction(int idx){
		if(idx < 0 || (unsigned)idx >= script_functions->count){
			THROW_RUNTIME_ERROR("script function idx node out of bound");
			return NULL;
		}

		return (ScriptFunction 	*)script_functions->items[idx];
	}

	bool ScriptFunctionFactory::checkCanregisterNativeFunction(const std::string &function_name){

		int size = script_functions->count;

		if(size>=3){ //0 is main function (reserved). We check when >= 3 to check previous one (i.e from 1)
			ScriptFunction * info_function =(ScriptFunction *)script_functions->items[script_functions->count-1];
			if(
					(info_function->symbol.symbol_properties&SYMBOL_PROPERTY_C_OBJECT_REF)!=SYMBOL_PROPERTY_C_OBJECT_REF
			){
				THROW_RUNTIME_ERROR("function \"%s\" should register after C functions. Register after script functions is not allowed",function_name.c_str());
				return false;
			}

		}

		return true;
	}

	void ScriptFunctionFactory::clear(){
		bool end=false;
		do{
			ScriptFunction * info_function = (ScriptFunction * )script_functions->items[script_functions->count-1];
			end=(info_function->symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) == SYMBOL_PROPERTY_C_OBJECT_REF || script_functions->count==1;

			if(!end){

				if (info_function->instructions != NULL) {
					//for (PtrInstruction stat = info_function->object_info.instruction; *stat != NULL; stat++) {

						//free(*stat);
					//}

					free(info_function->instructions);
					info_function->instructions=NULL;
				}

				// unloading scope ...
				/*if (info_function->scope_block_vars != NULL) {
					for (unsigned j = 0; j < info_function->n_scope_block_vars; j++) {
						free(info_function->scope_block_vars[j].idx_local_var);
					}

					free(info_function->scope_block_vars);
					info_function->scope_block_vars=NULL;
				}*/

				script_functions->pop_back();
				delete info_function;

			}

		}while(!end);

	}


	ScriptFunctionFactory::~ScriptFunctionFactory(){
		for(unsigned i = 0;i < script_functions->count;i++){
			//ZS_PRINT_DEBUG("* Erasing function %s...", script_functions.at(i)->object_info.symbol_info.symbol_ref.c_str());
			ScriptFunction * info_function = (ScriptFunction *)script_functions->items[i];

			if (info_function->instructions != NULL) {

				free(info_function->instructions);
				info_function->instructions=NULL;
			}

			// unloading scope ...
			/*if (info_function->scope_block_vars != NULL) {
				for (unsigned j = 0; j < info_function->n_scope_block_vars; j++) {
					free(info_function->scope_block_vars[j].idx_local_var);
				}

				free(info_function->scope_block_vars);
				info_function->scope_block_vars=NULL;
			}*/

			delete info_function;
		}

		script_functions->clear();

		delete script_functions;
		script_functions=NULL;

	}


};
