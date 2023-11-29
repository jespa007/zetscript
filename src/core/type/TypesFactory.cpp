/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"


#define 				MAX_REGISTER_CLASSES 				100
#define 				MAIN_SCRIPT_CLASS_NAME 				"@MainClass"

#define SCF_REGISTER_STRUCT(_type_id, _class_type)\
	if(types->length()!=_type_id){\
		ZS_THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(_class_type));\
		return;\
	}\
	registerType<_class_type>(ZS_STR(_class_type));


#define SCF_REGISTER_CLASS(_type_id, _type_name, _class_type )\
	if(types->length()!=_type_id){\
		ZS_THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(_class_type));\
		return;\
	}\
	PRAGMA_PUSH\
	PRAGMA_DISABLE_WARNING(4127)\
	if(_type_id >= TYPE_ID_MAX){\
		PRAGMA_POP\
		ZS_THROW_RUNTIME_ERROR("The type to bind '%s' should be a built in type",ZS_STR(_class_type));\
		return;\
	}\
	registerType<_class_type>(_type_name,_class_type##_New,_class_type##_Delete);

#define SCF_REGISTER_SINGLETON_CLASS(_type_id, _class_type )\
	if(types->length()!=_type_id){\
		ZS_THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(_class_type));\
		return;\
	}\
	registerType<_class_type>(ZS_STR(_class_type));

#define SCF_REGISTER_SINGLETON_NAME_CLASS(_type_id, _type_name, _native_type )\
	if(types->length()!=_type_id){\
		ZS_THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(_native_type));\
		return;\
	}\
	registerType<_native_type>(_type_name);

#define SCF_REGISTER_NATIVE_TYPE(_type_id, _native_type)\
	if(types->length()!=_type_id){\
		ZS_THROW_RUNTIME_ERROR("Error initializing C built in type: '%s'",ZS_STR(_native_type));\
		return;\
	}else{\
		Type *type=registerType(typeid(_native_type).name(),"",ZS_TYPE_PROPERTY_NON_INSTANTIABLE);\
		type->scope->properties|=ZS_SCOPE_PROPERTY_IS_C_OBJECT_REF;\
		type->properties=ZS_TYPE_PROPERTY_C_OBJECT_REF;\
		type->native_name=(typeid(_native_type).name());\
	}

#define SCF_REGISTER_NATIVE_CUSTOM_TYPE(_type_id, _type_name, _native_type)\
	if(types->length()!=_type_id){\
		ZS_THROW_RUNTIME_ERROR("Error initializing C built in type: '%s'",ZS_STR(_native_type));\
		return;\
	}else{\
		Type *type=registerType(_type_name);\
		type->scope->properties|=ZS_SCOPE_PROPERTY_IS_C_OBJECT_REF;\
		type->properties=ZS_TYPE_PROPERTY_C_OBJECT_REF;\
		type->native_name=(typeid(_native_type).name());\
	}

#define SCF_REGISTER_TYPE(_type_id,_type_name)\
	if(types->length()!=_type_id){\
		ZS_THROW_RUNTIME_ERROR("Error initializing built in type: '%s'",_type_name);\
		return;\
	}else{\
		registerType(_type_name);\
	}

namespace zetscript{


	const char * k_str_void_type =typeid(void).name();
	const char * k_str_int_type_ptr=typeid(zs_int *).name();
	const char * k_str_const_int_type_ptr=typeid(const zs_int *).name();
	const char * k_str_float_type_ptr=typeid(zs_float *).name();
	const char * k_str_const_float_type_ptr=typeid(const zs_float *).name();
	const char * k_str_string_type_ptr=typeid(String *).name();
	const char * k_str_char_type_ptr=typeid(char *).name();
	const char * k_str_const_char_type_ptr=typeid(const char *).name();
	const char * k_str_bool_type_ptr=typeid(bool *).name();
	const char * k_str_const_bool_type_ptr=typeid(const bool *).name();
	const char * k_str_int_type=typeid(zs_int).name();
	const char * k_str_float_type=typeid(zs_float).name();
	const char * k_str_bool_type=typeid(bool).name();
	const char * k_str_stack_element_type=typeid(StackElement).name();
	const char * k_str_zettype_ptr=typeid(ZetScript *).name();

	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(DictionaryObject)
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(StringObject)
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ArrayObject)

	TypesFactory::TypesFactory(ZetScript *_zs){
		zs = _zs;
		vm = _zs->getVirtualMachine();
		scope_factory = this->zs->getScopeFactory();
		script_function_factory= this->zs->getFunctionFactory();
		main_function=NULL;
		main_object=NULL;
		types=new Vector<Type *>;
		main_object=NULL;
		main_function=NULL;
		idx_clear_checkpoint=0;
	}

	void TypesFactory::init(){
		String error;

		// FunctionFactory has to be created
		main_object=registerType(MAIN_SCRIPT_CLASS_NAME); // 0
		ZS_MAIN_SCOPE(this)->owner_type=main_object;

		Symbol *symbol_main_function=main_object->registerMemberFunction(
				ZS_MAIN_FUNCTION_NAME
		);

		main_function=(Function *)symbol_main_function->ref_ptr;
		main_function->scope=ZS_MAIN_SCOPE(this);

		idx_clear_checkpoint=1; // by default restore till main type.
	}

	ZetScript *ptrToZetScriptPtr(ZetScript *_zs, zs_int ptr){
		ZS_UNUSUED_PARAM(_zs);
		return (ZetScript *)ptr;
	}

	zs_int intMaxValue(ZetScript *_zs){
		ZS_UNUSUED_PARAM(_zs);
		return INTPTR_MAX;
	}

	zs_float floatMaxValue(ZetScript *_zs){
		ZS_UNUSUED_PARAM(_zs);
		return FLT_MAX;
	}

	zs_float parseFloat(ZetScript *_zs,zs_int number){
		ZS_UNUSUED_PARAM(_zs);
		return (zs_float)(number);
	}

	zs_float parseFloat(ZetScript *_zs,zs_float *number){
		ZS_UNUSUED_PARAM(_zs);
		return *number;
	}

	zs_float parseFloat(ZetScript *_zs,String  *number_str){
		ZS_UNUSUED_PARAM(_zs);
		zs_float result=0;
		zs_float *result_ptr=Float::parse(*number_str);

		if(result_ptr != NULL){
			result=*result_ptr;
			delete result_ptr;
		}

		return result;
	}

	zs_int parseInt(ZetScript *_zs,zs_int number){
		ZS_UNUSUED_PARAM(_zs);
		return number;
	}

	zs_int parseInt(ZetScript *_zs,zs_float *number){
		ZS_UNUSUED_PARAM(_zs);
		return (zs_int)(*number);
	}

	zs_int parseInt(ZetScript *_zs,String  *number_str){
		ZS_UNUSUED_PARAM(_zs);
		zs_int result=0;
		zs_int *result_ptr=Integer::parse(*number_str);
		if(result_ptr!=NULL){
			result=*result_ptr;
			delete result_ptr;
		}
		return result;
	}

	bool isNumber(ZetScript *_zs,StackElement *_stk){
		ZS_UNUSUED_PARAM(_zs);
		return (_stk->properties & (STACK_ELEMENT_PROPERTY_INT | STACK_ELEMENT_PROPERTY_FLOAT)) != 0;
	}

	void TypesFactory::setup(){

		// !!!
		// !!! START REGISTER BUILT IN CLASSES AND TYPES
		// !!! WARNING IT SHOULD BE IN THE SAME ORDER AS IS DEFINED IN COMMON.H
		// !!!

		// primitives
		SCF_REGISTER_TYPE(TYPE_ID_UNDEFINED,ZS_TYPE_NAME_UNDEFINED);
		SCF_REGISTER_TYPE(TYPE_ID_NULL,ZS_TYPE_NAME_NULL);
		SCF_REGISTER_NATIVE_TYPE(TYPE_ID_VOID_C,void);
		SCF_REGISTER_NATIVE_CUSTOM_TYPE(TYPE_ID_INT_C,ZS_TYPE_NAME_INT,zs_int);
		SCF_REGISTER_NATIVE_TYPE(TYPE_ID_INT_PTR_C,zs_int *);
		SCF_REGISTER_NATIVE_TYPE(TYPE_ID_CHAR_PTR_C,char *);
		SCF_REGISTER_NATIVE_TYPE(TYPE_ID_CONST_CHAR_PTR_C,const char *);
		SCF_REGISTER_NATIVE_TYPE(TYPE_ID_STRING_C,String);
		SCF_REGISTER_NATIVE_TYPE(TYPE_ID_STRING_PTR_C,String *);
		SCF_REGISTER_NATIVE_CUSTOM_TYPE(TYPE_ID_BOOL_C,ZS_TYPE_NAME_BOOL,bool);
		SCF_REGISTER_NATIVE_TYPE(TYPE_ID_BOOL_PTR_C,bool *);
		SCF_REGISTER_NATIVE_CUSTOM_TYPE(TYPE_ID_FLOAT_C,ZS_TYPE_NAME_FLOAT,zs_float);
		SCF_REGISTER_NATIVE_TYPE(TYPE_ID_FLOAT_PTR_C,zs_float *);
		SCF_REGISTER_NATIVE_TYPE(TYPE_ID_CONST_FLOAT_PTR_C,const zs_float *);

		// estructures
		SCF_REGISTER_STRUCT(TYPE_ID_STACK_ELEMENT,StackElement);

		//------------------------
		// BUILT-IN SCRIPT OBJECTS
		// It self Script object
		SCF_REGISTER_SINGLETON_NAME_CLASS(TYPE_ID_FUNCTION,ZS_TYPE_NAME_FUNCTION,Function);
		SCF_REGISTER_SINGLETON_NAME_CLASS(TYPE_ID_OBJECT_VAR_REF,ZS_TYPE_NAME_OBJECT_VAR_REF,VarRefObject);
		SCF_REGISTER_SINGLETON_NAME_CLASS(TYPE_ID_OBJECT_FUNCTION_MEMBER,ZS_TYPE_NAME_OBJECT_FUNCTION_MEMBER,MemberFunctionObject);
		SCF_REGISTER_SINGLETON_CLASS(TYPE_ID_OBJECT_ZETSCRIPT,ZetScript);

		SCF_REGISTER_CLASS(TYPE_ID_OBJECT_STRING,ZS_TYPE_NAME_OBJECT_STRING,StringObject);

		// Script object iterators
		SCF_REGISTER_SINGLETON_NAME_CLASS(TYPE_ID_OBJECT_ITERATOR_STRING,ZS_TYPE_NAME_OBJECT_ITERATOR_STRING,StringIteratorObject);
		SCF_REGISTER_SINGLETON_NAME_CLASS(TYPE_ID_OBJECT_ITERATOR_ARRAY,ZS_TYPE_NAME_OBJECT_ITERATOR_ARRAY,ArrayIteratorObject);
		SCF_REGISTER_SINGLETON_NAME_CLASS(TYPE_ID_OBJECT_ITERATOR_OBJECT,ZS_TYPE_NAME_OBJECT_ITERATOR_OBJECT,ObjectIteratorObject);


		// BUILT-IN SCRIPT OBJECTS
		//------------------------
		// BUILT-IN SCRIPT OBJECTS CLASSES
		SCF_REGISTER_CLASS(TYPE_ID_OBJECT_ARRAY,ZS_TYPE_NAME_OBJECT_ARRAY,ArrayObject);
		SCF_REGISTER_CLASS(TYPE_ID_DICTIONARY_OBJECT,ZS_TYPE_NAME_DICTIONARY_OBJECT,DictionaryObject);
		SCF_REGISTER_SINGLETON_NAME_CLASS(TYPE_ID_OBJECT_CLASS,ZS_TYPE_NAME_OBJECT_CLASS,ClassObject);
		// it needs script object type to have zetscript reference
		// BUILT-IN SCRIPT OBJECTS CLASSES
		//------------------------

		// !!!
		// !!! END REGISTER BUILT IN CLASSES AND TYPES
		// !!! WARNING IT SHOULD BE IN THE SAME ORDER AS IS DEFINED IN COMMON.H
		// !!!

		//------------------------------------------------------------------------------------------------------------
		// Let's register functions,...
		// register c function's

		zs->registerFunction("ptrToZetScriptPtr",ptrToZetScriptPtr);

		Type *integer_type=this->getType(ZS_TYPE_NAME_INT);
		integer_type->registerConstMemberProperty("MAX_VALUE",intMaxValue);
		integer_type->registerStaticMemberFunction("parse",static_cast<zs_int (*)(ZetScript *,zs_float *)>(parseInt));
		integer_type->registerStaticMemberFunction("parse",static_cast<zs_int (*)(ZetScript *,zs_int )>(parseInt));
		integer_type->registerStaticMemberFunction("parse",static_cast<zs_int (*)(ZetScript *,String *)>(parseInt));

		Type *float_type=this->getType(ZS_TYPE_NAME_FLOAT);
		float_type->registerConstMemberProperty("MAX_VALUE",floatMaxValue);
		float_type->registerStaticMemberFunction("parse",static_cast<zs_float (*)(ZetScript *,zs_int )>(parseFloat));
		float_type->registerStaticMemberFunction("parse",static_cast<zs_float (*)(ZetScript *,zs_float *)>(parseFloat));
		float_type->registerStaticMemberFunction("parse",static_cast<zs_float (*)(ZetScript *,String *)>(parseFloat));


		// Global symbols
		zs->registerFunction("isNumber",isNumber);

		//-------------------------
		// Wrap functions

		//---------------------------------------------
		// String

		// properties
		registerMemberPropertyMetamethod<StringObject>("length","_get",StringObjectZs_length);

		// members
		registerStaticMemberFunction<StringObject>("formatNative",StringObject::format);
		registerMemberFunction<StringObject>("eraseAt",StringObjectZs_eraseAt);
		registerMemberFunction<StringObject>("insertAt",static_cast<void (*)(ZetScript *_zs,StringObject *so, zs_int, zs_int )>(StringObjectZs_insertAt));
		registerMemberFunction<StringObject>("insertAt",static_cast<void (*)(ZetScript *_zs,StringObject *so, zs_int, String *)>(StringObjectZs_insertAt));
		registerMemberFunction<StringObject>("clear",StringObjectZs_clear);
		registerMemberFunction<StringObject>("replace",StringObjectZs_replace);
		registerMemberFunction<StringObject>("split",static_cast<ArrayObject * (*)(ZetScript *_zs,StringObject *so, zs_int )>(StringObjectZs_split));
		registerMemberFunction<StringObject>("split",static_cast<ArrayObject * (*)(ZetScript *_zs,StringObject *so, String *)>(StringObjectZs_split));
		registerMemberFunction<StringObject>("contains",static_cast<bool (*)(ZetScript *_zs,StringObject *so, String *)>(&StringObjectZs_contains));
		registerMemberFunction<StringObject>("contains",static_cast<bool (*)(ZetScript *_zs,StringObject *so, zs_int )>(&StringObjectZs_contains));

		registerMemberFunction<StringObject>("indexOf",static_cast<zs_int (*)(ZetScript *_zs,StringObject *so, String *)>(&StringObjectZs_indexOf));
		registerMemberFunction<StringObject>("indexOf",static_cast<zs_int (*)(ZetScript *_zs,StringObject *so, zs_int )>(&StringObjectZs_indexOf));

		registerMemberFunction<StringObject>("startsWith",StringObjectZs_startsWith);
		registerMemberFunction<StringObject>("endsWith",StringObjectZs_endsWith);
		registerMemberFunction<StringObject>("substring",static_cast<StringObject * (*)(ZetScript *_zs,StringObject *so, zs_int,zs_int )>(&StringObjectZs_substring));
		registerMemberFunction<StringObject>("substring",static_cast<StringObject * (*)(ZetScript *_zs,StringObject *so, zs_int )>(&StringObjectZs_substring));
		registerMemberFunction<StringObject>("append",static_cast<void (*)(ZetScript *,StringObject *, StringObject *)>(&StringObjectZs_append));
		registerMemberFunction<StringObject>("append",static_cast<void (*)(ZetScript *,StringObject *, zs_int )>(&StringObjectZs_append));
		registerMemberFunction<StringObject>("toLowerCase",StringObjectZs_toLowerCase);
		registerMemberFunction<StringObject>("toUpperCase",StringObjectZs_toUpperCase);

		registerMemberFunction<StringObject>("_iter",StringObjectZs_iter);

		// IteratorString
		registerMemberFunction<StringIteratorObject>("_next",StringIteratorObjectZs_next);
		registerMemberFunction<StringIteratorObject>("_end",StringIteratorObjectZs_end);
		registerMemberFunction<StringIteratorObject>("_get",StringIteratorObjectZs_get);

		//---------------------------------------------
		// Array

		// properties
		registerMemberPropertyMetamethod<ArrayObject>("length","_get",&ArrayObjectZs_length);

		// members
		registerMemberFunction<ArrayObject>("push",&ArrayObjectZs_push);
		registerMemberFunction<ArrayObject>("pop",&ArrayObjectZs_pop);
		registerMemberFunction<ArrayObject>("insertAt",&ArrayObjectZs_insertAt);
		registerMemberFunction<ArrayObject>("eraseAt",&ArrayObjectZs_eraseAt);
		registerMemberFunction<ArrayObject>("clear",&ArrayObjectZs_clear);
		registerMemberFunction<ArrayObject>("join",&ArrayObjectZs_join);
		registerMemberFunction<ArrayObject>("extend",&ArrayObjectZs_extend);
		registerStaticMemberFunction<ArrayObject>("concat",&ArrayObjectZs_concat);
		registerMemberFunction<ArrayObject>("contains",&ArrayObjectZs_contains);

		registerMemberFunction<ArrayObject>("_iter",&ArrayObjectZs_iter);
		registerStaticMemberFunction<ArrayObject>("_equ",&ArrayObjectZs_equal);
		//registerStaticMemberFunction<ArrayObject>("_add",&ArrayObjectZs_equal_add);

		// IteratorArray
		registerMemberFunction<ArrayIteratorObject>("_next",ArrayIteratorObjectZs_next);
		registerMemberFunction<ArrayIteratorObject>("_end",ArrayIteratorObjectZs_end);
		registerMemberFunction<ArrayIteratorObject>("_get",ArrayIteratorObjectZs_get);


		//---------------------------------------------
		// Object
		registerStaticMemberFunction<DictionaryObject>("clear",&ObjectObjectZs_clear);
		registerStaticMemberFunction<DictionaryObject>("erase",&ObjectObjectZs_erase);
		registerStaticMemberFunction<DictionaryObject>("contains",&ObjectObjectZs_contains);
		registerStaticMemberFunction<DictionaryObject>("extend",&ObjectObjectZs_extend);
		registerStaticMemberFunction<DictionaryObject>("concat",DictionaryObject::concat);
		registerStaticMemberFunction<DictionaryObject>("keys",ObjectObjectZs_keys);
		registerStaticMemberFunction<DictionaryObject>("_iter",ObjectObjectZs_iter);

		// IteratorObject
		registerMemberFunction<ObjectIteratorObject>("_next",ObjectIteratorObjectZs_next);
		registerMemberFunction<ObjectIteratorObject>("_end",ObjectIteratorObjectZs_end);
		registerMemberFunction<ObjectIteratorObject>("_get",ObjectIteratorObjectZs_get);

		zs->saveState();
	}

	void TypesFactory::clear(short _idx_start){
		short idx_start = _idx_start == ZS_UNDEFINED_IDX ?  idx_clear_checkpoint:_idx_start;
		for(
			int v=types->length()-1;
			v > idx_start; // avoid main type
			v--
		){
			Type * type = (Type *)types->get(v);
			delete type;
			types->pop();
		}
	}

	void TypesFactory::saveState(){
		idx_clear_checkpoint = types->length()-1;
	}

	void TypesFactory::checkTypeName(const String & _str_type){

		if(types->length()>=MAX_REGISTER_CLASSES){
			ZS_THROW_RUNTIME_ERROR("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES);
		}

		if(_str_type.empty()){
			ZS_THROW_RUNTIME_ERRORF("Class name empty");
		}

		if(zs->getFunctionFactory()->getFunctions()->length() > 0){
			Symbol *main_function_symbol=NULL;
			if((main_function_symbol=scope_factory->getMainScope()->getSymbol(
					_str_type
					,ZS_NO_PARAMS_SYMBOL_ONLY,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN
			))!=NULL){
				ZS_THROW_RUNTIME_ERROR("Type name '%s' is already defined %s"
						,_str_type.toConstChar()
						,main_function_symbol->file!=NULL && strcmp(main_function_symbol->file,"")!=0 && main_function_symbol->line!=-1?
							StringUtils::format("at " ZS_FORMAT_FILE_LINE
								,Path::getFilename(main_function_symbol->file).toConstChar()
								,main_function_symbol->line
							).toConstChar():""
				);
			}
		}
	}

	Type * TypesFactory::registerType(
			const String & _str_type
			 ,const String & _base_class_name
			 ,uint16_t _properties
			 ,const char * _file
			 , short _line
	){
		TypeId  new_type_id;
		Type *type=NULL;

		checkTypeName(_str_type);

		if((new_type_id = getTypeId(_str_type))==ZS_UNDEFINED_IDX){ // check whether is local var registered scope ...
			uint16_t properties_register_scope=REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN;
			new_type_id=types->length();

			// To avoid built-int conflict bool type
			if(
					new_type_id == TYPE_ID_BOOL_C
					|| new_type_id == TYPE_ID_UNDEFINED
					|| new_type_id == TYPE_ID_NULL
					|| new_type_id == TYPE_ID_INT_C
					|| new_type_id == TYPE_ID_FLOAT_C
			){
				properties_register_scope|=REGISTER_SCOPE_NO_CHECK_CLASS_SYMBOLS;
			}
			// BYTE_CODE_NEW SCOPE C and register ...
			Scope * scope_class = ZS_NEW_SCOPE(this,ZS_UNDEFINED_IDX,NULL, ZS_SCOPE_PROPERTY_IS_SCOPE_CLASS);

			// register symbol on main scope...

			Symbol *symbol=ZS_MAIN_SCOPE(this)->registerSymbolType(_file,_line,_str_type,properties_register_scope);

			type = new Type(this->zs,new_type_id, _str_type, scope_class,ZS_TYPE_NAME_OBJECT,_properties);
			scope_class->setTypeOwner(type);
			symbol->ref_ptr=(zs_int)type;

			//type->native_name = ZS_TYPE_NAME_OBJECT;

			types->push(type);

			if(_base_class_name != ""){

				Type *base_type=NULL;

				if(type->base_type_ids->length() > 0){
					Type *match_class=getType(type->base_type_ids->get(0));
					ZS_THROW_RUNTIME_ERROR("Type '%s' already is inherited from '%s'"
							,_str_type.toConstChar()
							,match_class->name.toConstChar());
				}

				if((base_type = getType(_base_class_name)) == NULL){
					ZS_THROW_RUNTIME_ERROR("Type '%s' not registered",_base_class_name.toConstChar());
				}

				/*if(base_type->isStatic()){
					ZS_THROW_RUNTIME_ERROR("Class '%s' cannot extend from '%s' because is static. To allow extension register with 'bindNativeType' instead of 'registerClass'"
						,_str_type.toConstChar()
						,_base_class_name.toConstChar()
					);
				}*/


				// 1. extend all symbols from base type
				Vector<Symbol *> *symbol_functions=base_type->scope->symbol_functions;
				for(int i=0; i < symbol_functions->length(); i++){
					Symbol *symbol_src=(Symbol *)symbol_functions->get(i);
					Symbol *symbol_dst=scope_class->registerSymbolFunction(
							symbol_src->file
							,symbol_src->line
							,symbol_src->name
							,symbol_src->n_params
							,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN|REGISTER_SCOPE_NO_CHECK_REPEATED_SYMBOLS
					);

					symbol_dst->ref_ptr=symbol_src->ref_ptr;
					symbol_dst->overrided_symbol=symbol_src->overrided_symbol;
					symbol_dst->properties=symbol_src->properties;
				}

				// set idx starting member
				type->idx_starting_this_member_functions=type->scope->symbol_functions->length();

				// 1. extend all symbols from base type
				Vector<Symbol *> *symbol_variables=base_type->scope->symbol_variables;
				for(int i=0; i < symbol_variables->length(); i++){
					Symbol *symbol_src=(Symbol *)symbol_variables->get(i);
					Symbol *symbol_dst=scope_class->registerSymbolVariable(
							symbol_src->file
							,symbol_src->line
							,symbol_src->name
					);

					symbol_dst->ref_ptr=symbol_src->ref_ptr;
					symbol_dst->properties=symbol_src->properties;


					// attribs has to be copy MemberProperty...
					if(symbol_src->properties & SYMBOL_PROPERTY_MEMBER_PROPERTY){
						MemberProperty *mp_src=(MemberProperty *)symbol_src->ref_ptr;
						MemberProperty *mp_dst=new MemberProperty(type,mp_src->property_name);
						mp_dst->metamethod_members.getter=mp_src->metamethod_members.getter;
						mp_dst->metamethod_members.postinc=mp_src->metamethod_members.postinc;
						mp_dst->metamethod_members.postdec=mp_src->metamethod_members.postdec;
						mp_dst->metamethod_members.preinc=mp_src->metamethod_members.preinc;
						mp_dst->metamethod_members.predec=mp_src->metamethod_members.predec;
						mp_dst->metamethod_members.neg=mp_src->metamethod_members.neg;
						mp_dst->metamethod_members.bwc=mp_src->metamethod_members.bwc;

						const Metamethod *it_setters=MetamethodMembers::member_setter_list;

						while(*it_setters!=0){
							MetamethodMemberSetterInfo mp_info=mp_src->metamethod_members.getSetterInfo(*it_setters);
							for(int j=0; j < mp_info.setters->length();j++){
								mp_dst->metamethod_members.addSetter(*it_setters,(Symbol *)(((StackElement *)mp_info.setters->get(j))->value));

							}
							it_setters++;
						}

						type->allocated_member_properties->push(mp_dst);
						symbol_dst->ref_ptr=(zs_int)mp_dst;
					}
				}

				// set idx starting member
				type->idx_starting_this_member_variables=type->scope->symbol_variables->length();

				// 2. set idx base type...
				type->base_type_ids->push(base_type->id);
			}

			if(type->id != TYPE_ID_CLASS_MAIN){ // main type has no field initializers and reserve first function as main function
				String error="";
				Symbol *symbol_field_initializer=NULL;

				symbol_field_initializer=type->registerMemberFunction(
					StringUtils::format(
						"@__field_initializer_%s_@__"
						,type->name.toConstChar()
					)
				);

				type->sf_field_initializer=(Function *)symbol_field_initializer->ref_ptr;

				// create new scope
				type->sf_field_initializer->scope=(((zs)->getScopeFactory()))->newScope(
						type->sf_field_initializer->id
						,scope_class
						,ZS_SCOPE_PROPERTY_IS_SCOPE_FUNCTION
				);
			}

			return type;
		}else{
			ZS_THROW_RUNTIME_ERROR("Type '%s' already registered",_str_type.toConstChar());
		}
		return NULL;
	}

	Vector<Type *> * TypesFactory::getTypes(){
		return types;
	}

	Type 	* TypesFactory::getType(TypeId _type_id){
		if(_type_id == ZS_UNDEFINED_IDX){
			ZS_THROW_RUNTIME_ERRORF("Type node out of bound");
			return NULL;
		}
		return (Type *)types->get(_type_id);
	}

	Type *TypesFactory::getType(const String & _type_name){

		for(int i = 0; i < types->length(); i++){
			Type * type=(Type *)types->get(i);
			if(_type_name == type->name){//metadata_info.object_info.symbol_info.str_native_type){
				return type;
			}
		}
		return NULL;
	}

	Type *TypesFactory::getTypeFromTypeNamePtr(const String & _type_name_ptr){

		for(int i = 0; i < types->length(); i++){
			Type * type=(Type *)types->get(i);
			if(_type_name_ptr == type->native_name){//metadata_info.object_info.symbol_info.str_native_type){
				return type;
			}
		}
		return NULL;
	}


	TypeId TypesFactory::getTypeId(const String & _type_name){

		for(int i = 0; i < types->length(); i++){
			Type * type=(Type *)types->get(i);
			if(_type_name == type->name){
				return i;
			}
		}
		return TYPE_ID_INVALID;
	}

	TypeId TypesFactory::getTypeIdFromTypeNamePtr(const String & _type_name_ptr){
		// ok check str_native_type
		for(int i = 0; i < types->length(); i++){
			Type * type=(Type *)types->get(i);
			if(type->native_name == _type_name_ptr){
				return i;
			}
		}
		return TYPE_ID_INVALID;
	}

	bool TypesFactory::isTypeRegistered(const String & _type_name){
		return getTypeId(_type_name) != ZS_UNDEFINED_IDX;
	}

	Object *		TypesFactory::instanceObjectByTypeName(const String & _type_name){
		 // 0. Search type info ...
		 Type * type = getType(_type_name);

		 if(type != NULL){
			 return instanceObjectByTypeId(type->id);
		 }
		 return NULL;
	 }

	 Object 		 * TypesFactory::instanceObjectByTypeId(TypeId _type_id, void * value_object){

		 Object *so=NULL;

		 // 0. Search type info ...
		 Type *type = getType(_type_id);

		 if(type != NULL){
			 // Is a primitive ?
			switch(type->id){
			case TYPE_ID_OBJECT_STRING: // "String"
				so=StringObject::newStringObject(zs);
				break;
			case TYPE_ID_OBJECT_ARRAY: // Array []
				so=ArrayObject::newArrayObject(zs);
				break;
			// Object
			case TYPE_ID_DICTIONARY_OBJECT: //  Object {}
				so=DictionaryObject::newObjectObject(zs);
				break;

			default:

				if(type->id > TYPE_ID_OBJECT_CLASS){
					 // we create the object but not init as shared because it can hold a C pointer that is in charge of user deallocate or not
					 so = ClassObject::newClassObject(zs,type->id, value_object);
				}else{
					 ZS_THROW_RUNTIME_ERROR("Internal error: An idx type was expected but it was %i",type->id);
					 return NULL;
				 }
				break;
		 	 }
		 }
		 return so;
	 }


	const char * TypesFactory::getTypeName(TypeId _type_id){
		if(_type_id != ZS_UNDEFINED_IDX){
			Type *type=(Type *)types->get(_type_id);
			return type->name.toConstChar();
		}
		 return "type_unknow";
	}

	bool 	TypesFactory::scriptTypeInheritsFrom(TypeId _type_id, TypeId _type_id_base){

		if(_type_id == _type_id_base){
			return true;
		}

		Type *type=(Type *)types->get(_type_id);

		for(int i=0; i < type->base_type_ids->length(); i++){
			if(scriptTypeInheritsFrom(type->base_type_ids->get(i),_type_id_base)){
				return true;
			}
		}

		return false;
	}

	bool	TypesFactory::isTypeInstanceable(TypeId _type_id){

		if(
				_type_id == TYPE_ID_OBJECT_STRING
				|| _type_id == TYPE_ID_OBJECT_ARRAY
				|| _type_id == TYPE_ID_DICTIONARY_OBJECT
				|| _type_id >= TYPE_ID_MAX
		){
			return ((Type *)types->get(_type_id))->isNonInstantiable()==false;
		}

		return false;
	}


	TypesFactory::~TypesFactory(){
		// we have to destroy all allocated constructor/destructor ...
		for(int i = 0; i < types->length(); i++) {

			delete (Type *)types->get(i);
		}
		types->clear();

		delete types;
	}
}
