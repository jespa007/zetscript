/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	bool ScriptClass::isNativeClass(){

		 return ((properties & SCRIPT_CLASS_PROPERTY_C_OBJECT_REF) != 0);
	}

	bool ScriptClass::isNativeSingletonClass(){
		 return ((properties & SCRIPT_CLASS_PROPERTY_C_OBJECT_REF) != 0)
				 	 &&
				this->c_constructor == NULL;
	}
	//------------------------------------------------------------

	ScriptClass::ScriptClass(ZetScript *_zs
			,short _idx_class
			, zs_string _class_name
			, Scope *_class_scope
			,const char *_str_class_ptr_type
			,uint16_t _properties){
		sf_field_initializer=NULL;

		str_class_ptr_type="";
		c_destructor = NULL;
		c_constructor=NULL;
		idx_function_member_constructor =ZS_IDX_UNDEFINED;
		idx_class=_idx_class;
		idx_starting_this_member_variables=0;
		idx_starting_this_member_functions=0;
		class_name=_class_name;
		class_scope=_class_scope;
		str_class_ptr_type=_str_class_ptr_type;
		allocated_member_attributes=new zs_vector();

		idx_base_classes=new zs_vector;

		// factories
		zs = _zs;
		scope_factory = zs->getScopeFactory();
		script_function_factory= zs->getScriptFunctionFactory();
		script_class_factory=zs->getScriptClassFactory();
		sf_field_initializer=NULL; // will be created after register class and register member extension (if available)
		member_properties=NULL;
		properties=_properties;

	}

	bool ScriptClass::isDerivedFrom(short idx_class){
		for(unsigned i=0; i < this->idx_base_classes->count; i++){
			if(this->idx_base_classes->items[i]==idx_class){
				return true;
			}
		}

		return false;
	}

	ScriptClass * 				ScriptClass::getScriptClass(short idx){
		return script_class_factory->getScriptClass(idx);
	}


	short					ScriptClass::getIdxClassFromItsNativeType(const char * s){
		return script_class_factory->getIdxClassFromItsNativeType(s);
	}

	ScriptClass * 			ScriptClass::getScriptClassByNativeClassPtr(const zs_string & class_type){
		return script_class_factory->getScriptClassByNativeClassPtr(class_type);
	}

	//---------------------------------------------------
	// VARIABLES
	Symbol				* 	ScriptClass::registerMemberVariable(
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

	Symbol				* 	ScriptClass::registerNativeMemberVariable(
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

	Symbol				* 	ScriptClass::registerInternalMemberVariable(
		const zs_string & symbol_name
		,unsigned short symbol_properties
		,const zs_string & str_native_type
		,zs_int ref_ptr
		,const char * file
		,short line

	){
		if(getSymbol(symbol_name)!=NULL){
			THROW_RUNTIME_ERROR("Variable \"%s\" already registered",symbol_name.c_str());
			return NULL;
		}

		Symbol *symbol=class_scope->registerSymbolVariable(
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
	// ATTRIBUTES
	Symbol *ScriptClass::registerMemberProperty(
			 const zs_string & attrib_name
			,const char * file
			,short line
	){
		Symbol *symbol_attrib=NULL;
		if((symbol_attrib=getSymbol(attrib_name)) != NULL){ // give an error  ...

			const char *what="is already defined";
			if((symbol_attrib->properties & SYMBOL_PROPERTY_MEMBER_PROPERTY)==0){

				if(symbol_attrib->properties & SYMBOL_PROPERTY_FUNCTION){
					what="it conflicts with member function";
				}else{
					what="it conflicts with member variable";
				}
			}
			THROW_SCRIPT_ERROR_FILE_LINE(file,line,"Property \"%s\" %s at [%s:%i]"
				,attrib_name.c_str()
				,what
				,zs_path::get_filename(symbol_attrib->file).c_str()
				,symbol_attrib->line
			);
		}

		symbol_attrib = class_scope->registerSymbolVariable(file,line,attrib_name);
		symbol_attrib->ref_ptr=(zs_int)(new MemberProperty(this,attrib_name));
		symbol_attrib->properties=SYMBOL_PROPERTY_MEMBER_PROPERTY;
		allocated_member_attributes->push_back(symbol_attrib->ref_ptr);

		return symbol_attrib;
	}

	Symbol				* 	ScriptClass::registerNativeMemberPropertyMetamethod(
			const zs_string & _property_name
			,ByteCodeMetamethod _byte_code_metamethod_setter
			,ScriptFunctionParam **_params
			,size_t _params_len
			,int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_getter_function_properties
			,const char * _file
			,short _line


	){
		Symbol *symbol_attrib=NULL;
		MemberProperty *ma=NULL;
		Symbol *symbol_function=NULL;

		if((symbol_attrib=getSymbol(_property_name)) == NULL){
			symbol_attrib=registerMemberProperty(_property_name,_file,_line);
		}

		ma=(MemberProperty *)symbol_attrib->ref_ptr;
		//zs_string member_property_metamethod_name=byte_code_metamethod_to_symbol_str(_metamethod)+"@"+_property_name;


		symbol_function=registerNativeMemberFunction(
				ZS_MEMBER_PROPERTY_SYMBOL_NAME_SETTER(_byte_code_metamethod_setter,_property_name),
				_params,
				_params_len,
				_idx_return_type,
				_ref_ptr,
				_symbol_getter_function_properties,
				_file,
				_line
		);

		ma->addSettter(_byte_code_metamethod_setter,(ScriptFunction *)symbol_function->ref_ptr);

		return symbol_attrib;
	}


	Symbol				* 	ScriptClass::registerNativeMemberPropertyMetamethodGetter(
			 const zs_string & _property_name
			 , ScriptFunctionParam **_params
			 ,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_getter_function_properties
			,const char *_file
			,short _line
	){

		Symbol *symbol_attrib=NULL;
		Symbol *symbol_function=NULL;

		MemberProperty *ma=NULL;
		if((symbol_attrib=getSymbol(_property_name)) == NULL){
			symbol_attrib=registerMemberProperty(_property_name,_file,_line);
		}

		ma=(MemberProperty *)symbol_attrib->ref_ptr;

		if(ma->getter != NULL){

			THROW_SCRIPT_ERROR_FILE_LINE(_file,_line,"Property \"%s\" has already a getter"
				,_property_name.c_str()
			);
		}

		symbol_function=registerNativeMemberFunction(
				ZS_MEMBER_PROPERTY_SYMBOL_NAME_GETTER+_property_name,
				_params,
				_params_len,
				_idx_return_type,
				_ref_ptr,
				_symbol_getter_function_properties,
				_file,
				_line
		);

		ma->getter=(ScriptFunction *)symbol_function->ref_ptr;

		return symbol_attrib;
	}

	/*Symbol				* 	ScriptClass::registerNativeMemberPropertyPostIncrement(
			 const zs_string & _property_name
			 , ScriptFunctionParam **_params
			 ,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_post_inc_function_properties
			,const char *_file
			,short _line
	){

		Symbol *symbol_attrib=NULL;
		Symbol *symbol_function=NULL;

		MemberProperty *ma=NULL;
		if((symbol_attrib=getSymbol(_property_name)) == NULL){
			symbol_attrib=registerMemberProperty(_property_name,_file,_line);
		}

		ma=(MemberProperty *)symbol_attrib->ref_ptr;

		if(ma->post_inc != NULL){

			THROW_SCRIPT_ERROR_FILE_LINE(_file,_line,"Property '%s' has already a post increment (aka '%s++') metamethod"
				,_property_name.c_str()
				,_property_name.c_str()
			);
		}

		symbol_function=registerNativeMemberFunction(
				ZS_MEMBER_PROPERTY_SYMBOL_NAME_POST_INC+_property_name,
				_params,
				_params_len,
				_idx_return_type,
				_ref_ptr,
				_symbol_post_inc_function_properties,
				_file,
				_line
		);

		ma->post_inc=(ScriptFunction *)symbol_function->ref_ptr;

		return symbol_attrib;
	}

	Symbol				* 	ScriptClass::registerNativeMemberPropertyPostDecrement(
			 const zs_string & _property_name
			 , ScriptFunctionParam **_params
			 ,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_post_dec_function_properties
			,const char *_file
			,short _line
	){

		Symbol *symbol_attrib=NULL;
		Symbol *symbol_function=NULL;

		MemberProperty *ma=NULL;
		if((symbol_attrib=getSymbol(_property_name)) == NULL){
			symbol_attrib=registerMemberProperty(_property_name,_file,_line);
		}

		ma=(MemberProperty *)symbol_attrib->ref_ptr;

		if(ma->post_dec != NULL){

			THROW_SCRIPT_ERROR_FILE_LINE(_file,_line,"Property '%s' has already a post decrement (aka '%s--') metamethod"
				,_property_name.c_str()
				,_property_name.c_str()
			);
		}

		symbol_function=registerNativeMemberFunction(
				ZS_MEMBER_PROPERTY_SYMBOL_NAME_POST_DEC+_property_name,
				_params,
				_params_len,
				_idx_return_type,
				_ref_ptr,
				_symbol_post_dec_function_properties,
				_file,
				_line
		);

		ma->post_dec=(ScriptFunction *)symbol_function->ref_ptr;

		return symbol_attrib;
	}

	Symbol				* 	ScriptClass::registerNativeMemberPropertyPreIncrement(
			 const zs_string & _property_name
			 , ScriptFunctionParam **_params
			 ,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_pre_inc_function_properties
			,const char *_file
			,short _line
	){

		Symbol *symbol_attrib=NULL;
		Symbol *symbol_function=NULL;

		MemberProperty *ma=NULL;
		if((symbol_attrib=getSymbol(_property_name)) == NULL){
			symbol_attrib=registerMemberProperty(_property_name,_file,_line);
		}

		ma=(MemberProperty *)symbol_attrib->ref_ptr;

		if(ma->pre_inc != NULL){

			THROW_SCRIPT_ERROR_FILE_LINE(_file,_line,"Property '%s' has already a post increment (aka '++%s') metamethod"
				,_property_name.c_str()
				,_property_name.c_str()
			);
		}

		symbol_function=registerNativeMemberFunction(
				ZS_MEMBER_PROPERTY_SYMBOL_NAME_PRE_INC+_property_name,
				_params,
				_params_len,
				_idx_return_type,
				_ref_ptr,
				_symbol_pre_inc_function_properties,
				_file,
				_line
		);

		ma->pre_inc=(ScriptFunction *)symbol_function->ref_ptr;

		return symbol_attrib;
	}

	Symbol				* 	ScriptClass::registerNativeMemberPropertyPreDecrement(
			 const zs_string & _property_name
			 , ScriptFunctionParam **_params
			 ,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_pre_dec_function_properties
			,const char *_file
			,short _line
	){

		Symbol *symbol_attrib=NULL;
		Symbol *symbol_function=NULL;

		MemberProperty *ma=NULL;
		if((symbol_attrib=getSymbol(_property_name)) == NULL){
			symbol_attrib=registerMemberProperty(_property_name,_file,_line);
		}

		ma=(MemberProperty *)symbol_attrib->ref_ptr;

		if(ma->post_dec != NULL){

			THROW_SCRIPT_ERROR_FILE_LINE(_file,_line,"Property '%s' has already a pre decrement (aka '--%s') metamethod"
				,_property_name.c_str()
				,_property_name.c_str()
			);
		}

		symbol_function=registerNativeMemberFunction(
				ZS_MEMBER_PROPERTY_SYMBOL_NAME_PRE_DEC+_property_name,
				_params,
				_params_len,
				_idx_return_type,
				_ref_ptr,
				_symbol_pre_dec_function_properties,
				_file,
				_line
		);

		ma->pre_dec=(ScriptFunction *)symbol_function->ref_ptr;

		return symbol_attrib;
	}

	template <typename F>
	void ScriptClass::registerNativeMemberPropertyAddSet(
			const zs_string & _property_name
		,F _ptr_function
		, const char *registered_file
		,short registered_line
	){

	}


	Symbol				* 	ScriptClass::registerNativeMemberPropertyAddSet(
		const zs_string & _property_name
		,ScriptFunctionParam **_params
		,size_t _params_len
		, int _idx_return_type
		,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
		,unsigned short _symbol_add_set_function_properties
		,const char * _file
		,short _line

	){
		return NULL;
	}

	// SUB SET
	template <typename F>
	void ScriptClass::registerNativeMemberPropertySubSet(
			const zs_string & _property_name
		,F _ptr_function
		, const char *registered_file
		,short registered_line
	){

	}

	Symbol				* 	ScriptClass::registerNativeMemberPropertySubSet(
		const zs_string & _property_name
		,ScriptFunctionParam **_params
		,size_t _params_len
		, int _idx_return_type
		,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
		,unsigned short _symbol_sub_set_function_properties
		,const char * _file
		,short _line

	){
		return NULL;
	}

	// MUL SET
	template <typename F>
	void ScriptClass::registerNativeMemberPropertyMulSet(
			const zs_string & _property_name
		,F _ptr_function
		, const char *registered_file
		,short registered_line
	){

	}


	Symbol				* 	ScriptClass::registerNativeMemberPropertyMulSet(
		const zs_string & _property_name
		,ScriptFunctionParam **_params
		,size_t _params_len
		, int _idx_return_type
		,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
		,unsigned short _symbol_mul_set_function_properties
		,const char * _file
		,short _line

	){
		return NULL;
	}

	// DIV SET
	template <typename F>
	void ScriptClass::registerNativeMemberPropertyDivSet(
			const zs_string & _property_name
		,F _ptr_function
		, const char *registered_file
		,short registered_line
	){

	}


	Symbol				* 	ScriptClass::registerNativeMemberPropertyDivSet(
		const zs_string & _property_name
		,ScriptFunctionParam **_params
		,size_t _params_len
		, int _idx_return_type
		,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
		,unsigned short _symbol_div_set_function_properties
		,const char * _file
		,short _line

	){
		return NULL;
	}

	// MOD SET
	template <typename F>
	void ScriptClass::registerNativeMemberPropertyModSet(
			const zs_string & _property_name
		,F _ptr_function
		, const char *registered_file
		,short registered_line
	){

	}


	Symbol				* 	ScriptClass::registerNativeMemberPropertyModSet(
		const zs_string & _property_name
		,ScriptFunctionParam **_params
		,size_t _params_len
		, int _idx_return_type
		,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
		,unsigned short _symbol_mod_set_function_properties
		,const char * _file
		,short _line

	){
		return NULL;
	}

	// AND SET
	template <typename F>
	void ScriptClass::registerNativeMemberPropertyAndSet(
			const zs_string & _property_name
		,F _ptr_function
		, const char *registered_file
		,short registered_line
	){

	}


	Symbol				* 	ScriptClass::registerNativeMemberPropertyAndSet(
		const zs_string & _property_name
		,ScriptFunctionParam **_params
		,size_t _params_len
		, int _idx_return_type
		,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
		,unsigned short _symbol_and_set_function_properties
		,const char * _file
		,short _line

	){
		return NULL;
	}

	// OR SET
	template <typename F>
	void ScriptClass::registerNativeMemberPropertyOrSet(
			const zs_string & _property_name
		,F _ptr_function
		, const char *registered_file
		,short registered_line
	){

	}


	Symbol				* 	ScriptClass::registerNativeMemberPropertyOrSet(
		const zs_string & _property_name
		,ScriptFunctionParam **_params
		,size_t _params_len
		, int _idx_return_type
		,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
		,unsigned short _symbol_or_set_function_properties
		,const char * _file
		,short _line

	){
		return NULL;
	}

	// XOR SET
	template <typename F>
	void ScriptClass::registerNativeMemberPropertyXorSet(
			const zs_string & _property_name
		,F _ptr_function
		, const char *registered_file
		,short registered_line
	){

	}


	Symbol				* 	ScriptClass::registerNativeMemberPropertyXorSet(
		const zs_string & _property_name
		,ScriptFunctionParam **_params
		,size_t _params_len
		, int _idx_return_type
		,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
		,unsigned short _symbol_xor_set_function_properties
		,const char * _file
		,short _line

	){
		return NULL;
	}

	// SHL SET
	template <typename F>
	void ScriptClass::registerNativeMemberPropertyShlSet(
			const zs_string & _property_name
		,F _ptr_function
		, const char *registered_file
		,short registered_line
	){

	}


	Symbol				* 	ScriptClass::registerNativeMemberPropertyShlSet(
		const zs_string & _property_name
		,ScriptFunctionParam **_params
		,size_t _params_len
		, int _idx_return_type
		,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
		,unsigned short _symbol_shl_set_function_properties
		,const char * _file
		,short _line

	){
		return NULL;
	}

	// SHR SET
	template <typename F>
	void ScriptClass::registerNativeMemberPropertyShrSet(
			const zs_string & _property_name
		,F _ptr_function
		, const char *registered_file
		,short registered_line
	){

	}


	Symbol				* 	ScriptClass::registerNativeMemberPropertyShrSet(
		const zs_string & _property_name
		,ScriptFunctionParam **_params
		,size_t _params_len
		, int _idx_return_type
		,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
		,unsigned short _symbol_shr_set_function_properties
		,const char * _file
		,short _line

	){
		return NULL;
	}
*/
	//---------------------------------------------------
	// FUNCTIONS

	Symbol				* 	ScriptClass::registerMemberFunction(
			 const zs_string & _function_name
			 , ScriptFunctionParam **_params
			 ,size_t _params_len
			, unsigned short _function_properties
			,const char * _file
			, short _line
	){
		return registerInternalMemberFunction(
				 _function_name
				, _params
				,_params_len
				, _function_properties
				,ZS_IDX_UNDEFINED
				,0
				, _file
				,  _line

		);
	}

	Symbol				* 	ScriptClass::registerNativeMemberFunction(
			const zs_string & _function_name
			 , ScriptFunctionParam **_params
			 ,size_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // script function
			, unsigned short _symbol_properties
			,const char * _file
			, short _line

	){
		return registerInternalMemberFunction(
				 _function_name
				,_params
				,_params_len
				,_symbol_properties
				,_idx_return_type
				,(zs_int)_ref_ptr
				, _file
				, _line

		);

	}

	Symbol * ScriptClass::registerInternalMemberFunction(
		 const zs_string & _function_name
		 , ScriptFunctionParam **_params
		 ,size_t _params_len
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
					THROW_RUNTIME_ERROR("Function member \"%s\" is already defined at [%s:%i]"
						,_function_name.c_str()
						,zs_path::get_filename(_file).c_str()
						,_line
						,zs_path::get_filename(existing_symbol->file).c_str()
						,existing_symbol->line
					);
				}
				return NULL;
			}
		}

		Symbol *function_symbol =  script_function_factory->newScriptFunction(
				//---- Register data
				class_scope
				,_file
				,_line
				//---- Function data
				,idx_class 				// idx class which belongs to...
				,_function_name
				,_params
				,_params_len
				,_idx_return_type
				,_ref_ptr // c function
				,_function_properties
		);

		ScriptFunction *sf_current=(ScriptFunction *)function_symbol->ref_ptr;

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
		if(_function_name == this->class_name){ //  FUNCTION_MEMBER_CONSTRUCTOR_NAME
			idx_function_member_constructor = function_symbol->idx_position;
			sf_current->properties|=FUNCTION_PROPERTY_CONSTRUCTOR;
		}
		else{
			// check metamethod function...
			for(unsigned i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
				if(ZS_STRCMP(byte_code_metamethod_to_symbol_str((ByteCodeMetamethod)i),==,_function_name.c_str())){
					ByteCodeMetamethod op=(ByteCodeMetamethod)i;
					const char *byte_code_metamethod_operator_str=byte_code_metamethod_to_operator_str(op);
					const char *str_symbol_metamethod=byte_code_metamethod_to_symbol_str(op);
					int n_args_static_metamethod=byte_code_metamethod_get_num_arguments(op); // expected params for static function, n_args -1 else
					int this_arg=0;

					Symbol *symbol_result=NULL;

					// can be one parameter or 0 params...
					if(byte_code_metamethod_should_be_static(op) && ((_function_properties & FUNCTION_PROPERTY_STATIC)==0)){
						THROW_RUNTIME_ERROR("Metamethod '%s::%s' has to be declared as static instead of member"
							,class_name.c_str()
							,_function_name.c_str()
						);
						return NULL;
					}else if((byte_code_metamethod_should_be_static(op)==false) && ((_function_properties & FUNCTION_PROPERTY_STATIC))){
						THROW_RUNTIME_ERROR("Metamethod '%s::%s' has to be declared as member instead of static"
							,class_name.c_str()
							,_function_name.c_str()
						);
						return NULL;
					}

					// native
					if((_function_properties & FUNCTION_PROPERTY_C_OBJECT_REF)){ // if-native
						if(op == BYTE_CODE_METAMETHOD_TO_STRING && !(_idx_return_type == IDX_TYPE_STRING_PTR_C || _idx_return_type == IDX_TYPE_STRING_C) ){
							THROW_RUNTIME_ERROR("Metamethod '%s::%s' should return zs_string * or zs_string *"
								,class_name.c_str()
								,_function_name.c_str()
							);
							return NULL;
						}

						if(_function_properties & FUNCTION_PROPERTY_MEMBER_FUNCTION){
							this_arg=1;
						}
					}

					if(function_symbol->n_params< (n_args_static_metamethod+this_arg)){ // non-static functions pass this object as first parameter
						THROW_RUNTIME_ERROR("%s metamethod '%s' (aka '%s') should have at least %i parameter/s"
							,(_function_properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)?"Member":"Static"
							,str_symbol_metamethod
							,byte_code_metamethod_operator_str
							,n_args_static_metamethod
						);
						return NULL;
					}


					// everything ok
					if(/*op==BYTE_CODE_METAMETHOD_GET || */op==BYTE_CODE_METAMETHOD_SET){
						if(member_properties == NULL){
							member_properties = new MemberProperty(this,class_name);
						}

						MemberPropertyInfo info_mp=member_properties->getInfo(op);

						if(info_mp->setters!=NULL && info_mp->setters->count>0){
							// error already set (script functions only can be set once)
							THROW_RUNTIME_ERROR("Setter '%s::%s' already set"
									,class_name.c_str()
									,info_mp.str_byte_code_metamethod);

							return NULL;
						}
						member_properties->addSetter(op,(ScriptFunction *)function_symbol->ref_ptr);
					}

					break;
				}
			}
		}

		return function_symbol;
	}
	//---------------------------------------------------------
	Symbol *    ScriptClass::getSymbolVariableMember(const zs_string & symbol_name, bool include_inherited_symbols){
		int idx_end=include_inherited_symbols==true?0:idx_starting_this_member_variables;
		zs_vector *list=this->class_scope->symbol_variables;

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

	Symbol *    ScriptClass::getSymbolMemberFunction(const zs_string & symbol_name, char n_params, bool include_inherited_symbols){
		bool only_symbol=n_params<0;
		int idx_end=include_inherited_symbols==true?0:idx_starting_this_member_functions;
		zs_vector *symbol_functions=this->class_scope->symbol_functions;

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

	Symbol				* 	ScriptClass::getSymbol(const zs_string & symbol_name, char n_params,bool include_inherited_symbols){

		Symbol *symbol=NULL;

		if((symbol=getSymbolVariableMember(symbol_name,include_inherited_symbols))==NULL){
			symbol=getSymbolMemberFunction(symbol_name,n_params,include_inherited_symbols);
		}

		return symbol;
	}

	const char *ScriptClass::getClassName(){
		if(idx_class==IDX_TYPE_ZS_INT_C){
			return "int";
		}else if(idx_class==IDX_TYPE_ZS_FLOAT_C){
			return "float";
		}

		return class_name.c_str();
	}



	ScriptClass::~ScriptClass(){

		for(unsigned i=0; i < allocated_member_attributes->count; i++){
			MemberProperty *ma=(MemberProperty *)allocated_member_attributes->items[i];
			delete ma;
		}

		delete allocated_member_attributes;
		delete idx_base_classes;

		if(member_properties != NULL){
			delete member_properties;
		}
	}
}



