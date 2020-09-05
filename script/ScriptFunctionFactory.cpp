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
			, unsigned char idx_class
			, short _idx_position
			, const std::string & function_name
			, std::vector<FunctionParam> args
			, int idx_return_type
			, intptr_t ref_native_function_ptr
			, unsigned short symbol_properties
		){

		if((symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) == SYMBOL_PROPERTY_C_OBJECT_REF){
			if(script_functions->count > 1){ // if greather than 1 check if node consecutive...
				ScriptFunction *sc = (ScriptFunction *)script_functions->items[script_functions->count-1];
				if(!((sc->symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) == SYMBOL_PROPERTY_C_OBJECT_REF)){ // non consecutive c node..
						THROW_RUNTIME_ERROR("C Functions should be added after global scope and consecutuve C scope node.");
				}
			}
		}

		ScriptFunction *found_match=NULL;

		// check there's no collisions with same symbol and signature...
		for(unsigned i=0; i < script_functions->count && found_match==NULL;i++){
			ScriptFunction *function1=(ScriptFunction *)script_functions->items[i];

			if(function1->symbol.name == function_name){
				if(function1->params->count == args.size()){
					bool equal=true;
					for(unsigned p=0; p < function1->params->count && equal;p++){
						FunctionParam *fp1=(FunctionParam *)function1->params->items[p];
						FunctionParam *fp2=&args[p];
						equal = (fp1->arg_name == fp2->arg_name);
					}

					if(equal){
						found_match = function1;
					}
				}
			}
		}

		if(found_match!=NULL){
			THROW_RUNTIME_ERROR(" trying to register a C function \"%s\" at [%s:%i] was already registered at [%s:%i]"
					,function_name.c_str()
					,zs_path::get_file_name(file.c_str()).c_str()
					,line
					,zs_path::get_file_name(found_match->symbol.file.c_str()).c_str()
					,found_match->symbol.line
			);
		}

		Symbol *symbol=NULL;
		if((symbol=scope->registerNativeFunctionSymbol(
				file
				,line
				,function_name
				,args.size()
		))==NULL){
			return NULL;
		}

		short idx_script_function = script_functions->count;

		// sets local function information...
		symbol->idx_position = _idx_position; // idx local/member function
		symbol->symbol_properties = symbol_properties | SYMBOL_PROPERTY_IS_SCRIPT_FUNCTION;

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
		for(int v=script_functions->count-1;
				v >= 1; // avoid delete main function
				v--){

			ScriptFunction * info_function = (ScriptFunction * )script_functions->items[v];
			if((info_function->symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) != SYMBOL_PROPERTY_C_OBJECT_REF) //
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
