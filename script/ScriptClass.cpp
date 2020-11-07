/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	bool ScriptClass::isNativeClass(){

		 return ((symbol_class.properties & SYMBOL_PROPERTY_C_OBJECT_REF) != 0);
	}

	bool ScriptClass::isNativeSingletonClass(){
		 return ((symbol_class.properties & SYMBOL_PROPERTY_C_OBJECT_REF) != 0)
				 	 &&
				this->c_constructor == NULL;

	}
	//------------------------------------------------------------

	ScriptClass::ScriptClass(ZetScript *_zs,ClassTypeIdx _idx_class){

		str_class_ptr_type="";
		c_destructor = NULL;
		c_constructor=NULL;
		idx_function_member_constructor =ZS_IDX_UNDEFINED;
		idx_class=_idx_class;
		//idx_starting_this_members=0;

		for(unsigned i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
			metamethod_operator[i]=new zs_vector();
		}

		symbol_members=new zs_vector();
		symbol_members_built_in=new zs_vector();
		idx_base_classes=new zs_vector;


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
		,zs_int ref_ptr
		, unsigned short properties
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
		symbol->n_params=NO_PARAMS_SYMBOL_ONLY;
		symbol->ref_ptr=ref_ptr;
		symbol->name=symbol_name;
		symbol->str_native_type = str_native_type;
		symbol->properties=properties | SYMBOL_PROPERTY_C_OBJECT_REF;

		symbol_members->push_back((zs_int)symbol);
		symbol_members_built_in->push_back((zs_int)symbol);
		return symbol;

	}

	Symbol *	ScriptClass::getSuperFunctionSymbol(Symbol *symbol){

		if((symbol->properties & SYMBOL_PROPERTY_IS_FUNCTION) == 0){
			THROW_RUNTIME_ERROR("internal error: symbol is not a function");
		}

		for(int i = symbol->idx_position-1; i >=0; i--){
			Symbol *symbol_member = (Symbol *)symbol_members->items[i];
			if((symbol->name == symbol_member->name) && (symbol_member->properties & SYMBOL_PROPERTY_IS_FUNCTION)){
				return symbol_member;
			}
		}

		return NULL;
	}

	Symbol				* 	ScriptClass::getSymbol(const std::string & symbol_name, char n_params){
		bool only_symbol=n_params<0;

		for(int i = (int)(symbol_members->count-1); i >= 0 ; i--){
			Symbol *member_symbol=(Symbol *)symbol_members->items[i];
			if(member_symbol->name == symbol_name){
				if(only_symbol){
					return member_symbol;
				}
				if(member_symbol->properties & SYMBOL_PROPERTY_IS_FUNCTION){ // for C function symbols
					ScriptFunction *sf=(ScriptFunction *)member_symbol->ref_ptr;
					if((((int)n_params==sf->params->count) || (n_params==ANY_PARAMS_SYMBOL_ONLY))
					 ){
						return member_symbol;
					}
				}
			}
		}

		return NULL;
	}

	Symbol * ScriptClass::registerMemberFunction(
		const std::string & file
		, short line
		, const std::string & function_name
		, std::vector<FunctionParam> params
		, int idx_return_type
		,zs_int ref_ptr
		, unsigned short properties
	){

		if((properties & SYMBOL_PROPERTY_C_OBJECT_REF)==0){ // we only allow repeated symbols on native functions...
			if(getSymbol(function_name,(char)params.size()) != NULL){
				Symbol *existing_symbol;
				if((existing_symbol=getSymbol(function_name, NO_PARAMS_SYMBOL_ONLY)) != NULL){

					THROW_RUNTIME_ERROR("Function \"%s\" declared at [%s:%i] is already defined at [%s:%i]"
						,function_name.c_str()
						,zs_path::get_file_name(file.c_str()).c_str()
						,line
						,zs_path::get_file_name(existing_symbol->file.c_str()).c_str()
						,existing_symbol->line
					);
				}
				return NULL;
			}
		}

		int idx_position=symbol_members->count;

		Symbol *function_symbol =  script_function_factory->newScriptFunction(
				//---- Register data
				symbol_class.scope
				,file
				,line
				//---- Function data
				,idx_class 				// idx class which belongs to...
				,idx_position // idx position ...
				,function_name
				,params
				,idx_return_type
				,ref_ptr
				,properties|SYMBOL_PROPERTY_IS_FUNCTION
		);

		// register num function symbols only for c symbols...
		if(function_symbol->properties & SYMBOL_PROPERTY_C_OBJECT_REF){
			Symbol *symbol_repeat=NULL;
			if((symbol_repeat=getSymbol(function_symbol->name,NO_PARAMS_SYMBOL_ONLY))!=NULL){ // there's one or more name with same args --> mark
				((ScriptFunction *)symbol_repeat->ref_ptr)->symbol.properties|=SYMBOL_PROPERTY_DEDUCE_AT_RUNTIME; // mark the function found (only matters for first time)
				((ScriptFunction *)function_symbol->ref_ptr)->symbol.properties|=SYMBOL_PROPERTY_DEDUCE_AT_RUNTIME;
			}
		}

		// register
		symbol_members->push_back((zs_int)function_symbol);

		// constructor...
		if(function_name == FUNCTION_MEMBER_CONSTRUCTOR_NAME){
			idx_function_member_constructor = idx_position;
		}
		else{
			// check if metamethod...
			for(unsigned i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
				if(ZS_STRCMP(ByteCodeMetamethodToSymbolStr((ByteCodeMetamethod)i),==,function_name.c_str())){
					StackElement *stk_element = (StackElement *)malloc(sizeof(StackElement));
					*stk_element = {0,(void *)function_symbol->ref_ptr,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION};
					metamethod_operator[i]->push_back((zs_int)stk_element);
					std::string class_name = script_class_factory->getScriptClass(this->idx_class)->symbol_class.name;

					ZS_PRINT_DEBUG("Registered metamethod %s::%s",class_name.c_str(), function_name.c_str());
					break;
				}
			}
		}

		return function_symbol;
	}
	//-----
	ScriptClass::~ScriptClass(){

		if ((symbol_class.properties & SYMBOL_PROPERTY_C_OBJECT_REF) == SYMBOL_PROPERTY_C_OBJECT_REF) {

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

		delete idx_base_classes;

	}
}



