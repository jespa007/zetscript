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

	ScriptClass::ScriptClass(ZetScript *_zs,short _idx_class,Symbol *_symbol_class){

		//std::string error="";
		//Symbol *symbol_field_initializer=NULL;
		sf_field_initializer=NULL;

		str_class_ptr_type="";
		c_destructor = NULL;
		c_constructor=NULL;
		idx_function_member_constructor =ZS_IDX_UNDEFINED;
		idx_class=_idx_class;
		idx_starting_this_members=0;
		symbol_class=*_symbol_class;
		//memset(metamethod_operator,0,sizeof(metamethod_operator));

		/*for(unsigned i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
			metamethod_operator[i]=new zs_vector();
		}*/

		symbol_members=new zs_vector();
		symbol_members_allocated=new zs_vector();
//		symbol_members_static=new zs_vector();

		idx_base_classes=new zs_vector;

		// factories
		zs = _zs;
		scope_factory = zs->getScopeFactory();
		script_function_factory= zs->getScriptFunctionFactory();
		script_class_factory=zs->getScriptClassFactory();
		//static_constructor_destructor=false;
		sf_field_initializer=NULL; // will be created after register class and register member extension (if available)
		setter_getter=NULL;

	}

	ScriptClass * 				ScriptClass::getScriptClass(short idx){
		return script_class_factory->getScriptClass(idx);
	}


	short					ScriptClass::getIdxClassFromItsNativeType(const std::string & s){
		return script_class_factory->getIdxClassFromItsNativeType(s);
	}

	ScriptClass * 			ScriptClass::getScriptClassByNativeClassPtr(const std::string & class_type){
		return script_class_factory->getScriptClassByNativeClassPtr(class_type);
	}

	//---------------------------------------------------
	// VARIABLES
	Symbol				* 	ScriptClass::registerMemberVariable(
		const std::string & symbol_name
		,unsigned short symbol_properties
		,const char * file
		,short line
	){
		return registerInternalMemberVariable(
			symbol_name
			,symbol_properties
			,symbol_name
			,0
			,file
			,line
		);
	}

	Symbol				* 	ScriptClass::registerNativeMemberVariable(
		 const std::string & symbol_name
		,const std::string & str_native_type
		,zs_int ref_ptr
		,unsigned short symbol_properties
		,const char * file
		,short line
	){
		return registerInternalMemberVariable(
			symbol_name
			,symbol_properties
			,str_native_type
			,ref_ptr
			,file
			,line
		);
	}


	Symbol				* 	ScriptClass::registerInternalMemberVariable(
		const std::string & symbol_name
		,unsigned short symbol_properties
		,const std::string & str_native_type
		,zs_int ref_ptr
		,const char * file
		,short line

	){
		// ref_ptr: as natives is the inc pointer when object is created or stack_element pointer for static/const

		if(getSymbol(symbol_name)!=NULL){
			THROW_RUNTIME_ERROR("Variable \"%s\" already registered",symbol_name.c_str());
			return NULL;
		}

		/*if(((symbol_properties & (SYMBOL_PROPERTY_C_OBJECT_REF | SYMBOL_PROPERTY_CONST))==0) && ref_ptr==0){ // add
			//error=zs_strutils::format("Variable \"%s\" should registered as native or const",symbol_name.c_str());
			//return NULL;
		}*/

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
		symbol_members_allocated->push_back((zs_int)symbol);

		return symbol;
	}
	//---------------------------------------------------
	// ATTRIBUTES
	Symbol *ScriptClass::registerMemberAttribute(
			 const std::string & attrib_name
			,const char * file
			,short line


	){
		Symbol *symbol_attrib=NULL;
		if((symbol_attrib=getSymbol(attrib_name)) != NULL){ // give an error  ...
			//Symbol *existing_symbol;
			//if((existing_symbol=getSymbol(attrib_name)) != NULL){
			const char *what="is already defined";
			if((symbol_attrib->properties & SYMBOL_PROPERTY_MEMBER_ATTRIBUTE)==0){

				if(symbol_attrib->properties & SYMBOL_PROPERTY_MEMBER_FUNCTION){
					what="it conflicts with member function";
				}else{
					what="it conflicts with member variable";
				}
			}
			THROW_RUNTIME_ERROR(file,line,"Attribute \"%s\" %s at [%s:%i]"
				,attrib_name.c_str()
				,what
				,zs_path::get_filename(symbol_attrib->file).c_str()
				,symbol_attrib->line
			);
		}


		symbol_attrib = new Symbol();
		symbol_attrib->name=attrib_name;
		symbol_attrib->ref_ptr=(zs_int)(new MemberAttribute());
		symbol_attrib->properties=SYMBOL_PROPERTY_MEMBER_ATTRIBUTE;
		symbol_members->push_back((zs_int)symbol_attrib);
		symbol_members_allocated->push_back((zs_int)symbol_attrib);

		return symbol_attrib;
	}

	/*Symbol				* 	ScriptClass::registerMemberAttributeSetter(
		std::string & error
		,const char * file
		,short line
		,const std::string & attribute_name
		,ScriptFunction *sf // it's the offset from pointer or a pointer directly
	){


		return NULL;
	}*/

	Symbol				* 	ScriptClass::registerNativeMemberAttributeSetter(
			const std::string & attribute_name
			, ScriptFunctionArg arg_value
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_properties
			,const char * file
			,short line


	){
		return NULL;
		/*return registerInternalMemberFunction(
				error
				, file
				, line
				,function_name
				,args
				,symbol_properties
				,idx_return_type
				,(zs_int)ref_ptr
		);*/

	}

	/*Symbol				* 	ScriptClass::registerMemberAttributeGetter(
		const std::string & attribute_name
		,ScriptFunction *sf // it's the offset from pointer or a pointer directly
		,const char * file
		,short line

	){
		return NULL;
	}*/

	Symbol				* 	ScriptClass::registerNativeMemberAttributeGetter(
			 const std::string & attribute_name
			, int idx_return_type
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_properties
			,const char *file
			,short line

	){
		return NULL;
		/*return registerInternalMemberFunction(
				error
				, file
				, line
				,function_name
				,args
				,symbol_properties
				,idx_return_type
				,(zs_int)ref_ptr
		);*/

	}

	/*Symbol				* 	ScriptClass::registerInternalMemberAttributeSetter(
		std::string & error
		,const std::string & file
		,short line
		,const std::string & attribute_name
		,ScriptFunctionArg arg_value
		,zs_int ref_ptr // c function or symbol
		,unsigned short symbol_properties

	){
		return NULL;
	}

	Symbol				* 	ScriptClass::registerInternalMemberAttributeGetter(
		std::string & error
		,const std::string & file
		,short line
		,const std::string & attribute_name
		,zs_int ref // it's the offset from pointer or a pointer directly
		,int idx_return_type
		,zs_int ref_ptr // c function
		,unsigned short symbol_properties
	){
		return NULL;
	}*/

	//---------------------------------------------------
	// FUNCTIONS

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

	Symbol				* 	ScriptClass::registerMemberFunction(
			 const std::string & function_name
			, std::vector<ScriptFunctionArg> args
			, unsigned short symbol_properties
			,const char * file
			, short line



	){
		return registerInternalMemberFunction(
				 function_name
				, args
				, symbol_properties
				,ZS_IDX_UNDEFINED
				,0
				, file
				,  line

		);
	}

	Symbol				* 	ScriptClass::registerNativeMemberFunction(
			const std::string & function_name
			, std::vector<ScriptFunctionArg> args
			, int idx_return_type
			,zs_int ref_ptr // script function
			, unsigned short symbol_properties
			,const char * file
			, short line

	){
		return registerInternalMemberFunction(
				 function_name
				,args
				,symbol_properties
				,idx_return_type
				,(zs_int)ref_ptr
				, file
				, line

		);

	}

	Symbol * ScriptClass::registerInternalMemberFunction(
		 const std::string & function_name
		, std::vector<ScriptFunctionArg> params
		, unsigned short symbol_properties
		, int idx_return_type
		,zs_int ref_ptr
		, const char * file
		, short line


	){

		if((symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF)==0){ // we only allow repeated symbols on native functions...
			if(getSymbol(function_name,(char)params.size(),false) != NULL){ // we only search repeat symbols on this class ...
				Symbol *existing_symbol;
				if((existing_symbol=getSymbol(function_name, NO_PARAMS_SYMBOL_ONLY)) != NULL){
					THROW_RUNTIME_ERROR("Function \"%s\" is already defined at [%s:%i]"
						,function_name.c_str()
						,zs_path::get_filename(file).c_str()
						,line
						,zs_path::get_filename(existing_symbol->file).c_str()
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
				,ref_ptr // c function
				,symbol_properties|SYMBOL_PROPERTY_FUNCTION
		);

		// register num function symbols only for c symbols...
		if(function_symbol->properties & SYMBOL_PROPERTY_C_OBJECT_REF){
			Symbol *symbol_repeat=NULL;
			if((symbol_repeat=getSymbol(function_symbol->name,NO_PARAMS_SYMBOL_ONLY))!=NULL){ // there's one or more name with same args --> mark deduce at runtime
				((ScriptFunction *)symbol_repeat->ref_ptr)->symbol.properties|=SYMBOL_PROPERTY_DEDUCE_AT_RUNTIME;
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
				if(ZS_STRCMP(byte_code_metamethod_to_symbol_str((ByteCodeMetamethod)i),==,function_name.c_str())){
					ByteCodeMetamethod op=(ByteCodeMetamethod)i;
					const char *byte_code_metamethod_operator_str=byte_code_metamethod_to_operator_str(op);
					const char *str_symbol_metamethod=byte_code_metamethod_to_symbol_str(op);
					int min_args_static_metamethod=byte_code_get_num_arguments_static_metamethod(op); // expected params for static function, n_args -1 else

					Symbol *symbol_result;

					// can be one parameter or 0 params...
					if((symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF)==0){
						if((symbol_result=getSymbol(function_name,(char)params.size())) != NULL){
							THROW_RUNTIME_ERROR("Metamethod \"%s::%s\" is already defined at \"%s::%s\" (%s:%i). Metamethods cannot be override"
								,symbol_class.name.c_str()
								,function_name.c_str()
								,symbol_result->scope->script_class->symbol_class.name.c_str()
								,function_name.c_str()
								,zs_path::get_filename(symbol_result->file).c_str()
								,symbol_result->line
							);
							return NULL;
						}
					}

					// metamethod in script side are not static
					if(function_symbol->properties & SYMBOL_PROPERTY_STATIC){
						if(function_symbol->n_params<min_args_static_metamethod){
							THROW_RUNTIME_ERROR("Static metamethod %s (aka %s) should have at least %i parameter/s"
								,str_symbol_metamethod
								,byte_code_metamethod_operator_str
								,min_args_static_metamethod
							);

							return NULL;
						}
					}
					else {
						min_args_static_metamethod=get_num_arguments_non_static_metamethod(op);
						if(function_symbol->n_params< min_args_static_metamethod){ // non-static functions pass this object as first parameter
							THROW_RUNTIME_ERROR("Non static metamethod %s (aka %s) should have at least %i parameter/s"
								,str_symbol_metamethod
								,byte_code_metamethod_operator_str
								,min_args_static_metamethod
							);
							return NULL;
						}
					}

					// everything ok
					if(/*op==BYTE_CODE_METAMETHOD_GET || */op==BYTE_CODE_METAMETHOD_SET){
						if(setter_getter == NULL){
							setter_getter = new MemberAttribute();
						}

						/*if(op==BYTE_CODE_METAMETHOD_GET){ // getter
							if(setter_getter->getter==NULL){
								setter_getter->getter=(ScriptFunction *)function_symbol->ref_ptr;
							}else{
								// error already set
								error = zs_strutils::format("Getter \"%s::_get\" already set"
										,symbol_class.name.c_str()
								);
								return NULL;
							}
						}else*/{ // setter
							if(setter_getter->setters.count>0 && ((function_symbol->properties & SYMBOL_PROPERTY_C_OBJECT_REF)==0)){
								// error already set (script functions only can be set once)
								THROW_RUNTIME_ERROR("Setter \"%s::_set\" already set"
										,symbol_class.name.c_str()
								);
								return NULL;
							}

							setter_getter->setters.push_back(function_symbol->ref_ptr);
						}
					}

					break;
				}
			}
		}

		symbol_members->push_back((zs_int)function_symbol);

		return function_symbol;
	}
	//---------------------------------------------------------
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


	ScriptClass::~ScriptClass(){

		/*if ((symbol_class.properties & SYMBOL_PROPERTY_C_OBJECT_REF) == SYMBOL_PROPERTY_C_OBJECT_REF) {

			if(this->static_constructor_destructor == false){
				if (c_constructor !=NULL) {
					delete (std::function<void *()> *)c_constructor;
					c_constructor=NULL;
				}

				if (c_destructor != NULL) {
					delete (std::function<void (void *)> *)c_destructor;
					c_destructor=NULL;
				}
			}

		}*/

		for(unsigned i=0; i < symbol_members_allocated->count; i++){
			Symbol *symbol=(Symbol *)symbol_members_allocated->items[i];
			if(symbol->properties & SYMBOL_PROPERTY_MEMBER_ATTRIBUTE){
				delete (MemberAttribute *)symbol->ref_ptr;
			}
			delete symbol; // symbol variable member was created before
		}
		delete symbol_members_allocated;

		// delete symbol vector...
		delete symbol_members;
		symbol_members=NULL;


		delete idx_base_classes;

		if(setter_getter != NULL){
			delete setter_getter;
		}


	}
}



