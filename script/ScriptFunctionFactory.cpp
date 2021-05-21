#include "zetscript.h"

namespace zetscript{

	ScriptFunctionFactory::ScriptFunctionFactory(ZetScript *_zs){
		zs = _zs;
		scope_factory = _zs->getScopeFactory();
		script_functions = new zs_vector();
		idx_clear_checkpoint = 1; // by default MAIN function
	}

	zs_vector 	*ScriptFunctionFactory::getScriptFunctions(){
		return script_functions;
	}

	Symbol *		 ScriptFunctionFactory::newScriptFunction(
			//--- Register data
			  Scope *scope
			, const char * file
			, short line
			//--- Function data
			, int idx_class
			, short _idx_position
			, const std::string & function_name
			, std::vector<ScriptFunctionArg> args
			, int idx_return_type
			, zs_int ref_native_function_ptr
			, unsigned short properties
		){

		Symbol *symbol=NULL;

		symbol=scope->addSymbol(
				file
				,line
				,function_name
				,(char)args.size()
		);

		short idx_script_function = script_functions->count;

		// sets local function information...
		symbol->idx_position = _idx_position; // idx local/member function
		symbol->properties = properties | SYMBOL_PROPERTY_FUNCTION;

		ScriptFunction *script_function = new ScriptFunction(
				zs
				,idx_class
				,idx_script_function
				,args
				,idx_return_type
				,symbol
				,ref_native_function_ptr
		);

		symbol->ref_ptr = (zs_int)script_function;		  // ptr function
		script_functions->push_back((zs_int)script_function);
		return symbol;
	}

	void	ScriptFunctionFactory::setScriptFunction(short idx, ScriptFunction *sf){
		if(idx < 0 || (unsigned)idx >= script_functions->count){
			THROW_RUNTIME_ERROR("script function idx node out of bound");
		}

		script_functions->items[idx]=(zs_int)sf;
	}

	ScriptFunction 	* ScriptFunctionFactory::getScriptFunction(int idx){
		if(idx < 0 || (unsigned)idx >= script_functions->count){
			THROW_RUNTIME_ERROR("script function idx node out of bound");
			return NULL;
		}
		return (ScriptFunction 	*)script_functions->items[idx];
	}

	void ScriptFunctionFactory::clear(int _idx_start){

		int idx_start = _idx_start == ZS_IDX_UNDEFINED ?  idx_clear_checkpoint:_idx_start;

		for(
			int v=script_functions->count-1;
			v >= idx_start; // avoid delete main function
			v--
		){

			ScriptFunction * info_function = (ScriptFunction * )script_functions->items[v];
			delete info_function;
			script_functions->pop_back();

		}
	}

	void ScriptFunctionFactory::setClearCheckpoint(){
		idx_clear_checkpoint = script_functions->count-1;
	}

	ScriptFunctionFactory::~ScriptFunctionFactory(){
		// erases all functions...
		for(unsigned i = 0;i < script_functions->count;i++){
			ScriptFunction * info_function = (ScriptFunction *)script_functions->items[i];

			if (info_function->instructions != NULL) {

				free(info_function->instructions);
				info_function->instructions=NULL;
			}

			delete info_function;

		}

		script_functions->clear();

		delete script_functions;
		script_functions=NULL;
	}
};
