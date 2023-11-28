/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	FunctionFactory::FunctionFactory(ZetScript *_zs){
		zs = _zs;
		scope_factory = _zs->getScopeFactory();
		script_functions = new Vector<Function *>();
		idx_clear_checkpoint = 1; // by default MAIN function
	}

	Vector<Function *> 	*FunctionFactory::getFunctions(){
		return script_functions;
	}

	Symbol *		 FunctionFactory::newFunction(
			//--- Register data
			  Scope *_scope
			, const char * _file
			, short _line
			//--- Function data
			, TypeId _type_id
			, const String & _function_name
			, FunctionParam **_params
			,int _params_len
			, TypeId _return_type_id
			, zs_int _ref_native_function_ptr
			, unsigned short _function_properties
		){

		Symbol *symbol=NULL;
		uint16_t _symbol_check_repeated=_function_properties & FUNCTION_PROPERTY_NATIVE_OBJECT_REF? REGISTER_SCOPE_NO_CHECK_REPEATED_SYMBOLS:REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN;


		// try to not check if function name is constructor or is function member but different type scope
		if((_function_properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)){
			Symbol *p_irv = _scope->getSymbol(_function_name);
			Function *sf_repeaded=NULL;
			if(p_irv!=NULL && (p_irv->properties & SYMBOL_PROPERTY_FUNCTION)){
				sf_repeaded=(Function *)p_irv->ref_ptr;
				if((sf_repeaded->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION) && sf_repeaded->owner_type_id != _type_id){
					_symbol_check_repeated|=REGISTER_SCOPE_NO_CHECK_REPEATED_SYMBOLS;
				}
			}

			if(_function_name == zs->getTypeFactory()->getTypeName(_type_id)){
				_symbol_check_repeated|=REGISTER_SCOPE_NO_CHECK_CLASS_SYMBOLS;
			}
		}

		symbol=_scope->registerSymbolFunction(
				_file
				,_line
				,_function_name
				,_params_len
				,_symbol_check_repeated
		);

		if(_function_properties & FUNCTION_PROPERTY_STATIC){
			symbol->properties|=SYMBOL_PROPERTY_STATIC;
		}

		int id = script_functions->length();

		Function *script_function = new Function(
				zs
				,id
				,_type_id
				, symbol->idx_position
				,_function_name
				,_params
				,_params_len
				,_return_type_id
				,_ref_native_function_ptr
				,_function_properties
		);

		symbol->ref_ptr = (zs_int)script_function;		  // ptr function
		script_functions->push(script_function);
		return symbol;
	}

	void	FunctionFactory::setFunction(int idx, Function *sf){
		if(idx<0 || idx >= script_functions->length()){
			ZS_THROW_RUNTIME_ERRORF("script function idx node out of bound");
		}

		script_functions->set(idx,sf);
	}

	Function 	* FunctionFactory::getFunction(int idx){
		if(idx<0 || idx >= script_functions->length()){
			ZS_THROW_RUNTIME_ERRORF("script function idx node out of bound");
			return NULL;
		}
		return (Function 	*)script_functions->get(idx);
	}

	void FunctionFactory::clear(int _idx_start){

		int idx_start = _idx_start == ZS_UNDEFINED_IDX ?  idx_clear_checkpoint:_idx_start;

		for(
			int v=script_functions->length()-1;
			v > idx_start; // avoid delete main function
			v--
		){

			Function * info_function = (Function * )script_functions->get(v);
			delete info_function;
			script_functions->pop();

		}

		// clear unresolved symbols for main..
		if(_idx_start == ZS_UNDEFINED_IDX){
			((Function *)script_functions->get(0))->clearUnresolvedSymbols();
		}
	}

	void FunctionFactory::saveState(){
		idx_clear_checkpoint = script_functions->length()-1;
	}

	FunctionFactory::~FunctionFactory(){
		// erases all functions...
		for(int i = 0;i < script_functions->length();i++){
			Function * info_function = (Function *)script_functions->get(i);

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
}
