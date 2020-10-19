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

	Symbol *		 ScriptFunctionFactory::newScriptFunction(
			//--- Register data
			  Scope *scope
			, const std::string & file
			, short line
			//--- Function data
			, ClassTypeIdx idx_class
			, short _idx_position
			, const std::string & function_name
			, std::vector<FunctionParam> args
			, int idx_return_type
			, intptr_t ref_native_function_ptr
			, unsigned short properties
		){

		Symbol *symbol=NULL;

		if((properties & SYMBOL_PROPERTY_C_OBJECT_REF)){
			if(script_functions->count > 1){ // if greather than 1 check if node consecutive...
				ScriptFunction *sc = (ScriptFunction *)script_functions->items[script_functions->count-1];
				if(!((sc->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF) == SYMBOL_PROPERTY_C_OBJECT_REF)){ // non consecutive c node..
						THROW_RUNTIME_ERROR("C Functions should be added after global scope and consecutuve C scope node.");
				}
			}
		}


		if((symbol=scope->addSymbol(
				file
				,line
				,function_name
				,(char)args.size()
		))==NULL){
			return NULL;
		}

		short idx_script_function = script_functions->count;

		// sets local function information...
		symbol->idx_position = _idx_position; // idx local/member function
		symbol->properties = properties | SYMBOL_PROPERTY_IS_FUNCTION;

		ScriptFunction *script_function = new ScriptFunction(
				zs
				,idx_class
				,idx_script_function
				,args
				,idx_return_type
				,symbol
				,ref_native_function_ptr
		);

		symbol->ref_ptr = (intptr_t)script_function;		  // ptr function
		script_functions->push_back((intptr_t)script_function);
		return symbol;
	}

	void	ScriptFunctionFactory::setScriptFunction(short idx, ScriptFunction *sf){
		if(idx < 0 || (unsigned)idx >= script_functions->count){
			THROW_RUNTIME_ERROR("script function idx node out of bound");
		}

		script_functions->items[idx]=(intptr_t)sf;
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
					(info_function->symbol.properties&SYMBOL_PROPERTY_C_OBJECT_REF)!=SYMBOL_PROPERTY_C_OBJECT_REF
			){
				return false;
			}
		}

		return true;
	}

	void ScriptFunctionFactory::clear(){
		for(int v=script_functions->count-1;
				v >= 1; // avoid delete main function
				v--){

			ScriptFunction * info_function = (ScriptFunction * )script_functions->items[v];
			if((info_function->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF) != SYMBOL_PROPERTY_C_OBJECT_REF) //
			{
				script_functions->pop_back();
				delete info_function;
			}
			else{
				break;
			}
		}
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
