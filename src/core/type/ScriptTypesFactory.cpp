/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"


#define 				MAX_REGISTER_CLASSES 				100
#define 				MAIN_SCRIPT_CLASS_NAME 				"@MainClass"

#define SCF_REGISTER_STRUCT(_script_type_id, _class_type)\
	if(script_types->length()!=_script_type_id){\
		ZS_THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(_class_type));\
		return;\
	}\
	registerType<_class_type>(ZS_STR(_class_type));


#define SCF_REGISTER_CLASS(_script_type_id, _script_type_name, _class_type )\
	if(script_types->length()!=_script_type_id){\
		ZS_THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(_class_type));\
		return;\
	}\
	PRAGMA_PUSH\
	PRAGMA_DISABLE_WARNING(4127)\
	if(_script_type_id >= SCRIPT_TYPE_ID_MAX){\
		PRAGMA_POP\
		ZS_THROW_RUNTIME_ERROR("The type to bind '%s' should be a built in type",ZS_STR(_class_type));\
		return;\
	}\
	registerType<_class_type>(_script_type_name,_class_type##_New,_class_type##_Delete);

#define SCF_REGISTER_SINGLETON_CLASS(_script_type_id, _class_type )\
	if(script_types->length()!=_script_type_id){\
		ZS_THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(_class_type));\
		return;\
	}\
	registerType<_class_type>(ZS_STR(_class_type));

#define SCF_REGISTER_SINGLETON_NAME_CLASS(_script_type_id, _script_type_name, _native_type )\
	if(script_types->length()!=_script_type_id){\
		ZS_THROW_RUNTIME_ERROR("Error: built in type '%s' doesn't match its id",ZS_STR(_native_type));\
		return;\
	}\
	registerType<_native_type>(_script_type_name);
/*
#define SCF_REGISTER_NATIVE_TYPE(_script_type_id, _native_type)\
	if(script_types->length()!=_script_type_id){\
		ZS_THROW_RUNTIME_ERROR("Error initializing C built in type: '%s'",ZS_STR(_native_type));\
		return;\
	}else{\
		ScriptType *script_type=registerType(ZS_STR(_native_type),"",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);\
		script_type->scope->properties|=SCOPE_PROPERTY_IS_C_OBJECT_REF;\
		script_type->properties=SCRIPT_TYPE_PROPERTY_NATIVE_OBJECT_REF;\
		script_type->native_name=(typeid(_native_type).name());\
	}
*/

#define SCF_REGISTER_NATIVE_TYPE(_script_type_id, _script_type_name, _native_type)\
	if(script_types->length()!=_script_type_id){\
		ZS_THROW_RUNTIME_ERROR("Error initializing C built in type: '%s'",ZS_STR(_native_type));\
		return;\
	}else{\
		ScriptType *script_type=registerType(_script_type_name);\
		script_type->scope->properties|=SCOPE_PROPERTY_IS_C_OBJECT_REF;\
		script_type->properties=SCRIPT_TYPE_PROPERTY_NATIVE_OBJECT_REF;\
		script_type->native_name=(typeid(_native_type).name());\
	}

#define SCF_REGISTER_TYPE(_script_type_id,_script_type_name)\
	if(script_types->length()!=_script_type_id){\
		ZS_THROW_RUNTIME_ERROR("Error initializing built in type: '%s'",_script_type_name);\
		return;\
	}else{\
		registerType(_script_type_name);\
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
	const char * k_str_zettype_ptr=typeid(ScriptEngine *).name();

	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ObjectScriptObject)
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(StringScriptObject)
	ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(ArrayScriptObject)

	ScriptTypesFactory::ScriptTypesFactory(ScriptEngine *_script_engine){
		script_engine = _script_engine;
		vm = _script_engine->getVirtualMachine();
		scope_factory = this->script_engine->getScriptScopesFactory();
		script_function_factory= this->script_engine->getScriptFunctionsFactory();
		main_function=NULL;
		main_object=NULL;
		script_types=new Vector<ScriptType *>;
		main_object=NULL;
		main_function=NULL;
		idx_clear_checkpoint=0;
	}

	void ScriptTypesFactory::init(){
		String error;

		// ScriptFunctionsFactory has to be created
		main_object=registerType(MAIN_SCRIPT_CLASS_NAME); // 0
		ZS_MAIN_SCOPE(this)->owner_type=main_object;

		Symbol *symbol_main_function=main_object->registerMemberFunction(
				ZS_MAIN_FUNCTION_NAME
		);

		main_function=(ScriptFunction *)symbol_main_function->ref_ptr;
		main_function->scope=ZS_MAIN_SCOPE(this);

		idx_clear_checkpoint=1; // by default restore till main type.
	}

	zetscript::ScriptEngine *ptrToScriptEnginePtr(ScriptEngine *_script_engine, zs_int ptr){
		ZS_UNUSUED_PARAM(_script_engine);
		return (ScriptEngine *)ptr;
	}

	zs_int intMaxValue(ScriptEngine *_script_engine){
		ZS_UNUSUED_PARAM(_script_engine);
		return INTPTR_MAX;
	}

	zs_float floatMaxValue(ScriptEngine *_script_engine){
		ZS_UNUSUED_PARAM(_script_engine);
		return FLT_MAX;
	}

	zs_float parseFloat(ScriptEngine *_script_engine,zs_int number){
		ZS_UNUSUED_PARAM(_script_engine);
		return (zs_float)(number);
	}

	zs_float parseFloat(ScriptEngine *_script_engine,zs_float *number){
		ZS_UNUSUED_PARAM(_script_engine);
		return *number;
	}

	zs_float parseFloat(ScriptEngine *_script_engine,String  *number_str){
		ZS_UNUSUED_PARAM(_script_engine);
		zs_float result=0;
		zs_float *result_ptr=Float::parse(*number_str);

		if(result_ptr != NULL){
			result=*result_ptr;
			delete result_ptr;
		}

		return result;
	}

	zs_int parseInt(ScriptEngine *_script_engine,zs_int number){
		ZS_UNUSUED_PARAM(_script_engine);
		return number;
	}

	zs_int parseInt(ScriptEngine *_script_engine,zs_float *number){
		ZS_UNUSUED_PARAM(_script_engine);
		return (zs_int)(*number);
	}

	zs_int parseInt(ScriptEngine *_script_engine,String  *number_str){
		ZS_UNUSUED_PARAM(_script_engine);
		zs_int result=0;
		zs_int *result_ptr=Integer::parse(*number_str);
		if(result_ptr!=NULL){
			result=*result_ptr;
			delete result_ptr;
		}
		return result;
	}

	bool isNumber(ScriptEngine *_script_engine,StackElement *_stk){
		ZS_UNUSUED_PARAM(_script_engine);
		return (_stk->properties & (STACK_ELEMENT_PROPERTY_INT | STACK_ELEMENT_PROPERTY_FLOAT)) != 0;
	}

	void ScriptTypesFactory::setup(){

		// !!!
		// !!! START REGISTER BUILT IN CLASSES AND TYPES
		// !!! WARNING IT SHOULD BE IN THE SAME ORDER AS IS DEFINED IN COMMON.H
		// !!!

		// primitives
		SCF_REGISTER_TYPE(SCRIPT_TYPE_ID_UNDEFINED				,ZS_SCRIPT_TYPE_NAME_UNDEFINED);
		SCF_REGISTER_TYPE(SCRIPT_TYPE_ID_NULL					,ZS_SCRIPT_TYPE_NAME_NULL);
		SCF_REGISTER_NATIVE_TYPE(SCRIPT_TYPE_ID_VOID			,ZS_SCRIPT_TYPE_NAME_VOID,void);
		SCF_REGISTER_NATIVE_TYPE(SCRIPT_TYPE_ID_INT				,ZS_SCRIPT_TYPE_NAME_INT,zs_int);
		SCF_REGISTER_NATIVE_TYPE(SCRIPT_TYPE_ID_INT_PTR			,ZS_SCRIPT_TYPE_NAME_INT_PTR,zs_int *);
		SCF_REGISTER_NATIVE_TYPE(SCRIPT_TYPE_ID_CHAR_PTR		,ZS_SCRIPT_TYPE_NAME_CHAR_PTR,char *);
		SCF_REGISTER_NATIVE_TYPE(SCRIPT_TYPE_ID_CONST_CHAR_PTR	,ZS_SCRIPT_TYPE_NAME_CONST_CHAR_PTR,const char *);
		SCF_REGISTER_NATIVE_TYPE(SCRIPT_TYPE_ID_STRING			,ZS_SCRIPT_TYPE_NAME_STRING,String);
		SCF_REGISTER_NATIVE_TYPE(SCRIPT_TYPE_ID_STRING_PTR		,ZS_SCRIPT_TYPE_NAME_STRING_PTR,String *);
		SCF_REGISTER_NATIVE_TYPE(SCRIPT_TYPE_ID_BOOL			,ZS_SCRIPT_TYPE_NAME_BOOL,bool);
		SCF_REGISTER_NATIVE_TYPE(SCRIPT_TYPE_ID_BOOL_PTR		,ZS_SCRIPT_TYPE_NAME_BOOL_PTR,bool *);
		SCF_REGISTER_NATIVE_TYPE(SCRIPT_TYPE_ID_FLOAT			,ZS_SCRIPT_TYPE_NAME_FLOAT,zs_float);
		SCF_REGISTER_NATIVE_TYPE(SCRIPT_TYPE_ID_FLOAT_PTR		,ZS_SCRIPT_TYPE_NAME_FLOAT_PTR,zs_float *);
		SCF_REGISTER_NATIVE_TYPE(SCRIPT_TYPE_ID_CONST_FLOAT_PTR	,ZS_SCRIPT_TYPE_NAME_CONST_FLOAT_PTR,const zs_float *);

		// estructures
		SCF_REGISTER_STRUCT(SCRIPT_TYPE_ID_STACK_ELEMENT,StackElement);

		//------------------------
		// BUILT-IN SCRIPT OBJECTS
		// It self Script object
		SCF_REGISTER_SINGLETON_NAME_CLASS(SCRIPT_TYPE_ID_SCRIPT_FUNCTION,ZS_SCRIPT_TYPE_NAME_SCRIPT_FUNCTION,ScriptFunction);
		SCF_REGISTER_SINGLETON_NAME_CLASS(SCRIPT_TYPE_ID_VAR_REF_SCRIPT_OBJECT,ZS_SCRIPT_TYPE_NAME_VAR_REF_SCRIPT_OBJECT,VarRefObject);
		SCF_REGISTER_SINGLETON_NAME_CLASS(SCRIPT_TYPE_ID_MEMBER_FUNCTION_SCRIPT_OBJECT,ZS_SCRIPT_TYPE_NAME_MEMBER_FUNCTION_SCRIPT_OBJECT,MemberFunctionScriptObject);
		SCF_REGISTER_SINGLETON_CLASS(SCRIPT_TYPE_ID_ZETSCRIPT_SCRIPT_OBJECT,ScriptEngine);

		SCF_REGISTER_CLASS(SCRIPT_TYPE_ID_STRING_SCRIPT_OBJECT,ZS_SCRIPT_TYPE_NAME_STRING_SCRIPT_OBJECT,StringScriptObject);

		// Script object iterators
		SCF_REGISTER_SINGLETON_NAME_CLASS(SCRIPT_TYPE_ID_STRING_ITERATOR_SCRIPT_OBJECT,ZS_SCRIPT_TYPE_NAME_STRING_ITERATOR_SCRIPT_OBJECT,StringIteratorScriptObject);
		SCF_REGISTER_SINGLETON_NAME_CLASS(SCRIPT_TYPE_ID_ARRAY_ITERATOR_SCRIPT_OBJECT,ZS_SCRIPT_TYPE_NAME_ARRAY_ITERATOR_SCRIPT_OBJECT,ArrayIteratorScriptObject);
		SCF_REGISTER_SINGLETON_NAME_CLASS(SCRIPT_TYPE_ID_OBJECT_ITERATOR_SCRIPT_OBJECT,ZS_SCRIPT_TYPE_NAME_OBJECT_ITERATOR_SCRIPT_OBJECT,ObjectIteratorScriptObject);


		// BUILT-IN SCRIPT OBJECTS
		//------------------------
		// BUILT-IN SCRIPT OBJECTS CLASSES
		SCF_REGISTER_CLASS(SCRIPT_TYPE_ID_ARRAY_SCRIPT_OBJECT,ZS_SCRIPT_TYPE_NAME_ARRAY_SCRIPT_OBJECT,ArrayScriptObject);
		SCF_REGISTER_CLASS(SCRIPT_TYPE_ID_OBJECT_SCRIPT_OBJECT,ZS_SCRIPT_TYPE_NAME_OBJECT_SCRIPT_OBJECT,ObjectScriptObject);
		SCF_REGISTER_SINGLETON_NAME_CLASS(SCRIPT_TYPE_ID_CLASS_SCRIPT_OBJECT,ZS_SCRIPT_TYPE_NAME_CLASS_SCRIPT_OBJECT,ClassScriptObject);
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

		script_engine->registerFunction("ptrToScriptEnginePtr",ptrToScriptEnginePtr);

		ScriptType *integer_type=this->getScriptType(ZS_SCRIPT_TYPE_NAME_INT);
		integer_type->registerConstMemberProperty("MAX_VALUE",intMaxValue);
		integer_type->registerStaticMemberFunction("parse",static_cast<zs_int (*)(ScriptEngine *,zs_float *)>(parseInt));
		integer_type->registerStaticMemberFunction("parse",static_cast<zs_int (*)(ScriptEngine *,zs_int )>(parseInt));
		integer_type->registerStaticMemberFunction("parse",static_cast<zs_int (*)(ScriptEngine *,String *)>(parseInt));

		ScriptType *float_type=this->getScriptType(ZS_SCRIPT_TYPE_NAME_FLOAT);
		float_type->registerConstMemberProperty("MAX_VALUE",floatMaxValue);
		float_type->registerStaticMemberFunction("parse",static_cast<zs_float (*)(ScriptEngine *,zs_int )>(parseFloat));
		float_type->registerStaticMemberFunction("parse",static_cast<zs_float (*)(ScriptEngine *,zs_float *)>(parseFloat));
		float_type->registerStaticMemberFunction("parse",static_cast<zs_float (*)(ScriptEngine *,String *)>(parseFloat));


		// Global symbols
		script_engine->registerFunction("isNumber",isNumber);

		//-------------------------
		// Wrap functions

		//---------------------------------------------
		// StringScriptObject (aka String in script)

		// Properties StringScriptObject
		registerMemberPropertyMetamethod<StringScriptObject>("length","_get",StringScriptObjectZs_length);

		// Members StringScriptObject
		registerStaticMemberFunction<StringScriptObject>("formatNative",StringScriptObject::format);
		registerMemberFunction<StringScriptObject>("eraseAt",StringScriptObjectZs_eraseAt);
		registerMemberFunction<StringScriptObject>("insertAt",static_cast<void (*)(ScriptEngine *_script_engine,StringScriptObject *so, zs_int, zs_int )>(StringScriptObjectZs_insertAt));
		registerMemberFunction<StringScriptObject>("insertAt",static_cast<void (*)(ScriptEngine *_script_engine,StringScriptObject *so, zs_int, String *)>(StringScriptObjectZs_insertAt));
		registerMemberFunction<StringScriptObject>("clear",StringScriptObjectZs_clear);
		registerMemberFunction<StringScriptObject>("replace",StringScriptObjectZs_replace);
		registerMemberFunction<StringScriptObject>("split",static_cast<ArrayScriptObject * (*)(ScriptEngine *_script_engine,StringScriptObject *so, zs_int )>(StringScriptObjectZs_split));
		registerMemberFunction<StringScriptObject>("split",static_cast<ArrayScriptObject * (*)(ScriptEngine *_script_engine,StringScriptObject *so, String *)>(StringScriptObjectZs_split));
		registerMemberFunction<StringScriptObject>("contains",static_cast<bool (*)(ScriptEngine *_script_engine,StringScriptObject *so, String *)>(&StringScriptObjectZs_contains));
		registerMemberFunction<StringScriptObject>("contains",static_cast<bool (*)(ScriptEngine *_script_engine,StringScriptObject *so, zs_int )>(&StringScriptObjectZs_contains));

		registerMemberFunction<StringScriptObject>("indexOf",static_cast<zs_int (*)(ScriptEngine *_script_engine,StringScriptObject *so, String *)>(&StringScriptObjectZs_indexOf));
		registerMemberFunction<StringScriptObject>("indexOf",static_cast<zs_int (*)(ScriptEngine *_script_engine,StringScriptObject *so, zs_int )>(&StringScriptObjectZs_indexOf));

		registerMemberFunction<StringScriptObject>("startsWith",StringScriptObjectZs_startsWith);
		registerMemberFunction<StringScriptObject>("endsWith",StringScriptObjectZs_endsWith);
		registerMemberFunction<StringScriptObject>("substring",static_cast<StringScriptObject * (*)(ScriptEngine *_script_engine,StringScriptObject *so, zs_int,zs_int )>(&StringScriptObjectZs_substring));
		registerMemberFunction<StringScriptObject>("substring",static_cast<StringScriptObject * (*)(ScriptEngine *_script_engine,StringScriptObject *so, zs_int )>(&StringScriptObjectZs_substring));
		registerMemberFunction<StringScriptObject>("append",static_cast<void (*)(ScriptEngine *,StringScriptObject *, StringScriptObject *)>(&StringScriptObjectZs_append));
		registerMemberFunction<StringScriptObject>("append",static_cast<void (*)(ScriptEngine *,StringScriptObject *, zs_int )>(&StringScriptObjectZs_append));
		registerMemberFunction<StringScriptObject>("toLowerCase",StringScriptObjectZs_toLowerCase);
		registerMemberFunction<StringScriptObject>("toUpperCase",StringScriptObjectZs_toUpperCase);

		registerMemberFunction<StringScriptObject>("_iter",StringScriptObjectZs_iter);

		// Iterator StringScriptObject
		registerMemberFunction<StringIteratorScriptObject>("_next",StringIteratorScriptObjectZs_next);
		registerMemberFunction<StringIteratorScriptObject>("_end",StringIteratorScriptObjectZs_end);
		registerMemberFunction<StringIteratorScriptObject>("_get",StringIteratorScriptObjectZs_get);

		//---------------------------------------------
		// ArrayScriptObject (aka Array in script)

		// Properties ArrayScriptObject
		registerMemberPropertyMetamethod<ArrayScriptObject>("length","_get",&ArrayScriptObjectZs_length);

		// Members ArrayScriptObject
		registerMemberFunction<ArrayScriptObject>("push",&ArrayScriptObjectZs_push);
		registerMemberFunction<ArrayScriptObject>("pop",&ArrayScriptObjectZs_pop);
		registerMemberFunction<ArrayScriptObject>("insertAt",&ArrayScriptObjectZs_insertAt);
		registerMemberFunction<ArrayScriptObject>("eraseAt",&ArrayScriptObjectZs_eraseAt);
		registerMemberFunction<ArrayScriptObject>("clear",&ArrayScriptObjectZs_clear);
		registerMemberFunction<ArrayScriptObject>("join",&ArrayScriptObjectZs_join);
		registerMemberFunction<ArrayScriptObject>("extend",&ArrayScriptObjectZs_extend);
		registerStaticMemberFunction<ArrayScriptObject>("concat",&ArrayScriptObjectZs_concat);
		registerMemberFunction<ArrayScriptObject>("contains",&ArrayScriptObjectZs_contains);

		registerMemberFunction<ArrayScriptObject>("_iter",&ArrayScriptObjectZs_iter);
		registerStaticMemberFunction<ArrayScriptObject>("_equ",&ArrayScriptObjectZs_equal);
		//registerStaticMemberFunction<ArrayScriptObject>("_add",&ArrayScriptObjectZs_equal_add);

		// Members ArrayIteratorScriptObject
		registerMemberFunction<ArrayIteratorScriptObject>("_next",ArrayIteratorObjectZs_next);
		registerMemberFunction<ArrayIteratorScriptObject>("_end",ArrayIteratorObjectZs_end);
		registerMemberFunction<ArrayIteratorScriptObject>("_get",ArrayIteratorObjectZs_get);


		//---------------------------------------------
		// ObjectScriptObject (aka Object in script)

		registerStaticMemberFunction<ObjectScriptObject>("clear",&ObjectScriptObjectZs_clear);
		registerStaticMemberFunction<ObjectScriptObject>("erase",&ObjectScriptObjectZs_erase);
		registerStaticMemberFunction<ObjectScriptObject>("contains",&ObjectScriptObjectZs_contains);
		registerStaticMemberFunction<ObjectScriptObject>("extend",&ObjectScriptObjectZs_extend);
		registerStaticMemberFunction<ObjectScriptObject>("concat",ObjectScriptObject::concat);
		registerStaticMemberFunction<ObjectScriptObject>("keys",ObjectScriptObjectZs_keys);
		registerStaticMemberFunction<ObjectScriptObject>("_iter",ObjectScriptObjectZs_iter);

		// ObjectIteratorScriptObject
		registerMemberFunction<ObjectIteratorScriptObject>("_next",ObjectIteratorScriptObjectZs_next);
		registerMemberFunction<ObjectIteratorScriptObject>("_end",ObjectIteratorScriptObjectZs_end);
		registerMemberFunction<ObjectIteratorScriptObject>("_get",ObjectIteratorScriptObjectZs_get);

		script_engine->saveState();
	}

	void ScriptTypesFactory::clear(short _idx_start){
		short idx_start = _idx_start == ZS_UNDEFINED_IDX ?  idx_clear_checkpoint:_idx_start;
		for(
			int v=script_types->length()-1;
			v > idx_start; // avoid main type
			v--
		){
			ScriptType * script_type = (ScriptType *)script_types->get(v);
			delete script_type;
			script_types->pop();
		}
	}

	void ScriptTypesFactory::saveState(){
		idx_clear_checkpoint = script_types->length()-1;
	}

	void ScriptTypesFactory::checkTypeName(const String & _script_type_name){

		if(script_types->length()>=MAX_REGISTER_CLASSES){
			ZS_THROW_RUNTIME_ERROR("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES);
		}

		if(_script_type_name.isEmpty()){
			ZS_THROW_RUNTIME_ERRORF("Class name empty");
		}

		if(script_engine->getScriptFunctionsFactory()->getFunctions()->length() > 0){
			Symbol *main_function_symbol=NULL;
			if((main_function_symbol=scope_factory->getMainScope()->getSymbol(
					_script_type_name
					,ZS_NO_PARAMS_SYMBOL_ONLY,SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_DOWN
			))!=NULL){
				ZS_THROW_RUNTIME_ERROR(
					"ScriptType name '%s' is already defined %s"
					,_script_type_name.toConstChar()
					, (main_function_symbol->file!=NULL && strcmp(main_function_symbol->file,"")!=0) && (main_function_symbol->line!=-1)
						?
							String::format(
								"at " ZS_FORMAT_FILE_LINE
								,Path::getFilename(main_function_symbol->file).toConstChar()
								,main_function_symbol->line
							).toConstChar()
						:""
				);
			}
		}
	}

	ScriptType * ScriptTypesFactory::registerType(
			const String & _script_type_name
			 ,const String & _base_script_type_name
			 ,uint16_t _properties
			 ,const char * _file
			 , short _line
	){
		ScriptTypeId  new_script_type_id;
		ScriptType *script_type=NULL;

		checkTypeName(_script_type_name);

		if((new_script_type_id = getScriptTypeId(_script_type_name))==ZS_UNDEFINED_IDX){ // check whether is local var registered scope ...
			uint16_t properties_register_scope=SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN;
			new_script_type_id=script_types->length();

			// To avoid built-int conflict bool type
			if(
					new_script_type_id == SCRIPT_TYPE_ID_BOOL
					|| new_script_type_id == SCRIPT_TYPE_ID_UNDEFINED
					|| new_script_type_id == SCRIPT_TYPE_ID_NULL
					|| new_script_type_id == SCRIPT_TYPE_ID_INT
					|| new_script_type_id == SCRIPT_TYPE_ID_FLOAT
			){
				properties_register_scope|=SCRIPT_SCOPE_REGISTER_PROPERTY_NO_CHECK_CLASS_SYMBOLS;
			}
			// BYTE_CODE_NEW SCOPE C and register ...
			ScriptScope * scope_class = ZS_NEW_SCOPE(this,ZS_UNDEFINED_IDX,NULL, SCOPE_PROPERTY_IS_SCOPE_CLASS);

			// register symbol on main scope...

			Symbol *symbol=ZS_MAIN_SCOPE(this)->registerSymbolType(_file,_line,_script_type_name,properties_register_scope);

			script_type = new ScriptType(
				this->script_engine
				,new_script_type_id
				, _script_type_name
				, scope_class
				, ZS_SCRIPT_TYPE_NAME_UNKNOWN
				,_properties
			);

			scope_class->setTypeOwner(script_type);
			symbol->ref_ptr=(zs_int)script_type;

			script_types->push(script_type);

			// if _base_script_type_name != "", the class extends from other
			if(_base_script_type_name != ""){

				ScriptType *base_script_type=NULL;

				if(script_type->base_script_type_ids->length() > 0){
					ScriptType *match_class=getScriptType(script_type->base_script_type_ids->get(0));
					ZS_THROW_RUNTIME_ERROR("ScriptType '%s' already is inherited from '%s'"
							,_script_type_name.toConstChar()
							,match_class->name.toConstChar());
				}

				if((base_script_type = getScriptType(_base_script_type_name)) == NULL){
					ZS_THROW_RUNTIME_ERROR("ScriptType '%s' not registered",_base_script_type_name.toConstChar());
				}

				// 1. extend all symbols from base type
				Vector<Symbol *> *symbol_functions=base_script_type->scope->symbol_functions;
				for(int i=0; i < symbol_functions->length(); i++){
					Symbol *symbol_src=(Symbol *)symbol_functions->get(i);
					Symbol *symbol_dst=scope_class->registerSymbolFunction(
							symbol_src->file
							,symbol_src->line
							,symbol_src->name
							,symbol_src->n_params
							,SCRIPT_SCOPE_REGISTER_PROPERTY_CHECK_REPEATED_SYMBOLS_UP_AND_DOWN|SCRIPT_SCOPE_REGISTER_PROPERTY_NO_CHECK_REPEATED_SYMBOLS
					);

					symbol_dst->ref_ptr=symbol_src->ref_ptr;
					symbol_dst->overrided_symbol=symbol_src->overrided_symbol;
					symbol_dst->properties=symbol_src->properties;
				}

				// set idx starting member
				script_type->idx_starting_this_member_functions=script_type->scope->symbol_functions->length();

				// 1. extend all symbols from base type
				Vector<Symbol *> *symbol_variables=base_script_type->scope->symbol_variables;
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
						MemberProperty *mp_dst=new MemberProperty(script_type,mp_src->property_name);
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

						script_type->allocated_member_properties->push(mp_dst);
						symbol_dst->ref_ptr=(zs_int)mp_dst;
					}
				}

				// set idx starting member
				script_type->idx_starting_this_member_variables=script_type->scope->symbol_variables->length();

				// 2. set idx base type...
				script_type->base_script_type_ids->push(base_script_type->id);
			}

			if(script_type->id != SCRIPT_TYPE_ID_CLASS_MAIN){ // main type has no field initializers and reserve first function as main function
				String error="";
				Symbol *symbol_field_initializer=NULL;

				symbol_field_initializer=script_type->registerMemberFunction(
					String::format(
						"@__field_initializer_%s_@__"
						,script_type->name.toConstChar()
					)
				);

				script_type->sf_field_initializer=(ScriptFunction *)symbol_field_initializer->ref_ptr;

				// create new scope
				script_type->sf_field_initializer->scope=(((script_engine)->getScriptScopesFactory()))->newScope(
						script_type->sf_field_initializer->id
						,scope_class
						,SCOPE_PROPERTY_IS_SCOPE_FUNCTION
				);
			}

			return script_type;
		}else{
			ZS_THROW_RUNTIME_ERROR("ScriptType '%s' already registered",_script_type_name.toConstChar());
		}
		return NULL;
	}

	Vector<ScriptType *> * ScriptTypesFactory::getScriptTypes(){
		return script_types;
	}

	ScriptType 	* ScriptTypesFactory::getScriptType(ScriptTypeId _script_type_id){
		if(_script_type_id == ZS_UNDEFINED_IDX){
			ZS_THROW_RUNTIME_ERRORF("ScriptType node out of bound");
			return NULL;
		}
		return (ScriptType *)script_types->get(_script_type_id);
	}

	ScriptType *ScriptTypesFactory::getScriptType(const String & _script_type_name){

		for(int i = 0; i < script_types->length(); i++){
			ScriptType * script_type=(ScriptType *)script_types->get(i);
			if(_script_type_name == script_type->name){//metadata_info.object_info.symbol_info.str_native_type){
				return script_type;
			}
		}
		return NULL;
	}

	ScriptType *ScriptTypesFactory::getScriptTypeFromNamePtr(const String & _script_type_name_ptr){

		for(int i = 0; i < script_types->length(); i++){
			ScriptType * script_type=(ScriptType *)script_types->get(i);
			if(_script_type_name_ptr == script_type->native_name){//metadata_info.object_info.symbol_info.str_native_type){
				return script_type;
			}
		}
		return NULL;
	}

	ScriptTypeId ScriptTypesFactory::getScriptTypeId(const String & _script_type_name){

		for(int i = 0; i < script_types->length(); i++){
			ScriptType * script_type=(ScriptType *)script_types->get(i);
			if(_script_type_name == script_type->name){
				return i;
			}
		}
		return SCRIPT_TYPE_ID_INVALID;
	}

	ScriptTypeId ScriptTypesFactory::getScriptTypeIdFromNamePtr(const String & _script_type_name_ptr){
		// ok check str_native_type
		for(int i = 0; i < script_types->length(); i++){
			ScriptType * script_type=(ScriptType *)script_types->get(i);
			if(script_type->native_name == _script_type_name_ptr){
				return i;
			}
		}
		return SCRIPT_TYPE_ID_INVALID;
	}

	bool ScriptTypesFactory::isScriptTypeRegistered(const String & _script_type_name){
		return getScriptTypeId(_script_type_name) != ZS_UNDEFINED_IDX;
	}

	ScriptObject *		ScriptTypesFactory::instanceObjectByScriptTypeName(const String & _script_type_name){
		 // 0. Search type info ...
		 ScriptType * script_type = getScriptType(_script_type_name);

		 if(script_type != NULL){
			 return instanceObjectByScriptTypeId(script_type->id);
		 }
		 return NULL;
	 }

	 ScriptObject 		 * ScriptTypesFactory::instanceObjectByScriptTypeId(ScriptTypeId _script_type_id, void * value_object){

		 ScriptObject *so=NULL;

		 // 0. Search type info ...
		 ScriptType *script_type = getScriptType(_script_type_id);

		 if(script_type != NULL){
			 // Is a primitive ?
			switch(script_type->id){
			case SCRIPT_TYPE_ID_STRING_SCRIPT_OBJECT: // "String"
				so=StringScriptObject::newStringScriptObject(script_engine);
				break;
			case SCRIPT_TYPE_ID_ARRAY_SCRIPT_OBJECT: // Array []
				so=ArrayScriptObject::newArrayScriptObject(script_engine);
				break;
			// ScriptObject
			case SCRIPT_TYPE_ID_OBJECT_SCRIPT_OBJECT: //  ScriptObject {}
				so=ObjectScriptObject::newObjectScriptObject(script_engine);
				break;

			default:

				if(script_type->id > SCRIPT_TYPE_ID_CLASS_SCRIPT_OBJECT){
					 // we create the object but not init as shared because it can hold a C pointer that is in charge of user deallocate or not
					 so = ClassScriptObject::newClassScriptObject(script_engine,script_type->id, value_object);
				}else{
					 ZS_THROW_RUNTIME_ERROR("Internal error: An idx type was expected but it was %i",script_type->id);
					 return NULL;
				 }
				break;
		 	 }
		 }
		 return so;
	 }

	 const char * ScriptTypesFactory::getScriptTypeName(ScriptTypeId _script_type_id){
		 if(_script_type_id != ZS_UNDEFINED_IDX){
			ScriptType *script_type=(ScriptType *)script_types->get(_script_type_id);
			return script_type->name.toConstChar();
		 }
		 return "type_unknow";
	 }

	bool 	ScriptTypesFactory::scriptTypeInheritsFrom(ScriptTypeId _script_type_id, ScriptTypeId _script_type_id_base){

		if(_script_type_id == _script_type_id_base){
			return true;
		}

		ScriptType *script_type=(ScriptType *)script_types->get(_script_type_id);

		for(int i=0; i < script_type->base_script_type_ids->length(); i++){
			if(scriptTypeInheritsFrom(script_type->base_script_type_ids->get(i),_script_type_id_base)){
				return true;
			}
		}

		return false;
	}

	bool	ScriptTypesFactory::isTypeInstanceable(ScriptTypeId _script_type_id){

		if(
				_script_type_id == SCRIPT_TYPE_ID_STRING_SCRIPT_OBJECT
				|| _script_type_id == SCRIPT_TYPE_ID_ARRAY_SCRIPT_OBJECT
				|| _script_type_id == SCRIPT_TYPE_ID_OBJECT_SCRIPT_OBJECT
				|| _script_type_id >= SCRIPT_TYPE_ID_MAX
		){
			return ((ScriptType *)script_types->get(_script_type_id))->isNonInstantiable()==false;
		}

		return false;
	}


	ScriptTypesFactory::~ScriptTypesFactory(){
		// we have to destroy all allocated constructor/destructor ...
		for(int i = 0; i < script_types->length(); i++) {

			delete (ScriptType *)script_types->get(i);
		}
		script_types->clear();

		delete script_types;
	}
}
