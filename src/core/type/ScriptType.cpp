/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	bool ScriptType::isNativeType(){

		 return ((properties & SCRIPT_TYPE_PROPERTY_NATIVE_OBJECT_REF) != 0);
	}

	bool ScriptType::isNonInstantiable(){
		 return ((properties & SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE) != 0);
	}
	//------------------------------------------------------------

	ScriptType::ScriptType(ScriptEngine *_script_engine
			,ScriptTypeId _id
			, const String & _class_name
			, ScriptScope *_class_scope
			,const char *_str_class_ptr_type
			,uint16_t _properties){
		sf_field_initializer=NULL;

		native_name=NULL;
		delete_native_instance = NULL;
		new_native_instance=NULL;
		idx_constructor_member_function = ZS_UNDEFINED_IDX;
		id=_id;
		idx_starting_this_member_variables=0;
		idx_starting_this_member_functions=0;
		name=_class_name;
		scope=_class_scope;
		native_name=_str_class_ptr_type;
		allocated_member_properties=new Vector<MemberProperty *>();

		base_script_type_ids=new Vector<ScriptTypeId>;

		// factories
		script_engine = _script_engine;
		scope_factory = script_engine->getScriptScopesFactory();
		script_function_factory= script_engine->getScriptFunctionsFactory();
		script_types_factory=script_engine->getScriptTypesFactory();
		sf_field_initializer=NULL; // will be created after register type and register member extension (if available)
		properties=_properties;
	}

	void ScriptType::printListMemberFunctions(){
		ScriptScope *scope=this->scope;
		for(int i=0; i < scope->symbol_functions->length();i++){
			Symbol *symbol = (Symbol *)scope->symbol_functions->get(i);
			ScriptFunction *sf=(ScriptFunction *)symbol->ref_ptr;
			int start_idx=0;

			String script_interface="";
			String native_interface="";

			// type if not mail
			script_interface.append(this->name);
			script_interface.append("::");

			// writes script function interface
			// function name
			script_interface.append(sf->name);
			script_interface.append("(");

			if(sf->properties & SCRIPT_FUNCTION_PROPERTY_NATIVE_OBJECT_REF){
				start_idx=2;
			}

			for(int a = start_idx; a < sf->params_len; a++){
				if(a>start_idx){
					script_interface.append(",");
				}

				if(sf->properties & SCRIPT_FUNCTION_PROPERTY_NATIVE_OBJECT_REF){
					script_interface+=Rtti::demangle(
						ZS_SCRIPT_TYPE_ID_TO_NATIVE_NAME(this,sf->params[a].script_type_id)
					);
				}else{
					script_interface+=String::format("p%i",a-start_idx+1);
				}
			}

			// writes wrap
			script_interface.append(")");

			//str_candidates.append(" -- BIND FUNCTION AS --> ");

			if(sf->properties & SCRIPT_FUNCTION_PROPERTY_NATIVE_OBJECT_REF){
				native_interface.append(Rtti::demangle(
						ZS_SCRIPT_TYPE_ID_TO_NATIVE_NAME(this,sf->return_script_type_id)
					)
				);

				native_interface.append(" (*)");
				native_interface.append("(");

				for(int a = 0; a < sf->params_len; a++){
					if(a>0){
						native_interface.append(",");
					}
					native_interface.append(Rtti::demangle(
							ZS_SCRIPT_TYPE_ID_TO_NATIVE_NAME(this,sf->params[a].script_type_id)
						)
					);
				}
				native_interface.append(")");
			}

			printf("______________________________________________________________\n\n");
			printf(" -ScriptFunction: '%s'\n",sf->name.toConstChar());
			printf(" -Script interface: '%s'\n",script_interface.toConstChar());
			if(sf->properties & SCRIPT_FUNCTION_PROPERTY_NATIVE_OBJECT_REF){
				printf(" -Native interface: '%s'\n",native_interface.toConstChar());
			}
			printf(" -ScriptType origin: '%s'\n",sf->scope->owner_type->name.toConstChar());
		}
	}

	bool ScriptType::extendsFrom(ScriptTypeId _script_type_id){
		if(_script_type_id==this->id){
			return true;
		}

		for(int i=0; i < this->base_script_type_ids->length(); i++){
			if (script_types_factory->getScriptType(this->base_script_type_ids->get(i))->extendsFrom(_script_type_id) == true) {
				return true;
			}
		}

		return false;
	}

	ScriptType * 				ScriptType::getScriptType(ScriptTypeId _script_type_id){
		return script_types_factory->getScriptType(_script_type_id);
	}

	ScriptTypeId				ScriptType::getScriptTypeIdFromNamePtr(const char * s){
		return script_types_factory->getScriptTypeIdFromNamePtr(s);
	}

	//---------------------------------------------------
	//
	// MEMBER VARIABLES
	//
	Symbol				* 	ScriptType::registerMemberVariable(
		const String & _symbol_name
		,unsigned short _symbol_properties
		,const char * _file
		,short _line
	){

		if(getSymbol(_symbol_name.toConstChar())!=NULL){
			ZS_THROW_RUNTIME_ERROR("Variable '%s' already registered",_symbol_name.toConstChar());
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
			 const String & attrib_name
			,const char * file
			,short line
	){
		Symbol *symbol_member_property=NULL;
		if((symbol_member_property=getSymbol(attrib_name.toConstChar())) != NULL){ // give an error  ...

			const char *what="is already defined";
			if((symbol_member_property->properties & SYMBOL_PROPERTY_MEMBER_PROPERTY)==0){

				if(symbol_member_property->properties & SYMBOL_PROPERTY_FUNCTION){
					what="it conflicts with member function";
				}else{
					what="it conflicts with member variable";
				}
			}
			ZS_THROW_EXCEPTION_FILE_LINE(file,line,"Property '%s' %s at " ZS_FORMAT_FILE_LINE
				,attrib_name.toConstChar()
				,what
				,Path::getFilename(symbol_member_property->file).toConstChar()
				,symbol_member_property->line
			);
		}

		symbol_member_property = scope->registerSymbolVariable(file,line,attrib_name);
		symbol_member_property->ref_ptr=(zs_int)(new MemberProperty(this,attrib_name));
		symbol_member_property->properties=SYMBOL_PROPERTY_MEMBER_PROPERTY;
		allocated_member_properties->push((MemberProperty *)symbol_member_property->ref_ptr);

		return symbol_member_property;
	}


	Symbol				* 	ScriptType::registerMemberPropertyMetamethod(
			const String & _property_name
			,const String & _metamethod_name
			,FunctionParam **_params
			,int _params_len
			,uint16_t _properties
			,ScriptTypeId _return_script_type_id
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
		Metamethod metamethod=METAMETHOD_INVALID;

		if((symbol_member_property=getSymbol(_property_name.toConstChar())) == NULL){
			symbol_member_property=registerMemberProperty(_property_name,_file,_line);
		}

		mp=(MemberProperty *)symbol_member_property->ref_ptr;


		//String member_property_metamethod_name=MetamethodHelper::getMetamethodSymbolName(_metamethod)+"@"+_property_name;
		mp_setter_info=mp->metamethod_members.getSetterInfo(_metamethod_name);
		if(mp_setter_info.metamethod != METAMETHOD_INVALID){
			ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_NAME(
				symbol_metamethod_function
				, _property_name.toConstChar()
				,_metamethod_name.toConstChar()
			);
			mp_setter_info=mp->metamethod_members.getSetterInfo(_metamethod_name);
			metamethod=mp_setter_info.metamethod;
		}
		else{

			MetamethodMemberGetterInfo mp_getter_info=mp->metamethod_members.getGetterInfo(_metamethod_name);
			if(mp_getter_info.metamethod != METAMETHOD_INVALID){

				ZS_SYMBOL_NAME_MEMBER_PROPERTY_METAMETHOD_NAME(
					symbol_metamethod_function
					, _property_name.toConstChar()
					,mp_getter_info.metamethod_name
				);

				if(*mp_getter_info.getter != NULL){

					ZS_THROW_EXCEPTION_FILE_LINE(_file,_line,"Error registering property metamethod '%s::%s@%s': MetamethodHelper '%s' already registered"
						,this->name.toConstChar()
						,_property_name.toConstChar()
						,_metamethod_name.toConstChar()
						,_metamethod_name.toConstChar()
					);
				}

				metamethod=mp_getter_info.metamethod;

				ptr_getter_script_function=mp_getter_info.getter;

			}else{

				ZS_THROW_EXCEPTION_FILE_LINE(_file,_line,"Error registering property '%s' metamethod '%s': MetamethodHelper not supported or not implemented"
					,_property_name.toConstChar()
					,_metamethod_name.toConstChar()
				);

			}
		}

		symbol_function=registerMemberFunction(
				symbol_metamethod_function,
				_params,
				_params_len,
				_properties,
				_return_script_type_id,
				_ref_ptr,
				_file,
				_line
		);

		if(ptr_getter_script_function!=NULL){ // getter
			*ptr_getter_script_function=symbol_function;
		}else{ // setter
			mp->metamethod_members.addSetter(metamethod,symbol_function);
			//mp_setter_info.setters->append(symbol_function->ref_ptr);
		}

		return symbol_member_property;
	}


	//---------------------------------------------------
	//
	// MEMBER FUNCTIONS
	//

	Symbol * ScriptType::registerMemberFunction(
		 const String & _function_name
		 , FunctionParam **_params
		 ,int _params_len
		, uint16_t _function_properties
		, ScriptTypeId _return_script_type_id
		,zs_int _ref_ptr
		, const char * _file
		, short _line
	){

		if((_function_properties & SCRIPT_FUNCTION_PROPERTY_NATIVE_OBJECT_REF)==0){ // we only allow repeated symbols on native functions...

			//if(getSymbol(_function_name,(int8_t)_params_len,false) != NULL){
				Symbol *existing_symbol=NULL;
				// we only search repeat symbols on this type because symbol cannot be repeated in the same class
				if((existing_symbol=getSymbol(_function_name.toConstChar(), ZS_NO_PARAMS_SYMBOL_ONLY)) != NULL){

					ScriptFunction *existing_sf=(ScriptFunction *)existing_symbol->ref_ptr;

					// check whether the function was declared at the same class
					if(existing_sf->scope->owner_type == this){
						ZS_THROW_EXCEPTION("Member function '%s' is already defined at " ZS_FORMAT_FILE_LINE
							,_function_name.toConstChar()
							//,Path::getFilename(_file).toConstChar()
							//,_line
							,Path::getFilename(existing_symbol->file).toConstChar()
							,existing_symbol->line
						);
						return NULL;
					}
				}

			//}
		}else{ // native function, check parameter type ...

			ScriptFunction::checkNativeFunctionParams(this->scope,_return_script_type_id,_function_name,*_params,_params_len);
		}

		Symbol *symbol_function =  script_function_factory->newFunction(
				//---- Register data
				this->scope
				,_file
				,_line
				//---- ScriptFunction data
				,this->id 				// idx type which belongs to...
				,_function_name
				,_params
				,_params_len
				,_return_script_type_id
				,_ref_ptr // c function
				,_function_properties
		);

		ScriptFunction *sf_current=(ScriptFunction *)symbol_function->ref_ptr;
		sf_current->scope=this->scope;

		// register num function symbols only for c symbols...
		if(sf_current->properties & SCRIPT_FUNCTION_PROPERTY_NATIVE_OBJECT_REF){
			Symbol *symbol_repeat=NULL;
			if((symbol_repeat=this->getSymbolMemberFunction(_function_name.toConstChar(),ZS_NO_PARAMS_SYMBOL_ONLY))!=NULL){ // there's one or more name with same args --> mark deduce at runtime
				ScriptFunction *sf_repeat=(ScriptFunction *)symbol_repeat->ref_ptr;

				sf_repeat->properties|=SCRIPT_FUNCTION_PROPERTY_DEDUCE_AT_RUNTIME;
				sf_current->properties|=SCRIPT_FUNCTION_PROPERTY_DEDUCE_AT_RUNTIME;
			}
		}

		// constructor...
		if(_function_name == ZS_CONSTRUCTOR_FUNCTION_NAME){ //  FUNCTION_MEMBER_CONSTRUCTOR_NAME
			idx_constructor_member_function = symbol_function->idx_position;
		}
		else{
			// check metamethod function (not property metamethod)...
			for(int i = 0; i < Metamethod::MAX_METAMETHOD_IDS; i++){
				if(ZS_STRCMP(MetamethodHelper::getMetamethodSymbolName((Metamethod)i),==,_function_name.toConstChar())){
					// check whether function meets the conditions of num params, static etc
					MetamethodMemberSetterInfo info_mp;
					Metamethod op=(Metamethod)i;
					const char *operator_str=MetamethodHelper::getMetamethodOperatorName(op);
					const char *str_symbol_metamethod=MetamethodHelper::getMetamethodSymbolName(op);
					int n_args_static_metamethod=MetamethodHelper::getMetamethodNumberArguments(op); // expected params for static function, n_args -1 else
					int this_arg=0;

					// can be one parameter or 0 params...
					if(MetamethodHelper::isMetamethodStatic(op) && ((_function_properties & SCRIPT_FUNCTION_PROPERTY_STATIC)==0)){
						ZS_THROW_RUNTIME_ERROR("MetamethodHelper '%s::%s' has to be declared as static instead of member"
							,name.toConstChar()
							,_function_name.toConstChar()
						);
						return NULL;
					}else if((MetamethodHelper::isMetamethodStatic(op)==false) && ((_function_properties & SCRIPT_FUNCTION_PROPERTY_STATIC))){
						ZS_THROW_RUNTIME_ERROR("MetamethodHelper '%s::%s' has to be declared as member instead of static"
							,name.toConstChar()
							,_function_name.toConstChar()
						);
						return NULL;
					}

					// native
					if((_function_properties & SCRIPT_FUNCTION_PROPERTY_NATIVE_OBJECT_REF)){ // if-native

						/*switch(_return_script_type_id){
						case ScriptTypeId::SCRIPT_TYPE_ID_CLASS_SCRIPT_OBJECT:
						case ScriptTypeId::SCRIPT_TYPE_ID_STRING_C:
						case ScriptTypeId::SCRIPT_TYPE_ID_INT_C:
						case ScriptTypeId::SCRIPT_TYPE_ID_FLOAT_C:
						case ScriptTypeId::SCRIPT_TYPE_ID_BOOL_C:
							break;

						default:
							ZS_THROW_RUNTIME_ERROR(
								"Error on register MetamethodHelper '%s::%s'. Invalid return type. Metamethods can only return on of these types:\n"
								"- bool \n"
								"-  \n"
								"- zs_float \n"
								"- zs_float \n"
								,name.toConstChar()
								,_function_name.toConstChar()
							);
							return NULL;
						}*/


						if(_function_properties & SCRIPT_FUNCTION_PROPERTY_STATIC){

							// ensure logic metamethods returns bool type
							switch(i){
							case METAMETHOD_EQU:
							case METAMETHOD_NEQU:
							case METAMETHOD_LT:
							case METAMETHOD_LTE:
							case METAMETHOD_GT:
							case METAMETHOD_GTE:
								break;
							case METAMETHOD_ADD: // +
							case METAMETHOD_SUB: // -
							case METAMETHOD_DIV: // /
							case METAMETHOD_MUL: // *
							case METAMETHOD_MOD: // %
							case METAMETHOD_AND: // & bitwise logic and
							case METAMETHOD_OR:  // | bitwise logic or
							case METAMETHOD_XOR: // ^ logic xor
							case METAMETHOD_SHL: // << shift left
							case METAMETHOD_SHR: // >> shift right
								if(_return_script_type_id != SCRIPT_TYPE_ID_CLASS_SCRIPT_OBJECT){
									ZS_THROW_RUNTIME_ERROR("Error registering static metamethod '%s::%s'. Expected return 'ClassScriptObject *' but it was '%s'",
										this->name.toConstChar(),
										_function_name.toConstChar(),
										Rtti::demangle(this->script_types_factory->getScriptType(_return_script_type_id)->native_name.toConstChar()).toConstChar()
									);

									return NULL;
								}
								break;

								/*if(this->script_types_factory->getScriptType(_return_script_type_id)->native_name != this->native_name){

									ZS_THROW_RUNTIME_ERROR("error registering metamethod '%s::%s'. Expected return '%s' but it was '%s'",
											this->name.toConstChar(),
											_function_name.toConstChar(),
											Rtti::demangle(this->native_name.toConstChar()).toConstChar(),
											Rtti::demangle(this->script_types_factory->getScriptType(_return_script_type_id)->native_name.toConstChar()).toConstChar()
									);
									return NULL;
								}*/

							default:

								ZS_THROW_RUNTIME_ERROR("Internal error registering static metamethod '%s::%s'. MetamethodHelper '%s' not supported or not implemented (1)",
									this->name.toConstChar(),
									_function_name.toConstChar(),
									_function_name.toConstChar()
								);

								return NULL;

								break;
							}



						}else{
							// member metamethod
							switch(i){
							case METAMETHOD_TO_STRING:
								if(_return_script_type_id != SCRIPT_TYPE_ID_STRING){
									ZS_THROW_RUNTIME_ERROR("Error registering member metamethod '%s::%s'. Expected return 'String' but it was '%s'",
											this->name.toConstChar(),
											_function_name.toConstChar(),
											Rtti::demangle(this->script_types_factory->getScriptType(_return_script_type_id)->native_name.toConstChar()).toConstChar()
									);
									return NULL;
								}
								break;
							case METAMETHOD_NOT:
							case METAMETHOD_IN:

								if(_return_script_type_id != SCRIPT_TYPE_ID_BOOL){
									ZS_THROW_RUNTIME_ERROR("Error registering member metamethod '%s::%s'. Expected return 'bool' but it was '%s'",
											this->name.toConstChar(),
											_function_name.toConstChar(),
											Rtti::demangle(this->script_types_factory->getScriptType(_return_script_type_id)->native_name.toConstChar()).toConstChar()
									);
									return NULL;
								}
								break;
							case METAMETHOD_NEG:
							case METAMETHOD_BWC:

								if(_return_script_type_id != SCRIPT_TYPE_ID_CLASS_SCRIPT_OBJECT){
									ZS_THROW_RUNTIME_ERROR("Error registering member metamethod '%s::%s'. Expected return 'ClassScriptObject *' but it was '%s'",
										this->name.toConstChar(),
										_function_name.toConstChar(),
										Rtti::demangle(this->script_types_factory->getScriptType(_return_script_type_id)->native_name.toConstChar()).toConstChar()
									);

									return NULL;
								}
								break;
							case METAMETHOD_POST_INC: // i++
							case METAMETHOD_POST_DEC: // i--
								if(_return_script_type_id != SCRIPT_TYPE_ID_CLASS_SCRIPT_OBJECT /*&& _return_script_type_id != ScriptTypeId::SCRIPT_TYPE_ID_VOID_C*/){
									ZS_THROW_RUNTIME_ERROR("Error registering member metamethod '%s::%s'. Expected return 'ClassScriptObject *' but it was '%s'",
										this->name.toConstChar(),
										_function_name.toConstChar(),
										Rtti::demangle(this->script_types_factory->getScriptType(_return_script_type_id)->native_name.toConstChar()).toConstChar()
									);

									return NULL;
								}
								break;
							case METAMETHOD_SET:
							case METAMETHOD_ADD_ASSIGN:
							case METAMETHOD_SUB_ASSIGN:
							case METAMETHOD_MUL_ASSIGN:
							case METAMETHOD_DIV_ASSIGN:
							case METAMETHOD_MOD_ASSIGN:
							case METAMETHOD_AND_ASSIGN:
							case METAMETHOD_OR_ASSIGN:
							case METAMETHOD_XOR_ASSIGN:
							case METAMETHOD_SHL_ASSIGN:
							case METAMETHOD_SHR_ASSIGN:
							case METAMETHOD_PRE_INC: // ++i
							case METAMETHOD_PRE_DEC: // --i
								// ok do not return nothing
								break;
							default:
								ZS_THROW_RUNTIME_ERROR("Internal error registering member metamethod '%s::%s'. MetamethodHelper '%s' not supported or not implemented (2)",
									this->name.toConstChar(),
									_function_name.toConstChar(),
									_function_name.toConstChar()
								);

								return NULL;


							}
						}

						if(_function_properties & SCRIPT_FUNCTION_PROPERTY_MEMBER_FUNCTION){
							this_arg=1;
						}
					}

					if(symbol_function->n_params< (n_args_static_metamethod+this_arg)){ // non-static functions pass this object as first parameter
						ZS_THROW_RUNTIME_ERROR("%s metamethod '%s' (aka '%s') should have at least %i parameter/s"
							,(_function_properties & SCRIPT_FUNCTION_PROPERTY_MEMBER_FUNCTION)?"Member":"Static"
							,str_symbol_metamethod
							,operator_str
							,n_args_static_metamethod
						);
						return NULL;
					}


					// in the type only setters are registered in member property variable (getter is ignored)
					switch(op){

					case METAMETHOD_EQU:
					case METAMETHOD_NEQU:
					case METAMETHOD_LT:
					case METAMETHOD_LTE:
					case METAMETHOD_GT:
					case METAMETHOD_GTE:
					case METAMETHOD_NOT:
					case METAMETHOD_IN:
					case METAMETHOD_ADD:
					case METAMETHOD_SUB:
					case METAMETHOD_DIV:
					case METAMETHOD_MUL:
					case METAMETHOD_MOD:
					case METAMETHOD_AND:
					case METAMETHOD_OR:
					case METAMETHOD_XOR:
					case METAMETHOD_SHL:
					case METAMETHOD_SHR:
					case METAMETHOD_TO_STRING:
						// do nothing
						break;


					case METAMETHOD_SET:
					case METAMETHOD_ADD_ASSIGN:
					case METAMETHOD_SUB_ASSIGN:
					case METAMETHOD_MUL_ASSIGN:
					case METAMETHOD_DIV_ASSIGN:
					case METAMETHOD_MOD_ASSIGN:
					case METAMETHOD_AND_ASSIGN:
					case METAMETHOD_OR_ASSIGN:
					case METAMETHOD_XOR_ASSIGN:
					case METAMETHOD_SHL_ASSIGN:
					case METAMETHOD_SHR_ASSIGN:

						info_mp=metamethod_members.getSetterInfo(op);

						if(((_function_properties & SCRIPT_FUNCTION_PROPERTY_NATIVE_OBJECT_REF)==0) //--> script function has to have one setter function, meanwhile c ref can have more than one (due different signatures)
								&&
							(info_mp.setters!=NULL && info_mp.setters->length()>0)){
							// error already set (script functions only can be set once)
							ZS_THROW_RUNTIME_ERROR("Setter '%s::%s' already set"
									,name.toConstChar()
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
											,name.toConstChar()
											,_function_name.toConstChar()
										);
									}
								}

								it++;
							}


							if(it->name==NULL){

								ZS_THROW_RUNTIME_ERROR("Internal error registering member metamethod '%s::%s'. MetamethodHelper '%s' not supported or not implemented (3)",
									this->name.toConstChar(),
									_function_name.toConstChar(),
									_function_name.toConstChar()
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
		Vector<Symbol *> *list=this->scope->symbol_variables;

		for(
				int i = (int)(list->length()-1);
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
		Vector<Symbol *> *symbol_functions=this->scope->symbol_functions;

		int i = (int)(symbol_functions->length()-1);

		while(i >= idx_end){
			Symbol *member_symbol=symbol_functions->data()[i];
			if(strcmp(member_symbol->name.toConstChar(),_const_symbol_name)==0){
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

	const char *ScriptType::getScriptTypeName(){
		return name.toConstChar();
	}

	ScriptType::~ScriptType(){

		for(int i=0; i < allocated_member_properties->length(); i++){
			MemberProperty *mp=(MemberProperty *)allocated_member_properties->get(i);
			delete mp;
		}

		delete allocated_member_properties;
		delete base_script_type_ids;

	}
}



