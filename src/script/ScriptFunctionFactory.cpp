/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
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
			, int _idx_class
			, int _idx_position
			, const zs_string & _function_name
			, ScriptFunctionParam **_params
			,size_t _params_len
			, int idx_return_type
			, zs_int ref_native_function_ptr
			, unsigned short properties
		){

		Symbol *symbol=NULL;
		uint16_t properties_register_symbol=properties & SYMBOL_PROPERTY_C_OBJECT_REF? REGISTER_SCOPE_NO_CHECK_REPEATED_SYMBOLS:REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN;


		// try to not check if function name is constructor
		if((properties & FUNCTION_PROPERTY_MEMBER_FUNCTION) && (_function_name == zs->getScriptClassFactory()->getScriptClassName(_idx_class))){
			properties_register_symbol|=REGISTER_SCOPE_NO_CHECK_CLASS_SYMBOLS;
		}


		symbol=scope->registerSymbolFunction(
				file
				,line
				,_function_name
				,_params_len
				,SYMBOL_PROPERTY_FUNCTION
		);

		short idx_script_function = script_functions->count;


		ScriptFunction *script_function = new ScriptFunction(
				zs
				,scope
				,idx_script_function
				,_idx_class
				,_idx_position
				,_function_name
				,_params
				,_params_len
				,idx_return_type
				,ref_native_function_ptr
				,properties
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
			v > idx_start; // avoid delete main function
			v--
		){

			ScriptFunction * info_function = (ScriptFunction * )script_functions->items[v];
			delete info_function;
			script_functions->pop_back();

		}
	}

	void ScriptFunctionFactory::saveState(){
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
