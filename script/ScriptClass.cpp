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
		sf_field_initializer=NULL;

		str_class_ptr_type="";
		c_destructor = NULL;
		c_constructor=NULL;
		idx_function_member_constructor =ZS_IDX_UNDEFINED;
		idx_class=_idx_class;
		idx_starting_this_members=0;
		symbol_class=*_symbol_class;
		symbol_members=new zs_vector();
		symbol_members_allocated=new zs_vector();

		idx_base_classes=new zs_vector;

		// factories
		zs = _zs;
		scope_factory = zs->getScopeFactory();
		script_function_factory= zs->getScriptFunctionFactory();
		script_class_factory=zs->getScriptClassFactory();
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

		Symbol *symbol=new Symbol;

		// copy class symbol props...
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
			THROW_SCRIPT_ERROR_FILE_LINE(file,line,"Attribute \"%s\" %s at [%s:%i]"
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
		symbol_attrib->idx_position=symbol_members->count;
		symbol_members->push_back((zs_int)symbol_attrib);
		symbol_members_allocated->push_back((zs_int)symbol_attrib);

		return symbol_attrib;
	}

	Symbol				* 	ScriptClass::registerNativeSetterMemberAttribute(
			const std::string & attribute_name
			, std::vector<ScriptFunctionArg> arg_value
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_getter_function_properties
			,const char * file
			,short line


	){
		Symbol *symbol_attrib=NULL;
		MemberAttribute *ma=NULL;
		Symbol *symbol_function=NULL;

		if((symbol_attrib=getSymbol(attribute_name)) == NULL){
			symbol_attrib=registerMemberAttribute(attribute_name,file,line);
		}

		ma=(MemberAttribute *)symbol_attrib->ref_ptr;


		symbol_function=registerNativeMemberFunction(
				ZS_PREFIX_SYMBOL_NAME_SETTER+attribute_name,
				arg_value,
				IDX_BUILTIN_TYPE_VOID_C,
				ref_ptr,
				symbol_getter_function_properties,
				file,
				line
		);

		ma->setters.push_back((zs_int)symbol_function->ref_ptr);

		return symbol_attrib;
	}

	Symbol				* 	ScriptClass::registerNativeGetterMemberAttribute(
			 const std::string & attribute_name
			 , std::vector<ScriptFunctionArg> arg_value
			, int idx_return_type
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_getter_function_properties
			,const char *file
			,short line
	){

		Symbol *symbol_attrib=NULL;
		Symbol *symbol_function=NULL;

		MemberAttribute *ma=NULL;
		if((symbol_attrib=getSymbol(attribute_name)) == NULL){
			symbol_attrib=registerMemberAttribute(attribute_name,file,line);
		}

		ma=(MemberAttribute *)symbol_attrib->ref_ptr;

		if(ma->getter != NULL){

			THROW_SCRIPT_ERROR_FILE_LINE(file,line,"Attribute \"%s\" has already a getter"
				,attribute_name.c_str()
			);
		}

		symbol_function=registerNativeMemberFunction(
				ZS_PREFIX_SYMBOL_NAME_GETTER+attribute_name,
				arg_value,
				idx_return_type,
				ref_ptr,
				symbol_getter_function_properties,
				file,
				line
		);

		ma->getter=(ScriptFunction *)symbol_function->ref_ptr;

		return symbol_attrib;
	}

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
					int n_args_static_metamethod=byte_code_metamethod_get_num_arguments(op); // expected params for static function, n_args -1 else
					int this_arg=0;

					Symbol *symbol_result;

					// can be one parameter or 0 params...
					if(byte_code_metamethod_should_be_static(op) && ((symbol_properties & SYMBOL_PROPERTY_STATIC)==0)){
						THROW_RUNTIME_ERROR("Metamethod '%s::%s' has to be declared as static instead of member"
							,symbol_class.name.c_str()
							,function_name.c_str()
						);
						return NULL;
					}else if((byte_code_metamethod_should_be_static(op)==false) && ((symbol_properties & SYMBOL_PROPERTY_STATIC))){
						THROW_RUNTIME_ERROR("Metamethod '%s::%s' has to be declared as member instead of static"
							,symbol_class.name.c_str()
							,function_name.c_str()
						);
						return NULL;
					}

					if((symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF)==0){ // non-native

						if((symbol_result=getSymbol(function_name,(char)params.size())) != NULL){

							THROW_RUNTIME_ERROR("Metamethod '%s::%s' is already defined at '%s::%s' (%s:%i). Metamethods cannot be override"
								,symbol_class.name.c_str()
								,function_name.c_str()
								,symbol_result->scope->script_class->symbol_class.name.c_str()
								,function_name.c_str()
								,zs_path::get_filename(symbol_result->file).c_str()
								,symbol_result->line
							);
							return NULL;
						}
					}else{ // native
						if(op == BYTE_CODE_METAMETHOD_TO_STRING && !(idx_return_type == IDX_BUILTIN_TYPE_STRING_PTR_C || idx_return_type == IDX_BUILTIN_TYPE_STRING_C) ){
							THROW_RUNTIME_ERROR("Metamethod '%s::%s' should return std::string * or std::string *"
								,symbol_class.name.c_str()
								,function_name.c_str()
							);
							return NULL;
						}
					}


					this_arg=(symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF)&& ((symbol_properties & SYMBOL_PROPERTY_STATIC)==0)?1:0;

					if(function_symbol->n_params< (n_args_static_metamethod+this_arg)){ // non-static functions pass this object as first parameter
						THROW_RUNTIME_ERROR("%s metamethod '%s' (aka '%s') should have at least %i parameter/s"
							,(symbol_properties & SYMBOL_PROPERTY_STATIC)?"Static":"Member"
							,str_symbol_metamethod
							,byte_code_metamethod_operator_str
							,n_args_static_metamethod
						);
						return NULL;
					}

					// everything ok
					if(/*op==BYTE_CODE_METAMETHOD_GET || */op==BYTE_CODE_METAMETHOD_SET){
						if(setter_getter == NULL){
							setter_getter = new MemberAttribute();
						}

							if(setter_getter->setters.count>0 && ((function_symbol->properties & SYMBOL_PROPERTY_C_OBJECT_REF)==0)){
							// error already set (script functions only can be set once)
							THROW_RUNTIME_ERROR("Setter '%s::_set' already set"
									,symbol_class.name.c_str()
							);
							return NULL;
						}
						setter_getter->setters.push_back(function_symbol->ref_ptr);
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

		for(unsigned i=0; i < symbol_members_allocated->count; i++){
			Symbol *symbol=(Symbol *)symbol_members_allocated->items[i];
#ifdef __DEBUG__
			ZS_LOG_DEBUG("Deallocating member attribute '%s::%s'",this->symbol_class.name.c_str(),symbol->name.c_str());
#endif
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



