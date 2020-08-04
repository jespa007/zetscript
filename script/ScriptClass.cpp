/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	void  writeError(const char *filename, int line, const  char  *string_text, ...);

	bool ScriptClass::is_C_Class(){

		 return ((symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) != 0);
	}
	//------------------------------------------------------------

	 ScriptClass::ScriptClass(ZetScript *_zs,unsigned char _idx_class){

			str_class_ptr_type="";
			c_destructor = NULL;
			c_constructor=NULL;
			idx_function_member_constructor =ZS_IDX_UNDEFINED;
			idx_class=_idx_class;

			for(unsigned i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
				metamethod_operator[i]=new zs_vector();
			}

			symbol_native_variable_members=new zs_vector();
			function_members=new zs_vector();
			idx_base_classes=new zs_vector();

			// factories
			zs = _zs;
			scope_factory = zs->getScopeFactory();
			script_function_factory= zs->getScriptFunctionFactory();
			script_class_factory=zs->getScriptClassFactory();

	}

	 ScriptFunction				* 	ScriptClass::getFunctionMember(const std::string & function_name, unsigned int n_args){
		 if(function_members->count>0){
			// from last value to first to get last override function...
			for(int i = (int)(function_members->count-1); i >= 0 ; i--){
				ScriptFunction *sf=(ScriptFunction *)function_members->items[i];
				if(
						(sf->symbol.name == function_name)
					 && (n_args == (int)sf->function_params->count)
					 ){

					return sf;
				}
			}
		}

		return NULL;
	 }

		Symbol				* 	ScriptClass::registerNativeSymbolVariableMember(
				const std::string & file
				, short line
				,const std::string & symbol_name
				,const std::string & c_type
				,intptr_t ref_ptr
				, unsigned short symbol_properties
		){
			if(get_C_SymbolVariableMember(symbol_name)==NULL){
				Symbol *symbol=new Symbol;

				// copy class symbol props...
				//*symbol=this->symbol;
				symbol->file=file;
				symbol->line=line;
				symbol->idx_position=symbol_native_variable_members->count;
				symbol->n_params=NO_PARAMS_IS_VARIABLE;
				symbol->ref_ptr=ref_ptr;
				symbol->name=symbol_name;
				symbol->c_type = c_type;
				symbol->symbol_properties=symbol_properties;

				symbol_native_variable_members->push_back((intptr_t)symbol);
				return symbol;
			}

			THROW_RUNTIME_ERROR(zs_strutils::format("Variable \"%s\" already registered",symbol_name.c_str()));
		}

		Symbol *	 ScriptClass::get_C_SymbolVariableMember(const std::string & symbol_name){
			// from lat value to first to get last override function...
			for(int i = (int)symbol_native_variable_members->count-1; i >= 0 ; i--){
				Symbol *symbol=(Symbol *)symbol_native_variable_members->items[i];
				if(symbol->name == symbol_name){
					return symbol;
				}
			}
			return NULL;
		}

	ScriptFunction * ScriptClass::registerFunctionMember(
			const std::string & file
			, short line
			, const std::string & function_name
			, std::vector<FunctionParam> function_params
			, int idx_return_type
			,intptr_t ref_ptr
			, unsigned short symbol_properties
		){

		if(getFunctionMember(function_name,(char)function_params.size()) != NULL){
			THROW_RUNTIME_ERROR(zs_strutils::format("Function \"%s\" already exist",function_name.c_str()));
			return NULL;
		}

		int idx_position=function_members->count;

		ScriptFunction *sf =  script_function_factory->newScriptFunction(
				//---- Register data
				symbol.scope
				,file
				,line
				//---- Function data
				,idx_class 				// idx class which belongs to...
				,idx_position // idx position ...
				,function_name
				,function_params
				,idx_return_type
				,ref_ptr
				,symbol_properties
		);/*registerFunction(
				 this->symbol.scope
				, file
				, line
				, function_name
				, args
				, idx_return_type
				, ref_ptr
				, symbol_properties
		);*/

		//std::string class_name=symbol.name;
		this->function_members->push_back((intptr_t)sf);

		// constructor...
		if(function_name == FUNCTION_MEMBER_CONSTRUCTOR_NAME){
			idx_function_member_constructor = idx_position;
		}
		else{
			// check if metamethod...
			for(unsigned i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
				if(ZS_STRCMP(ByteCodeMetamethodToStr((ByteCodeMetamethod)i),==,function_name.c_str())){

					metamethod_operator[i]->push_back((intptr_t)sf);

					ZS_PRINT_DEBUG("Registered metamethod %s::%s",class_name.c_str(), function_name.c_str());
					break;
				}
			}
		}
		return sf;

	}
	//-----

	ScriptClass::~ScriptClass(){

		if ((symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) == SYMBOL_PROPERTY_C_OBJECT_REF) {

			if (c_constructor !=NULL) {
				delete c_constructor;
				c_constructor=NULL;
			}

			if (c_destructor != NULL) {
				delete c_destructor;
				c_destructor=NULL;
			}

		}

		// delete symbol native variable members...
		for(unsigned i = 0; i < symbol_native_variable_members->count; i++){
			delete (Symbol *)symbol_native_variable_members->items[i];
		}
		delete symbol_native_variable_members;
		symbol_native_variable_members=NULL;

		for(unsigned i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
			zs_vector *vec=(zs_vector *)metamethod_operator[i];
			delete vec;
		}
		memset(metamethod_operator,0,sizeof(metamethod_operator));


		// idx base classes only stores int (no free)
		delete idx_base_classes;
		idx_base_classes=NULL;

		// delete function members only stores pointers (no free)
		delete function_members;
		function_members=NULL;

	}
}



