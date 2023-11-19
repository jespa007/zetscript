/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	bool ScriptType::isNativeType(){

		 return ((properties & ZS_SCRIPT_TYPE_PROPERTY_C_OBJECT_REF) != 0);
	}

	bool ScriptType::isNonInstantiable(){
		 return ((properties & ZS_SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE) != 0);
	}
	//------------------------------------------------------------

	ScriptType::ScriptType(ZetScript *_zs
			,short _idx_type
			, const zs_string & _class_name
			, Scope *_class_scope
			,const char *_str_class_ptr_type
			,uint16_t _properties){
		sf_field_initializer=NULL;

		native_name=NULL;
		delete_native_instance = NULL;
		new_native_instance=NULL;
		idx_function_member_constructor = ZS_UNDEFINED_IDX;
		id=_idx_type;
		idx_starting_this_member_variables=0;
		idx_starting_this_member_functions=0;
		name=_class_name;
		scope=_class_scope;
		native_name=_str_class_ptr_type;
		allocated_member_properties=new zs_vector<MemberProperty *>();

		idx_base_types=new zs_vector<zs_int>;

		// factories
		zs = _zs;
		scope_factory = zs->getScopeFactory();
		script_function_factory= zs->getScriptFunctionFactory();
		script_type_factory=zs->getScriptTypeFactory();
		sf_field_initializer=NULL; // will be created after register type and register member extension (if available)
		properties=_properties;
	}

	void ScriptType::printListMemberFunctions(){
		Scope *scope=this->scope;
		for(int i=0; i < scope->symbol_functions->size();i++){
			Symbol *symbol = (Symbol *)scope->symbol_functions->get(i);
			ScriptFunction *sf=(ScriptFunction *)symbol->ref_ptr;
			int start_idx=0;

			zs_string script_interface="";
			zs_string native_interface="";

			// type if not mail
			script_interface.append(this->name);
			script_interface.append("::");

			// writes script function interface
			// function name
			script_interface.append(sf->name);
			script_interface.append("(");

			if(sf->properties & FUNCTION_PROPERTY_C_OBJECT_REF){
				start_idx=2;
			}

			for(int a = start_idx; a < sf->params_len; a++){
				if(a>start_idx){
					script_interface.append(",");
				}

				if(sf->properties & FUNCTION_PROPERTY_C_OBJECT_REF){
					script_interface+=zs_rtti::demangle(
						GET_IDX_2_CLASS_C_STR(this,sf->params[a].id)
					);
				}else{
					script_interface+=zs_strutils::format("p%i",a-start_idx+1);
				}
			}

			// writes wrap
			script_interface.append(")");

			//str_candidates.append(" -- BIND FUNCTION AS --> ");

			if(sf->properties & FUNCTION_PROPERTY_C_OBJECT_REF){
				native_interface.append(zs_rtti::demangle(
						GET_IDX_2_CLASS_C_STR(this,sf->return_script_type_id)
					)
				);

				native_interface.append(" (*)");
				native_interface.append("(");

				for(int a = 0; a < sf->params_len; a++){
					if(a>0){
						native_interface.append(",");
					}
					native_interface.append(zs_rtti::demangle(
							GET_IDX_2_CLASS_C_STR(this,sf->params[a].id)
						)
					);
				}
				native_interface.append(")");
			}

			printf("______________________________________________________________\n\n");
			printf(" -Function: '%s'\n",sf->name.c_str());
			printf(" -Script interface: '%s'\n",script_interface.c_str());
			if(sf->properties & FUNCTION_PROPERTY_C_OBJECT_REF){
				printf(" -Native interface: '%s'\n",native_interface.c_str());
			}
			printf(" -Type origin: '%s'\n",sf->scope->script_type_owner->name.c_str());
		}
	}

	bool ScriptType::extendsFrom(short _idx_type){
		if(_idx_type==this->id){
			return true;
		}

		for(int i=0; i < this->idx_base_types->size(); i++){
			if (script_type_factory->getScriptType(this->idx_base_types->get(i))->extendsFrom(_idx_type) == true) {
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
		const zs_string & _symbol_name
		,unsigned short _symbol_properties
		,const char * _file
		,short _line
	){

		if(getSymbol(_symbol_name.c_str())!=NULL){
			ZS_THROW_RUNTIME_ERROR("Variable '%s' already registered",_symbol_name.c_str());
			return NULL;
		}

		Symbol *symbol=scope->registerSymbolVariable(
				_file
				,_line
				,_symbol_name
		);

		symbol->properties=_symbol_properties;
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
		if((symbol_member_property=getSymbol(attrib_name.c_str())) != NULL){ // give an error  ...

			const char *what="is already defined";
			if((symbol_member_property->properties & ZS_SYMBOL_PROPERTY_MEMBER_PROPERTY)==0){

				if(symbol_member_property->properties & ZS_SYMBOL_PROPERTY_FUNCTION){
					what="it conflicts with member function";
				}else{
					what="it conflicts with member variable";
				}
			}
			ZS_THROW_EXCEPTION_FILE_LINE(file,line,"Property '%s' %s at " ZS_FORMAT_FILE_LINE
				,attrib_name.c_str()
				,what
				,zs_path::get_filename(symbol_member_property->file).c_str()
				,symbol_member_property->line
			);
		}

		symbol_member_property = scope->registerSymbolVariable(file,line,attrib_name);
		symbol_member_property->ref_ptr=(zs_int)(new MemberProperty(this,attrib_name));
		symbol_member_property->properties=ZS_SYMBOL_PROPERTY_MEMBER_PROPERTY;
		allocated_member_properties->push_back((MemberProperty *)symbol_member_property->ref_ptr);

		return symbol_member_property;
	}


	Symbol				* 	ScriptType::registerMemberPropertyMetamethod(
			const zs_string & _property_name
			,const zs_string & _metamethod_name
			,ScriptFunctionParam **_params
			,int _params_len
			,uint16_t _properties
			,int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,const char * _file
			,short _line
	){

		Symbol *symbol_member_property=NULL;
		MemberProperty *mp=NULL;
		MetamethodMemberSetterInfo mp_setter_info;
		Symbol *symbol_function=NULL;
		char symbol_metamethod_function[100];
		Symbol **ptr_getter_script_function=NULL;
		MetamethodByteCode metamethod_byte_code=MetamethodByteCode::ZS_METAMETHOD_BYTE_CODE_INVALID;

		if((symbol_member_property=getSymbol(_property_name.c_str())) == NULL){
			symbol_member_property=registerMemberProperty(_property_name,_file,_line);
		}

		mp=(MemberProperty *)symbol_member_property->ref_ptr;


		//zs_string member_property_metamethod_name=metamethod_byte_code_to_symbol_str(_metamethod)+"@"+_property_name;
		mp_setter_info=mp->metamethod_members.getSetterInfo(_metamethod_name);
		if(mp_setter_info.metamethod_byte_code != MetamethodByteCode::ZS_METAMETHOD_BYTE_CODE_INVALID){
			ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_NAME(
				symbol_metamethod_function
				, _property_name.c_str()
				,_metamethod_name.c_str()
			);
			mp_setter_info=mp->metamethod_members.getSetterInfo(_metamethod_name);
			metamethod_byte_code=mp_setter_info.metamethod_byte_code;
		}
		else{

			MetamethodMemberGetterInfo mp_getter_info=mp->metamethod_members.getGetterInfo(_metamethod_name);
			if(mp_getter_info.metamethod_byte_code != MetamethodByteCode::ZS_METAMETHOD_BYTE_CODE_INVALID){

				ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_NAME(
					symbol_metamethod_function
					, _property_name.c_str()
					,mp_getter_info.metamethod_name
				);

				if(*mp_getter_info.getter != NULL){

					ZS_THROW_EXCEPTION_FILE_LINE(_file,_line,"Error registering property metamethod '%s::%s@%s': Metamethod '%s' already registered"
						,this->name.c_str()
						,_property_name.c_str()
						,_metamethod_name.c_str()
						,_metamethod_name.c_str()
					);
				}

				metamethod_byte_code=mp_getter_info.metamethod_byte_code;

				ptr_getter_script_function=mp_getter_info.getter;

			}else{

				ZS_THROW_EXCEPTION_FILE_LINE(_file,_line,"Error registering property '%s' metamethod '%s': Metamethod not supported or not implemented"
					,_property_name.c_str()
					,_metamethod_name.c_str()
				);

			}
		}

		symbol_function=registerMemberFunction(
				symbol_metamethod_function,
				_params,
				_params_len,
				_properties,
				_idx_return_type,
				_ref_ptr,
				_file,
				_line
		);

		if(ptr_getter_script_function!=NULL){ // getter
			*ptr_getter_script_function=symbol_function;
		}else{ // setter
			mp->metamethod_members.addSetter(metamethod_byte_code,symbol_function);
			//mp_setter_info.setters->push_back(symbol_function->ref_ptr);
		}

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
		, uint16_t _function_properties
		, int _idx_return_type
		,zs_int _ref_ptr
		, const char * _file
		, short _line
	){

		if((_function_properties & FUNCTION_PROPERTY_C_OBJECT_REF)==0){ // we only allow repeated symbols on native functions...

			//if(getSymbol(_function_name,(int8_t)_params_len,false) != NULL){
				Symbol *existing_symbol=NULL;
				// we only search repeat symbols on this type because symbol cannot be repeated in the same class
				if((existing_symbol=getSymbol(_function_name.c_str(), ZS_NO_PARAMS_SYMBOL_ONLY)) != NULL){

					ScriptFunction *existing_sf=(ScriptFunction *)existing_symbol->ref_ptr;

					// check whether the function was declared at the same class
					if(existing_sf->scope->script_type_owner == this){
						ZS_THROW_EXCEPTION("Member function '%s' is already defined at " ZS_FORMAT_FILE_LINE
							,_function_name.c_str()
							//,zs_path::get_filename(_file).c_str()
							//,_line
							,zs_path::get_filename(existing_symbol->file).c_str()
							,existing_symbol->line
						);
						return NULL;
					}
				}

			//}
		}else{ // native function, check parameter type ...

			ScriptFunction::checkNativeFunctionParams(this->scope,_idx_return_type,_function_name,*_params,_params_len);
		}

		Symbol *symbol_function =  script_function_factory->newScriptFunction(
				//---- Register data
				this->scope
				,_file
				,_line
				//---- Function data
				,id 				// idx type which belongs to...
				,_function_name
				,_params
				,_params_len
				,_idx_return_type
				,_ref_ptr // c function
				,_function_properties
		);

		ScriptFunction *sf_current=(ScriptFunction *)symbol_function->ref_ptr;
		sf_current->scope=this->scope;

		// register num function symbols only for c symbols...
		if(sf_current->properties & FUNCTION_PROPERTY_C_OBJECT_REF){
			Symbol *symbol_repeat=NULL;
			if((symbol_repeat=this->getSymbolMemberFunction(_function_name.c_str(),ZS_NO_PARAMS_SYMBOL_ONLY))!=NULL){ // there's one or more name with same args --> mark deduce at runtime
				ScriptFunction *sf_repeat=(ScriptFunction *)symbol_repeat->ref_ptr;

				sf_repeat->properties|=FUNCTION_PROPERTY_DEDUCE_AT_RUNTIME;
				sf_current->properties|=FUNCTION_PROPERTY_DEDUCE_AT_RUNTIME;
			}
		}

		// constructor...
		if(_function_name == ZS_CONSTRUCTOR_FUNCTION_NAME){ //  FUNCTION_MEMBER_CONSTRUCTOR_NAME
			idx_function_member_constructor = symbol_function->idx_position;
		}
		else{
			// check metamethod function (not property metamethod)...
			for(int i = 0; i < ZS_MAX_METAMETHOD_BYTE_CODES; i++){
				if(ZS_STRCMP(metamethod_byte_code_to_symbol_str((MetamethodByteCode)i),==,_function_name.c_str())){
					// check whether function meets the conditions of num params, static etc
					MetamethodMemberSetterInfo info_mp;
					MetamethodByteCode op=(MetamethodByteCode)i;
					const char *metamethod_byte_code_operator_str=metamethod_byte_code_to_operator_str(op);
					const char *str_symbol_metamethod=metamethod_byte_code_to_symbol_str(op);
					int n_args_static_metamethod=metamethod_byte_code_get_num_arguments(op); // expected params for static function, n_args -1 else
					int this_arg=0;

					// can be one parameter or 0 params...
					if(metamethod_byte_code_should_be_static(op) && ((_function_properties & FUNCTION_PROPERTY_STATIC)==0)){
						ZS_THROW_RUNTIME_ERROR("Metamethod '%s::%s' has to be declared as static instead of member"
							,name.c_str()
							,_function_name.c_str()
						);
						return NULL;
					}else if((metamethod_byte_code_should_be_static(op)==false) && ((_function_properties & FUNCTION_PROPERTY_STATIC))){
						ZS_THROW_RUNTIME_ERROR("Metamethod '%s::%s' has to be declared as member instead of static"
							,name.c_str()
							,_function_name.c_str()
						);
						return NULL;
					}

					// native
					if((_function_properties & FUNCTION_PROPERTY_C_OBJECT_REF)){ // if-native

						/*switch(_idx_return_type){
						case IDX_TYPE_SCRIPT_OBJECT_CLASS:
						case IDX_TYPE_ZS_STRING_C:
						case IDX_TYPE_INT_C:
						case IDX_TYPE_FLOAT_C:
						case IDX_TYPE_BOOL_C:
							break;

						default:
							ZS_THROW_RUNTIME_ERROR(
								"Error on register Metamethod '%s::%s'. Invalid return type. Metamethods can only return on of these types:\n"
								"- bool \n"
								"-  \n"
								"- zs_float \n"
								"- zs_float \n"
								,name.c_str()
								,_function_name.c_str()
							);
							return NULL;
						}*/


						if(_function_properties & FUNCTION_PROPERTY_STATIC){

							// ensure logic metamethods returns bool type
							switch(i){
							case ZS_METAMETHOD_BYTE_CODE_EQU:
							case ZS_METAMETHOD_BYTE_CODE_NEQU:
							case ZS_METAMETHOD_BYTE_CODE_LT:
							case ZS_METAMETHOD_BYTE_CODE_LTE:
							case ZS_METAMETHOD_BYTE_CODE_GT:
							case ZS_METAMETHOD_BYTE_CODE_GTE:
								break;
							case ZS_METAMETHOD_BYTE_CODE_ADD: // +
							case ZS_METAMETHOD_BYTE_CODE_SUB: // -
							case ZS_METAMETHOD_BYTE_CODE_DIV: // /
							case ZS_METAMETHOD_BYTE_CODE_MUL: // *
							case ZS_METAMETHOD_BYTE_CODE_MOD: // %
							case ZS_METAMETHOD_BYTE_CODE_AND: // & bitwise logic and
							case ZS_METAMETHOD_BYTE_CODE_OR:  // | bitwise logic or
							case ZS_METAMETHOD_BYTE_CODE_XOR: // ^ logic xor
							case ZS_METAMETHOD_BYTE_CODE_SHL: // << shift left
							case ZS_METAMETHOD_BYTE_CODE_SHR: // >> shift right
								if(_idx_return_type != IDX_TYPE_SCRIPT_OBJECT_CLASS){
									ZS_THROW_RUNTIME_ERROR("Error registering static metamethod '%s::%s'. Expected return 'ClassScriptObject *' but it was '%s'",
										this->name.c_str(),
										_function_name.c_str(),
										zs_rtti::demangle(this->script_type_factory->getScriptType(_idx_return_type)->native_name.c_str()).c_str()
									);

									return NULL;
								}
								break;

								/*if(this->script_type_factory->getScriptType(_idx_return_type)->native_name != this->native_name){

									ZS_THROW_RUNTIME_ERROR("error registering metamethod '%s::%s'. Expected return '%s' but it was '%s'",
											this->name.c_str(),
											_function_name.c_str(),
											zs_rtti::demangle(this->native_name.c_str()).c_str(),
											zs_rtti::demangle(this->script_type_factory->getScriptType(_idx_return_type)->native_name.c_str()).c_str()
									);
									return NULL;
								}*/

							default:

								ZS_THROW_RUNTIME_ERROR("Internal error registering static metamethod '%s::%s'. Metamethod '%s' not supported or not implemented (1)",
									this->name.c_str(),
									_function_name.c_str(),
									_function_name.c_str()
								);

								return NULL;

								break;
							}



						}else{
							// member metamethod
							switch(i){
							case ZS_METAMETHOD_BYTE_CODE_TO_STRING:
								if(_idx_return_type != IDX_TYPE_ZS_STRING_C){
									ZS_THROW_RUNTIME_ERROR("Error registering member metamethod '%s::%s'. Expected return 'zs_string' but it was '%s'",
											this->name.c_str(),
											_function_name.c_str(),
											zs_rtti::demangle(this->script_type_factory->getScriptType(_idx_return_type)->native_name.c_str()).c_str()
									);
									return NULL;
								}
								break;
							case ZS_METAMETHOD_BYTE_CODE_NOT:
							case ZS_METAMETHOD_BYTE_CODE_IN:

								if(_idx_return_type != IDX_TYPE_BOOL_C){
									ZS_THROW_RUNTIME_ERROR("Error registering member metamethod '%s::%s'. Expected return 'bool' but it was '%s'",
											this->name.c_str(),
											_function_name.c_str(),
											zs_rtti::demangle(this->script_type_factory->getScriptType(_idx_return_type)->native_name.c_str()).c_str()
									);
									return NULL;
								}
								break;
							case ZS_METAMETHOD_BYTE_CODE_NEG:
							case ZS_METAMETHOD_BYTE_CODE_BWC:

								if(_idx_return_type != IDX_TYPE_SCRIPT_OBJECT_CLASS){
									ZS_THROW_RUNTIME_ERROR("Error registering member metamethod '%s::%s'. Expected return 'ClassScriptObject *' but it was '%s'",
										this->name.c_str(),
										_function_name.c_str(),
										zs_rtti::demangle(this->script_type_factory->getScriptType(_idx_return_type)->native_name.c_str()).c_str()
									);

									return NULL;
								}
								break;
							case ZS_METAMETHOD_BYTE_CODE_POST_INC: // i++
							case ZS_METAMETHOD_BYTE_CODE_POST_DEC: // i--
								if(_idx_return_type != IDX_TYPE_SCRIPT_OBJECT_CLASS /*&& _idx_return_type != IDX_TYPE_VOID_C*/){
									ZS_THROW_RUNTIME_ERROR("Error registering member metamethod '%s::%s'. Expected return 'ClassScriptObject *' but it was '%s'",
										this->name.c_str(),
										_function_name.c_str(),
										zs_rtti::demangle(this->script_type_factory->getScriptType(_idx_return_type)->native_name.c_str()).c_str()
									);

									return NULL;
								}
								break;
							case ZS_METAMETHOD_BYTE_CODE_SET:
							case ZS_METAMETHOD_BYTE_CODE_ADD_ASSIGN:
							case ZS_METAMETHOD_BYTE_CODE_SUB_ASSIGN:
							case ZS_METAMETHOD_BYTE_CODE_MUL_ASSIGN:
							case ZS_METAMETHOD_BYTE_CODE_DIV_ASSIGN:
							case ZS_METAMETHOD_BYTE_CODE_MOD_ASSIGN:
							case ZS_METAMETHOD_BYTE_CODE_AND_ASSIGN:
							case ZS_METAMETHOD_BYTE_CODE_OR_ASSIGN:
							case ZS_METAMETHOD_BYTE_CODE_XOR_ASSIGN:
							case ZS_METAMETHOD_BYTE_CODE_SHL_ASSIGN:
							case ZS_METAMETHOD_BYTE_CODE_SHR_ASSIGN:
							case ZS_METAMETHOD_BYTE_CODE_PRE_INC: // ++i
							case ZS_METAMETHOD_BYTE_CODE_PRE_DEC: // --i
								// ok do not return nothing
								break;
							default:
								ZS_THROW_RUNTIME_ERROR("Internal error registering member metamethod '%s::%s'. Metamethod '%s' not supported or not implemented (2)",
									this->name.c_str(),
									_function_name.c_str(),
									_function_name.c_str()
								);

								return NULL;


							}
						}

						if(_function_properties & FUNCTION_PROPERTY_MEMBER_FUNCTION){
							this_arg=1;
						}
					}

					if(symbol_function->n_params< (n_args_static_metamethod+this_arg)){ // non-static functions pass this object as first parameter
						ZS_THROW_RUNTIME_ERROR("%s metamethod '%s' (aka '%s') should have at least %i parameter/s"
							,(_function_properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)?"Member":"Static"
							,str_symbol_metamethod
							,metamethod_byte_code_operator_str
							,n_args_static_metamethod
						);
						return NULL;
					}


					// in the type only setters are registered in member property variable (getter is ignored)
					switch(op){

					case ZS_METAMETHOD_BYTE_CODE_EQU:
					case ZS_METAMETHOD_BYTE_CODE_NEQU:
					case ZS_METAMETHOD_BYTE_CODE_LT:
					case ZS_METAMETHOD_BYTE_CODE_LTE:
					case ZS_METAMETHOD_BYTE_CODE_GT:
					case ZS_METAMETHOD_BYTE_CODE_GTE:
					case ZS_METAMETHOD_BYTE_CODE_NOT:
					case ZS_METAMETHOD_BYTE_CODE_IN:
					case ZS_METAMETHOD_BYTE_CODE_ADD:
					case ZS_METAMETHOD_BYTE_CODE_SUB:
					case ZS_METAMETHOD_BYTE_CODE_DIV:
					case ZS_METAMETHOD_BYTE_CODE_MUL:
					case ZS_METAMETHOD_BYTE_CODE_MOD:
					case ZS_METAMETHOD_BYTE_CODE_AND:
					case ZS_METAMETHOD_BYTE_CODE_OR:
					case ZS_METAMETHOD_BYTE_CODE_XOR:
					case ZS_METAMETHOD_BYTE_CODE_SHL:
					case ZS_METAMETHOD_BYTE_CODE_SHR:
					case ZS_METAMETHOD_BYTE_CODE_TO_STRING:
						// do nothing
						break;


					case ZS_METAMETHOD_BYTE_CODE_SET:
					case ZS_METAMETHOD_BYTE_CODE_ADD_ASSIGN:
					case ZS_METAMETHOD_BYTE_CODE_SUB_ASSIGN:
					case ZS_METAMETHOD_BYTE_CODE_MUL_ASSIGN:
					case ZS_METAMETHOD_BYTE_CODE_DIV_ASSIGN:
					case ZS_METAMETHOD_BYTE_CODE_MOD_ASSIGN:
					case ZS_METAMETHOD_BYTE_CODE_AND_ASSIGN:
					case ZS_METAMETHOD_BYTE_CODE_OR_ASSIGN:
					case ZS_METAMETHOD_BYTE_CODE_XOR_ASSIGN:
					case ZS_METAMETHOD_BYTE_CODE_SHL_ASSIGN:
					case ZS_METAMETHOD_BYTE_CODE_SHR_ASSIGN:

						info_mp=metamethod_members.getSetterInfo(op);

						if(((_function_properties & FUNCTION_PROPERTY_C_OBJECT_REF)==0) //--> script function has to have one setter function, meanwhile c ref can have more than one (due different signatures)
								&&
							(info_mp.setters!=NULL && info_mp.setters->size()>0)){
							// error already set (script functions only can be set once)
							ZS_THROW_RUNTIME_ERROR("Setter '%s::%s' already set"
									,name.c_str()
									,info_mp.metamethod_name);

							return NULL;
						}
						metamethod_members.addSetter(op,symbol_function);
						break;
					default:

						{
							struct CheckSymbolInfo{
								const char *name;
								Symbol **symbol;

							}check_symbol_info[]={
									//{"_get",&metamethod_members.getter}
									{"_postinc",&metamethod_members.postinc}
									,{"_postdec",&metamethod_members.postdec}
									,{"_preinc",&metamethod_members.preinc}
									,{"_predec",&metamethod_members.predec}
									,{"_neg",&metamethod_members.neg}
									,{"_bwc",&metamethod_members.bwc}
									,{NULL,NULL}
							};

							CheckSymbolInfo *it=check_symbol_info;

							while(it->name != NULL){
								if(_function_name == it->name){
									if(*it->symbol==NULL){
										*it->symbol=symbol_function;
										break;
									}else{
										ZS_THROW_EXCEPTION_FILE_LINE(_file,_line,"Class '%s' has alrady metamethod '%s' implemented"
											,name.c_str()
											,_function_name.c_str()
										);
									}
								}

								it++;
							}


							if(it->name==NULL){

								ZS_THROW_RUNTIME_ERROR("Internal error registering member metamethod '%s::%s'. Metamethod '%s' not supported or not implemented (3)",
									this->name.c_str(),
									_function_name.c_str(),
									_function_name.c_str()
								);

								return NULL;
							}
						}
						break;
					}
				}
			}
		}

		return symbol_function;
	}
	//---------------------------------------------------------
	Symbol *    ScriptType::getSymbolVariableMember(const char  *_const_symbol_name, bool include_inherited_symbols){
		int idx_end=include_inherited_symbols==true?0:idx_starting_this_member_variables;
		zs_vector<Symbol *> *list=this->scope->symbol_variables;

		for(
				int i = (int)(list->size()-1);
				i >= idx_end
				; i--
		){
			Symbol *member_symbol=(Symbol *)list->get(i);
			if(member_symbol->name == _const_symbol_name){
				return member_symbol;
			}
		}

		return NULL;
	}

	Symbol *    ScriptType::getSymbolMemberFunction(const char * _const_symbol_name, int8_t n_params, bool include_inherited_symbols){
		bool only_symbol=n_params<0;
		int idx_end=include_inherited_symbols==true?0:idx_starting_this_member_functions;
		zs_vector<Symbol *> *symbol_functions=this->scope->symbol_functions;

		int i = (int)(symbol_functions->size()-1);

		while(i >= idx_end){
			Symbol *member_symbol=symbol_functions->data()[i];
			if(strcmp(member_symbol->name.c_str(),_const_symbol_name)==0){
				ScriptFunction *sf=(ScriptFunction *)member_symbol->ref_ptr;
				if(only_symbol || (n_params==(int8_t)sf->params_len)){
					return member_symbol;
				}
			}
			i--;
		}

		return NULL;
	}

	Symbol				* 	ScriptType::getSymbol(const char  * _const_symbol_name, int8_t n_params,bool include_inherited_symbols){

		Symbol *symbol=NULL;

		if((symbol=getSymbolVariableMember(_const_symbol_name,include_inherited_symbols))==NULL){
			symbol=getSymbolMemberFunction(_const_symbol_name,n_params,include_inherited_symbols);
		}

		return symbol;
	}

	const char *ScriptType::getTypeName(){
		return name.c_str();
	}

	ScriptType::~ScriptType(){

		for(int i=0; i < allocated_member_properties->size(); i++){
			MemberProperty *mp=(MemberProperty *)allocated_member_properties->get(i);
			delete mp;
		}

		delete allocated_member_properties;
		delete idx_base_types;

	}
}



