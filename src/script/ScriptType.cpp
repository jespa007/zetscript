/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	bool ScriptType::isNativeType(){

		 return ((properties & SCRIPT_TYPE_PROPERTY_C_OBJECT_REF) != 0);
	}

	bool ScriptType::isNonInstantiable(){
		 return ((properties & SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE) != 0);
	}
	//------------------------------------------------------------

	ScriptType::ScriptType(ZetScript *_zs
			,short _idx_script_type
			, zs_string _class_name
			, Scope *_class_scope
			,const char *_str_class_ptr_type
			,uint16_t _properties){
		sf_field_initializer=NULL;

		script_type_name_ptr="";
		c_destructor = NULL;
		c_constructor=NULL;
		idx_function_member_constructor = ZS_IDX_UNDEFINED;
		idx_script_type=_idx_script_type;
		idx_starting_this_member_variables=0;
		idx_starting_this_member_functions=0;
		script_type_name=_class_name;
		script_type_scope=_class_scope;
		script_type_name_ptr=_str_class_ptr_type;
		allocated_member_properties=new zs_vector();

		idx_base_types=new zs_vector;

		// factories
		zs = _zs;
		scope_factory = zs->getScopeFactory();
		script_function_factory= zs->getScriptFunctionFactory();
		script_type_factory=zs->getScriptTypeFactory();
		sf_field_initializer=NULL; // will be created after register class and register member extension (if available)
		properties=_properties;

	}

	bool ScriptType::isDerivedFrom(short _idx_script_type){
		if(_idx_script_type==this->idx_script_type){
			return true;
		}

		for(int i=0; i < this->idx_base_types->count; i++){
			if(this->idx_base_types->items[i]==_idx_script_type){
				return true;
			}
		}

		return false;
	}

	ScriptType * 				ScriptType::getScriptType(short idx){
		return script_type_factory->getScriptType(idx);
	}


	short					ScriptType::getIdxScriptTypeFromTypeNamePtr(const char * s){
		return script_type_factory->getIdxScriptTypeFromTypeNamePtr(s);
	}

	//---------------------------------------------------
	//
	// MEMBER VARIABLES
	//
	Symbol				* 	ScriptType::registerMemberVariable(
		const zs_string & symbol_name
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

	Symbol				* 	ScriptType::bindMemberVariable(
		 const zs_string & symbol_name
		,const zs_string & str_native_type
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

	Symbol				* 	ScriptType::registerInternalMemberVariable(
		const zs_string & symbol_name
		,unsigned short symbol_properties
		,const zs_string & str_native_type
		,zs_int ref_ptr
		,const char * file
		,short line

	){
		if(getSymbol(symbol_name)!=NULL){
			THROW_RUNTIME_ERROR("Variable '%s' already registered",symbol_name.c_str());
			return NULL;
		}

		Symbol *symbol=script_type_scope->registerSymbolVariable(
				file
				,line
				,symbol_name
		);

		symbol->ref_ptr=ref_ptr;
		symbol->str_native_type = str_native_type;
		symbol->properties=symbol_properties;
		return symbol;
	}
	//---------------------------------------------------
	//
	// MEMBER PROPERTIES
	//
	Symbol *ScriptType::registerMemberProperty(
			 const zs_string & attrib_name
			,const char * file
			,short line
	){
		Symbol *symbol_member_property=NULL;
		if((symbol_member_property=getSymbol(attrib_name)) != NULL){ // give an error  ...

			const char *what="is already defined";
			if((symbol_member_property->properties & SYMBOL_PROPERTY_MEMBER_PROPERTY)==0){

				if(symbol_member_property->properties & SYMBOL_PROPERTY_FUNCTION){
					what="it conflicts with member function";
				}else{
					what="it conflicts with member variable";
				}
			}
			THROW_SCRIPT_ERROR_FILE_LINE(file,line,"Property '%s' %s at [%s:%i]"
				,attrib_name.c_str()
				,what
				,zs_path::get_filename(symbol_member_property->file).c_str()
				,symbol_member_property->line
			);
		}

		symbol_member_property = script_type_scope->registerSymbolVariable(file,line,attrib_name);
		symbol_member_property->ref_ptr=(zs_int)(new MemberProperty(this,attrib_name));
		symbol_member_property->properties=SYMBOL_PROPERTY_MEMBER_PROPERTY;
		allocated_member_properties->push_back(symbol_member_property->ref_ptr);

		return symbol_member_property;
	}

	Symbol				* 	ScriptType::bindMemberPropertyMetamethod(
			const zs_string & _property_name
			,ByteCodeMetamethod _byte_code_metamethod
			,ScriptFunctionParam **_params
			,int _params_len
			,int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,const char * _file
			,short _line
	){
		Symbol *symbol_member_property=NULL;
		MemberProperty *mp=NULL;
		MetamethodMemberSetterInfo mp_setter_info;
		Symbol *symbol_function=NULL;
		zs_string symbol_metamethod_function;
		Symbol **ptr_getter_script_function=NULL;

		if((symbol_member_property=getSymbol(_property_name)) == NULL){
			symbol_member_property=registerMemberProperty(_property_name,_file,_line);
		}

		mp=(MemberProperty *)symbol_member_property->ref_ptr;


		if(MetamethodMembers::isMetamethodMember(_byte_code_metamethod)==false){
			THROW_SCRIPT_ERROR_FILE_LINE(_file,_line,"Invalid metamethod '%s' for property '%s'"
				,byte_code_metamethod_to_symbol_str(_byte_code_metamethod)
				,_property_name.c_str()
			);
			symbol_metamethod_function=zs_string(byte_code_metamethod_to_symbol_str(_byte_code_metamethod))+"@"+_property_name; //byte_code_metamethod_to_symbol_str(_byte_code_metamethod)+"@"+_property_name;
		}
		//zs_string member_property_metamethod_name=byte_code_metamethod_to_symbol_str(_metamethod)+"@"+_property_name;
		switch(_byte_code_metamethod){
		case BYTE_CODE_METAMETHOD_SET:
		case BYTE_CODE_METAMETHOD_ADD_SET:
		case BYTE_CODE_METAMETHOD_SUB_SET:
		case BYTE_CODE_METAMETHOD_MUL_SET:
		case BYTE_CODE_METAMETHOD_DIV_SET:
		case BYTE_CODE_METAMETHOD_MOD_SET:
		case BYTE_CODE_METAMETHOD_AND_SET:
		case BYTE_CODE_METAMETHOD_OR_SET:
		case BYTE_CODE_METAMETHOD_XOR_SET:
		case BYTE_CODE_METAMETHOD_SHL_SET:
		case BYTE_CODE_METAMETHOD_SHR_SET:
			symbol_metamethod_function=ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_SETTER(_byte_code_metamethod,_property_name);
			mp_setter_info=mp->metamethod_members.getSetterInfo(_byte_code_metamethod);
			break;
		// particular case
		case BYTE_CODE_METAMETHOD_POST_INC:
			if(mp->metamethod_members.post_inc != NULL){

				THROW_SCRIPT_ERROR_FILE_LINE(_file,_line,"Property '%s' has already a post increment (aka '%s++') metamethod"
					,_property_name.c_str()
					,_property_name.c_str()
				);
			}
			ptr_getter_script_function=&mp->metamethod_members.post_inc;
			break;
		case BYTE_CODE_METAMETHOD_POST_DEC:
			if(mp->metamethod_members.post_dec != NULL){

				THROW_SCRIPT_ERROR_FILE_LINE(_file,_line,"Property '%s' has already a post decrement (aka '%s--') metamethod"
					,_property_name.c_str()
					,_property_name.c_str()
				);
			}
			ptr_getter_script_function=&mp->metamethod_members.post_dec;
			break;
		case BYTE_CODE_METAMETHOD_PRE_INC:
			if(mp->metamethod_members.pre_inc != NULL){

				THROW_SCRIPT_ERROR_FILE_LINE(_file,_line,"Property '%s' has already a pre increment (aka '++%s') metamethod"
					,_property_name.c_str()
					,_property_name.c_str()
				);
			}
			ptr_getter_script_function=&mp->metamethod_members.pre_inc;
			break;
		case BYTE_CODE_METAMETHOD_PRE_DEC:
			if(mp->metamethod_members.pre_dec != NULL){

				THROW_SCRIPT_ERROR_FILE_LINE(_file,_line,"Property '%s' has already a pre decrement (aka '--%s') metamethod"
					,_property_name.c_str()
					,_property_name.c_str()
				);
			}
			ptr_getter_script_function=&mp->metamethod_members.pre_dec;
			break;
		}

		symbol_function=registerMemberFunction(
				symbol_metamethod_function,
				_params,
				_params_len,
				FUNCTION_PROPERTY_C_OBJECT_REF | FUNCTION_PROPERTY_MEMBER_FUNCTION,
				_idx_return_type,
				_ref_ptr,
				_file,
				_line
		);

		if(ptr_getter_script_function!=NULL){ // getter
			*ptr_getter_script_function=symbol_function;
		}else{ // setter
			mp->metamethod_members.addSetter(_byte_code_metamethod,symbol_function);
			//mp_setter_info.setters->push_back(symbol_function->ref_ptr);
		}

		return symbol_member_property;
	}


	Symbol				* 	ScriptType::bindMemberPropertyMetamethodGetter(
			 const zs_string & _property_name
			 ,ScriptFunctionParam **_params
			 ,char _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,const char *_file
			,short _line
	){

		Symbol *symbol_member_property=NULL;
		Symbol *symbol_function=NULL;

		MemberProperty *mp=NULL;
		if((symbol_member_property=getSymbol(_property_name)) == NULL){
			symbol_member_property=registerMemberProperty(_property_name,_file,_line);
		}

		mp=(MemberProperty *)symbol_member_property->ref_ptr;

		if(mp->metamethod_members.getter != NULL){

			THROW_SCRIPT_ERROR_FILE_LINE(_file,_line,"Property '%s' has already a getter"
				,_property_name.c_str()
			);
		}

		symbol_function=registerMemberFunction(
				ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_GETTER+_property_name,
				_params,
				_params_len,
				FUNCTION_PROPERTY_C_OBJECT_REF | FUNCTION_PROPERTY_MEMBER_FUNCTION,
				_idx_return_type,
				_ref_ptr,
				_file,
				_line
		);

		mp->metamethod_members.getter=symbol_function;

		return symbol_member_property;
	}


	//---------------------------------------------------
	//
	// MEMBER FUNCTIONS
	//

	Symbol * ScriptType::registerMemberFunction(
		 const zs_string & _function_name
		 , ScriptFunctionParam **_params
		 ,int _params_len
		, unsigned short _function_properties
		, int _idx_return_type
		,zs_int _ref_ptr
		, const char * _file
		, short _line
	){

		if((_function_properties & FUNCTION_PROPERTY_C_OBJECT_REF)==0){ // we only allow repeated symbols on native functions...

			if(getSymbol(_function_name,(char)_params_len,false) != NULL){ // we only search repeat symbols on this class ...
				Symbol *existing_symbol;
				if((existing_symbol=getSymbol(_function_name, NO_PARAMS_SYMBOL_ONLY)) != NULL){
					THROW_RUNTIME_ERROR("Function member '%s' is already defined at [%s:%i]"
						,_function_name.c_str()
						//,zs_path::get_filename(_file).c_str()
						//,_line
						,zs_path::get_filename(existing_symbol->file).c_str()
						,existing_symbol->line
					);
				}
				return NULL;
			}
		}

		Symbol *symbol_function =  script_function_factory->newScriptFunction(
				//---- Register data
				this->script_type_scope
				,_file
				,_line
				//---- Function data
				,idx_script_type 				// idx class which belongs to...
				,_function_name
				,_params
				,_params_len
				,_idx_return_type
				,_ref_ptr // c function
				,_function_properties
		);

		ScriptFunction *sf_current=(ScriptFunction *)symbol_function->ref_ptr;

		// register num function symbols only for c symbols...
		if(sf_current->properties & FUNCTION_PROPERTY_C_OBJECT_REF){
			Symbol *symbol_repeat=NULL;
			if((symbol_repeat=this->getSymbolMemberFunction(_function_name,NO_PARAMS_SYMBOL_ONLY))!=NULL){ // there's one or more name with same args --> mark deduce at runtime
				ScriptFunction *sf_repeat=(ScriptFunction *)symbol_repeat->ref_ptr;

				sf_repeat->properties|=FUNCTION_PROPERTY_DEDUCE_AT_RUNTIME;
				sf_current->properties|=FUNCTION_PROPERTY_DEDUCE_AT_RUNTIME;
			}
		}

		// constructor...
		if(_function_name == CONSTRUCTOR_FUNCTION_NAME){ //  FUNCTION_MEMBER_CONSTRUCTOR_NAME
			idx_function_member_constructor = symbol_function->idx_position;
		}
		else{
			// check metamethod function...
			for(unsigned i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
				if(ZS_STRCMP(byte_code_metamethod_to_symbol_str((ByteCodeMetamethod)i),==,_function_name.c_str())){
					// check whether function meets the conditions of num params, static etc
					MetamethodMemberSetterInfo info_mp;
					ByteCodeMetamethod op=(ByteCodeMetamethod)i;
					const char *byte_code_metamethod_operator_str=byte_code_metamethod_to_operator_str(op);
					const char *str_symbol_metamethod=byte_code_metamethod_to_symbol_str(op);
					int n_args_static_metamethod=byte_code_metamethod_get_num_arguments(op); // expected params for static function, n_args -1 else
					int this_arg=0;

					// can be one parameter or 0 params...
					if(byte_code_metamethod_should_be_static(op) && ((_function_properties & FUNCTION_PROPERTY_STATIC)==0)){
						THROW_RUNTIME_ERROR("Metamethod '%s::%s' has to be declared as static instead of member"
							,script_type_name.c_str()
							,_function_name.c_str()
						);
						return NULL;
					}else if((byte_code_metamethod_should_be_static(op)==false) && ((_function_properties & FUNCTION_PROPERTY_STATIC))){
						THROW_RUNTIME_ERROR("Metamethod '%s::%s' has to be declared as member instead of static"
							,script_type_name.c_str()
							,_function_name.c_str()
						);
						return NULL;
					}

					// native
					if((_function_properties & FUNCTION_PROPERTY_C_OBJECT_REF)){ // if-native
						if(op == BYTE_CODE_METAMETHOD_TO_STRING && !(_idx_return_type == IDX_TYPE_STRING_PTR_C || _idx_return_type == IDX_TYPE_STRING_C) ){
							THROW_RUNTIME_ERROR("Metamethod '%s::%s' should return zs_string * or zs_string *"
								,script_type_name.c_str()
								,_function_name.c_str()
							);
							return NULL;
						}

						if(_function_properties & FUNCTION_PROPERTY_STATIC){

							// check if they are gte,gt,equ, not_equ, lt, lte
							switch(i){
							case BYTE_CODE_METAMETHOD_EQU: //STRCMP(name_script_function, == ,"_equ")
							case BYTE_CODE_METAMETHOD_NOT_EQU: //STRCMP(name_script_function, ==, "_nequ")
							case BYTE_CODE_METAMETHOD_LT://STRCMP(name_script_function, ==, "_lt")
							case BYTE_CODE_METAMETHOD_LTE://STRCMP(name_script_function, ==, "_lte")
							case BYTE_CODE_METAMETHOD_GT://STRCMP(name_script_function, ==, "_gt")
							case BYTE_CODE_METAMETHOD_GTE://STRCMP(name_script_function, ==, "_gte")
							case BYTE_CODE_METAMETHOD_NOT://STRCMP(name_script_function, ==, "_gte")

								// return type must be bool...
								if(_idx_return_type != IDX_TYPE_BOOL_C){
									THROW_RUNTIME_ERROR("error registering metamethod '%s::%s'. Expected return bool but it was '%s'",
											this->script_type_name.c_str(),
											_function_name.c_str(),
											zs_rtti::demangle(this->script_type_factory->getScriptType(_idx_return_type)->script_type_name_ptr).c_str()
									);
									return NULL;
								}
								break;
							case BYTE_CODE_METAMETHOD_ADD: // +
							case BYTE_CODE_METAMETHOD_SUB: // -
							case BYTE_CODE_METAMETHOD_DIV: // /
							case BYTE_CODE_METAMETHOD_MUL: // *
							case BYTE_CODE_METAMETHOD_MOD:  // %
							case BYTE_CODE_METAMETHOD_AND: // & bitwise logic and
							case BYTE_CODE_METAMETHOD_OR: // | bitwise logic or
							case BYTE_CODE_METAMETHOD_XOR: // ^ logic xor
							case BYTE_CODE_METAMETHOD_SHL: // << shift left
							case BYTE_CODE_METAMETHOD_SHR: // >> shift right

								if(ZS_STRCMP(this->script_type_factory->getScriptType(_idx_return_type)->script_type_name_ptr, != ,this->script_type_name_ptr)){

									THROW_RUNTIME_ERROR("error registering metamethod %s::%s. Expected return %s but it was %s",
											this->script_type_name.c_str(),
											_function_name.c_str(),
											zs_rtti::demangle(this->script_type_factory->getScriptType(_idx_return_type)->script_type_name_ptr).c_str()
									);
									return NULL;
								}
								break;
							default:
								break;
							}



						}

						if(_function_properties & FUNCTION_PROPERTY_MEMBER_FUNCTION){
							this_arg=1;
						}
					}

					if(symbol_function->n_params< (n_args_static_metamethod+this_arg)){ // non-static functions pass this object as first parameter
						THROW_RUNTIME_ERROR("%s metamethod '%s' (aka '%s') should have at least %i parameter/s"
							,(_function_properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)?"Member":"Static"
							,str_symbol_metamethod
							,byte_code_metamethod_operator_str
							,n_args_static_metamethod
						);
						return NULL;
					}


					// in the class only setters are registered in member property variable (getter is ignored)
					switch(op){
					case BYTE_CODE_METAMETHOD_SET:
					case BYTE_CODE_METAMETHOD_ADD_SET:
					case BYTE_CODE_METAMETHOD_SUB_SET:
					case BYTE_CODE_METAMETHOD_MUL_SET:
					case BYTE_CODE_METAMETHOD_DIV_SET:
					case BYTE_CODE_METAMETHOD_MOD_SET:
					case BYTE_CODE_METAMETHOD_AND_SET:
					case BYTE_CODE_METAMETHOD_OR_SET:
					case BYTE_CODE_METAMETHOD_XOR_SET:
					case BYTE_CODE_METAMETHOD_SHL_SET:
					case BYTE_CODE_METAMETHOD_SHR_SET:

						info_mp=metamethod_members.getSetterInfo(op);

						if(((_function_properties & FUNCTION_PROPERTY_C_OBJECT_REF)==0) //--> script function has to have one setter function, meanwhile c ref can have more than one (due different signatures)
								&&
							(info_mp.setters!=NULL && info_mp.setters->count>0)){
							// error already set (script functions only can be set once)
							THROW_RUNTIME_ERROR("Setter '%s::%s' already set"
									,script_type_name.c_str()
									,info_mp.str_byte_code_metamethod);

							return NULL;
						}
						metamethod_members.addSetter(op,symbol_function);
						break;
					case BYTE_CODE_METAMETHOD_POST_INC:
						if(metamethod_members.post_inc != NULL){

							THROW_SCRIPT_ERROR_FILE_LINE(_file,_line,"Class '%s' has already a post increment (aka '%s++') metamethod"
								,script_type_name.c_str()
							);
						}
						metamethod_members.post_inc=symbol_function;
						break;
					case BYTE_CODE_METAMETHOD_POST_DEC:
						if(metamethod_members.post_dec != NULL){

							THROW_SCRIPT_ERROR_FILE_LINE(_file,_line,"Class '%s' has already a post decrement (aka '%s--') metamethod"
								,script_type_name.c_str()
							);
						}
						metamethod_members.post_dec=symbol_function;
						break;
					case BYTE_CODE_METAMETHOD_PRE_INC:
						if(metamethod_members.pre_inc != NULL){

							THROW_SCRIPT_ERROR_FILE_LINE(_file,_line,"Class '%s' has already a pre increment (aka '++%s') metamethod"
								,script_type_name.c_str()
							);
						}
						metamethod_members.pre_inc=symbol_function;
						break;
					case BYTE_CODE_METAMETHOD_PRE_DEC:
						if(metamethod_members.pre_dec != NULL){

							THROW_SCRIPT_ERROR_FILE_LINE(_file,_line,"Class '%s' has already a pre decrement (aka '--%s') metamethod"
								,script_type_name.c_str()
							);
						}
						metamethod_members.pre_dec=symbol_function;
						break;

					}


					// break the loop telling that we found the metamethod
					break;
				}
			}
		}

		return symbol_function;
	}
	//---------------------------------------------------------
	Symbol *    ScriptType::getSymbolVariableMember(const zs_string & symbol_name, bool include_inherited_symbols){
		int idx_end=include_inherited_symbols==true?0:idx_starting_this_member_variables;
		zs_vector *list=this->script_type_scope->symbol_variables;

		for(
				int i = (int)(list->count-1);
				i >= idx_end
				; i--
		){
			Symbol *member_symbol=(Symbol *)list->items[i];
			if(member_symbol->name == symbol_name){
				return member_symbol;
			}
		}

		return NULL;
	}

	Symbol *    ScriptType::getSymbolMemberFunction(const zs_string & symbol_name, char n_params, bool include_inherited_symbols){
		bool only_symbol=n_params<0;
		int idx_end=include_inherited_symbols==true?0:idx_starting_this_member_functions;
		zs_vector *symbol_functions=this->script_type_scope->symbol_functions;

		for(
				int i = (int)(symbol_functions->count-1);
				i >= idx_end
				; i--
		){
			Symbol *member_symbol=(Symbol *)symbol_functions->items[i];
			if(member_symbol->name == symbol_name){
				if(only_symbol){
					return member_symbol;
				}

				ScriptFunction *sf=(ScriptFunction *)member_symbol->ref_ptr;
				if(((n_params==(char)sf->params_len) || (n_params==NO_PARAMS_SYMBOL_ONLY))
				 ){
					return member_symbol;
				}
			}
		}

		return NULL;
	}

	Symbol				* 	ScriptType::getSymbol(const zs_string & symbol_name, char n_params,bool include_inherited_symbols){

		Symbol *symbol=NULL;

		if((symbol=getSymbolVariableMember(symbol_name,include_inherited_symbols))==NULL){
			symbol=getSymbolMemberFunction(symbol_name,n_params,include_inherited_symbols);
		}

		return symbol;
	}

	const char *ScriptType::getTypeName(){
		/*if(idx_script_type==IDX_TYPE_ZS_INT_C){
			return "int";
		}else if(idx_script_type==IDX_TYPE_ZS_FLOAT_C){
			return "float";
		}*/

		return script_type_name.c_str();
	}



	ScriptType::~ScriptType(){

		for(int i=0; i < allocated_member_properties->count; i++){
			MemberProperty *mp=(MemberProperty *)allocated_member_properties->items[i];
			delete mp;
		}

		delete allocated_member_properties;
		delete idx_base_types;

	}
}



