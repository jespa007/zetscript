/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

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

			symbol_members=new zs_vector();
			symbol_members_built_in=new zs_vector();
			//function_members=new zs_vector();
			idx_base_classes=new zs_vector();

			// factories
			zs = _zs;
			scope_factory = zs->getScopeFactory();
			script_function_factory= zs->getScriptFunctionFactory();
			script_class_factory=zs->getScriptClassFactory();

	}

		Symbol				* 	ScriptClass::registerNativeVariableMember(
				const std::string & file
				, short line
				,const std::string & symbol_name
				,const std::string & str_native_type
				,intptr_t ref_ptr
				, unsigned short symbol_properties
		){
			if(getSymbol(symbol_name)!=NULL){
				THROW_RUNTIME_ERROR("Variable \"%s\" already registered",symbol_name.c_str());
			}

			Symbol *symbol=new Symbol;

			// copy class symbol props...
			//*symbol=this->symbol;
			symbol->file=file;
			symbol->line=line;
			symbol->idx_position=symbol_members->count;
			symbol->n_params=NO_PARAMS_IS_VARIABLE;
			symbol->ref_ptr=ref_ptr;
			symbol->name=symbol_name;
			symbol->str_native_type = str_native_type;
			symbol->symbol_properties=symbol_properties | SYMBOL_PROPERTY_C_OBJECT_REF;

			symbol_members->push_back((intptr_t)symbol);
			symbol_members_built_in->push_back((intptr_t)symbol);
			return symbol;

		}

		/*Symbol *	 ScriptClass::getVariableMember(const std::string & symbol_name){
			// from lat value to first to get last override function...
			for(int i = (int)symbol_members->count-1; i >= 0 ; i--){
				Symbol *symbol=(Symbol *)symbol_members->items[i];
				if(symbol->name == symbol_name){
					return symbol;
				}
			}
			return NULL;
		}*/

		 Symbol				* 	ScriptClass::getSymbol(const std::string & symbol_name, char n_params){
			 bool is_function=n_params != NO_PARAMS_IS_VARIABLE;

			for(int i = (int)(symbol_members->count-1); i >= 0 ; i--){
				Symbol *symbol=(Symbol *)symbol_members->items[i];
				if(is_function){
					if(symbol->symbol_properties & SYMBOL_PROPERTY_IS_SCRIPT_FUNCTION){
						ScriptFunction *sf=(ScriptFunction *)symbol->ref_ptr;
						if(
							(symbol->name == symbol_name)
						 && (n_params == (int)sf->function_params->count)
						 ){

							return symbol;
						}
					}
				}else{ // other
					if(symbol->name == symbol_name){
						return symbol;
					}
				}
			}

			return NULL;
		 }

	Symbol * ScriptClass::registerFunctionMember(
			const std::string & file
			, short line
			, const std::string & function_name
			, std::vector<FunctionParam> function_params
			, int idx_return_type
			,intptr_t ref_ptr
			, unsigned short symbol_properties
		){

		if(getSymbol(function_name,(char)function_params.size()) != NULL){
			THROW_RUNTIME_ERROR("Function \"%s\" already exist",function_name.c_str());
			return NULL;
		}

		int idx_position=symbol_members->count;

		Symbol *function_symbol =  script_function_factory->newScriptFunction(
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
				,symbol_properties|SYMBOL_PROPERTY_IS_SCRIPT_FUNCTION
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

		// register
		symbol_members->push_back((intptr_t)function_symbol);
		//std::string class_name=symbol.name;
		//this->function_members->push_back((intptr_t)sf);

		// constructor...
		if(function_name == FUNCTION_MEMBER_CONSTRUCTOR_NAME){
			idx_function_member_constructor = idx_position;
		}
		else{
			// check if metamethod...
			for(unsigned i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
				if(ZS_STRCMP(ByteCodeMetamethodToStr((ByteCodeMetamethod)i),==,function_name.c_str())){
					StackElement *stk_element = (StackElement *)malloc(sizeof(StackElement));
					*stk_element = {0,(void *)function_symbol->ref_ptr,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION};
					metamethod_operator[i]->push_back((intptr_t)stk_element);

					ZS_PRINT_DEBUG("Registered metamethod %s::%s",class_name.c_str(), function_name.c_str());
					break;
				}
			}
		}
		return function_symbol;

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

		for(unsigned i=0; i < symbol_members_built_in->count; i++){
			Symbol *symbol=(Symbol *)symbol_members_built_in->items[i];
			delete symbol; // symbol variable member was created before
		}
		delete symbol_members_built_in;

		// delete symbol vector...
		delete symbol_members;
		symbol_members=NULL;

		for(unsigned i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
			zs_vector *vec=(zs_vector *)metamethod_operator[i];
			for(unsigned j = 0; j < vec->count; j++){
				free((void *)vec->items[j]);
			}

			delete vec;
		}
		memset(metamethod_operator,0,sizeof(metamethod_operator));


		// idx base classes only stores int (no free)
		delete idx_base_classes;
		idx_base_classes=NULL;

		// delete function members only stores pointers (no free)
		//delete function_members;
		//function_members=NULL;

	}
}



