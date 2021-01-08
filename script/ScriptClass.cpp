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

	ScriptClass::ScriptClass(ZetScript *_zs,int _idx_class){

		str_class_ptr_type="";
		c_destructor = NULL;
		c_constructor=NULL;
		idx_function_member_constructor =ZS_IDX_UNDEFINED;
		idx_class=_idx_class;
		idx_starting_this_members=0;
		//memset(metamethod_operator,0,sizeof(metamethod_operator));

		/*for(unsigned i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
			metamethod_operator[i]=new zs_vector();
		}*/

		symbol_members=new zs_vector();
		symbol_members_built_in=new zs_vector();
//		symbol_members_static=new zs_vector();

		idx_base_classes=new zs_vector;

		// factories
		zs = _zs;
		scope_factory = zs->getScopeFactory();
		script_function_factory= zs->getScriptFunctionFactory();
		script_class_factory=zs->getScriptClassFactory();

	}

	Symbol				* 	ScriptClass::registerMemberVariable(
		std::string & error
		,const std::string & file
		,short line
		,const std::string & symbol_name

		,unsigned short symbol_properties

	){
		return registerInternalMemberVariable(
			error
			,file
			,line
			,symbol_name
			,symbol_properties
			,symbol_name
		);
	}

	Symbol				* 	ScriptClass::registerNativeMemberVariable(
		std::string & error
		,const std::string & file
		,short line
		,const std::string & symbol_name
		,const std::string & str_native_type
		,zs_int ref_ptr
		,unsigned short symbol_properties


	){
		return registerInternalMemberVariable(
			error
			,file
			,line
			,symbol_name
			,symbol_properties
			,str_native_type
			,ref_ptr
		);
	}

	Symbol				* 	ScriptClass::registerInternalMemberVariable(
	    std::string & error
		,const std::string & file
		,short line
		,const std::string & symbol_name
		,unsigned short symbol_properties
		,const std::string & str_native_type
		,zs_int ref_ptr
	){
		// ref_ptr: as natives is the inc pointer when object is created or stack_element pointer for static/const

		if(getSymbol(symbol_name)!=NULL){
			error=zs_strutils::format("Variable \"%s\" already registered",symbol_name.c_str());
			return NULL;
		}

		if((symbol_properties & (SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_CONST))==0){
			error=zs_strutils::format("Variable \"%s\" should registered as native or const",symbol_name.c_str());
			return NULL;
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
		symbol->properties=symbol_properties;

		symbol_members->push_back((zs_int)symbol);
		symbol_members_built_in->push_back((zs_int)symbol);
		return symbol;

	}

	Symbol *	ScriptClass::getSuperFunctionSymbol(Symbol *symbol){

		if((symbol->properties & SYMBOL_PROPERTY_FUNCTION) == 0){
			THROW_RUNTIME_ERROR("internal error: symbol is not a function");
		}

		for(int i = symbol->idx_position-1; i >=0; i--){
			Symbol *symbol_member = (Symbol *)symbol_members->items[i];
			if((symbol->name == symbol_member->name) && (symbol_member->properties & SYMBOL_PROPERTY_FUNCTION)){
				return symbol_member;
			}
		}

		return NULL;
	}

	Symbol				* 	ScriptClass::getSymbol(const std::string & symbol_name, char n_params,bool include_inherited_symbols){
		bool only_symbol=n_params<0;
		int idx_end=include_inherited_symbols==true?0:idx_starting_this_members;

		for(
				int i = (int)(symbol_members->count-1);
				i >= idx_end
				; i--
		){

			Symbol *member_symbol=(Symbol *)symbol_members->items[i];
			if(member_symbol->name == symbol_name){
				if(only_symbol){
					return member_symbol;
				}
				if(member_symbol->properties & SYMBOL_PROPERTY_FUNCTION){ // for C function symbols
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

	Symbol				* 	ScriptClass::registerMemberFunction(
			 std::string & error
			,const std::string & file
			, short line
			,const std::string & function_name
			, std::vector<FunctionParam> args
			, unsigned short symbol_properties


	){
		return registerInternalMemberFunction(
				 error
				, file
				,  line
				,function_name
				, args
				, symbol_properties

		);
	}

	Symbol				* 	ScriptClass::registerNativeMemberFunction(
			std::string & error
			,const std::string & file
			, short line
			,const std::string & function_name
			, std::vector<FunctionParam> args
			, int idx_return_type
			,zs_int ref_ptr
			, unsigned short symbol_properties

	){
		return registerInternalMemberFunction(
				error
				, file
				, line
				,function_name
				,args
				,symbol_properties
				,idx_return_type
				,ref_ptr
		);

	}

	Symbol * ScriptClass::registerInternalMemberFunction(
		std::string & error
		, const std::string & file
		, short line
		, const std::string & function_name
		, std::vector<FunctionParam> params
		, unsigned short symbol_properties
		, int idx_return_type
		,zs_int ref_ptr

	){

		if((symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF)==0){ // we only allow repeated symbols on native functions...
			if(getSymbol(function_name,(char)params.size(),false) != NULL){ // we only search repeat symbols on this class ...
				Symbol *existing_symbol;
				if((existing_symbol=getSymbol(function_name, NO_PARAMS_SYMBOL_ONLY)) != NULL){
					error=zs_strutils::format("Function \"%s\" is already defined at [%s:%i]"
						,function_name.c_str()
						,zs_path::get_filename(file.c_str()).c_str()
						,line
						,zs_path::get_filename(existing_symbol->file.c_str()).c_str()
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
				,symbol_properties|SYMBOL_PROPERTY_FUNCTION
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


		// constructor...
		if(function_name == this->symbol_class.name){ //  FUNCTION_MEMBER_CONSTRUCTOR_NAME
			idx_function_member_constructor = idx_position;
		}
		else{
			// check metamethod function...
			for(unsigned i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
				if(ZS_STRCMP(ByteCodeMetamethodToSymbolStr((ByteCodeMetamethod)i),==,function_name.c_str())){
					ByteCodeMetamethod op=(ByteCodeMetamethod)i;
					const char *byte_code_metamethod_operator_str=ByteCodeMetamethodToOperatorStr(op);
					const char *str_symbol_metamethod=ByteCodeMetamethodToSymbolStr(op);
					int min_args_static_metamethod=getNumArgumentsStaticMetamethod(op); // expected params for static function, n_args -1 else

					Symbol *symbol_result;

					// can be one parameter or 0 params...
					if((symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF)==0){
						if((symbol_result=getSymbol(function_name,(char)params.size())) != NULL){
							error = zs_strutils::format("Metamethod \"%s::%s\" is already defined at \"%s::%s\" (%s:%i). Metamethods cannot be override"
								,symbol_class.name.c_str()
								,function_name.c_str()
								,symbol_result->scope->script_class->symbol_class.name.c_str()
								,function_name.c_str()
								,zs_path::get_filename(symbol_result->file.c_str()).c_str()
								,symbol_result->line
							);
							return NULL;
						}
					}

					// metamethod in script side are not static
					if(function_symbol->properties & SYMBOL_PROPERTY_STATIC){
						if(function_symbol->n_params<min_args_static_metamethod){
							error = zs_strutils::format("Static metamethod %s (aka %s) should have at least %i parameter/s"
								,str_symbol_metamethod
								,byte_code_metamethod_operator_str
								,min_args_static_metamethod
							);

							return NULL;
						}
					}
					else {
						min_args_static_metamethod=getNumArgumentsNonStaticMetamethod(op);
						if(function_symbol->n_params< min_args_static_metamethod){ // non-static functions pass this object as first parameter
							error = zs_strutils::format("Non static metamethod %s (aka %s) should have at least %i parameter/s"
								,str_symbol_metamethod
								,byte_code_metamethod_operator_str
								,min_args_static_metamethod
							);
							return NULL;
						}
					}
					break;
				}
			}
		}

		symbol_members->push_back((zs_int)function_symbol);

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

		/*for(unsigned i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
			zs_vector *vec=(zs_vector *)metamethod_operator[i];
			if(vec != NULL){
				for(unsigned j = 0; j < vec->count; j++){
					free((void *)vec->items[j]);
				}
				delete vec;
			}
		}
		memset(metamethod_operator,0,sizeof(metamethod_operator));*/

		delete idx_base_classes;

	}
}



