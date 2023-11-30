/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ZetScript::ZetScript(){
		eval_int=0;
		eval_float=0;
		eval_string="";
		eval_bool = false;
		idx_current_global_variable_checkpoint=0;
		idx_current_global_function_checkpoint=0;
		idx_current_types_checkpoint=0;

		script_filenames_by_ref=NULL;

		eval_init();
		scope_factory = new ScriptScopesFactory(this);
		scope_factory->init();
		script_function_factory= new ScriptFunctionsFactory(this);

		virtual_machine = vm_new(this);
		script_types_factory = new ScriptTypesFactory(this);
		script_types_factory->init();
		vm_init(virtual_machine,this);

		script_types_factory->setup();

		script_filenames_by_ref=new MapString();
		stk_constants=new MapString();
		compiled_symbol_name = new MapString();
		//-------------------------
		// Register built in modules

		// Math mod
		ScriptType *cl=script_types_factory->registerScriptType("Math","",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);

		cl->registerConstMemberProperty("PI",MathModule_PI);
		cl->registerStaticMemberFunction("sin",MathModule_sin);
		cl->registerStaticMemberFunction("cos",MathModule_cos);
		cl->registerStaticMemberFunction("abs",MathModule_abs);
		cl->registerStaticMemberFunction("pow",MathModule_pow);
		cl->registerStaticMemberFunction("degToRad",MathModule_degToRad);
		cl->registerStaticMemberFunction("random",MathModule_random);
		cl->registerStaticMemberFunction("max",MathModule_max);
		cl->registerStaticMemberFunction("min",MathModule_min);
		cl->registerStaticMemberFunction("sqrt",MathModule_sqrt);
		cl->registerStaticMemberFunction("floor",MathModule_floor);
		cl->registerStaticMemberFunction("ceil",MathModule_ceil);
		cl->registerStaticMemberFunction("round",MathModule_round);

		// Console mod
		cl=script_types_factory->registerScriptType("Console","",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);
		cl->registerStaticMemberFunction("readChar",ConsoleModule_readChar);
		cl->registerStaticMemberFunction("outNative",ConsoleModule_out);
		cl->registerStaticMemberFunction("outlnNative",ConsoleModule_outln);
		cl->registerStaticMemberFunction("errorNative",ConsoleModule_error);
		cl->registerStaticMemberFunction("errorlnNative",ConsoleModule_errorln);

		// System mod
		cl=script_types_factory->registerScriptType("System","",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);
		cl->registerStaticMemberFunction("clock",SystemModule_clock);
		cl->registerStaticMemberFunction("eval",static_cast<void(*)(ZetScript *, StringScriptObject *)>(SystemModule_eval));
		cl->registerStaticMemberFunction("eval",static_cast<void(*)(ZetScript *, StringScriptObject *,DictionaryScriptObject *)>(SystemModule_eval));
		//cl->registerStaticMemberFunction("assertNative",SystemModule_assert);
		cl->registerStaticMemberFunction("errorNative",SystemModule_error);

		// Json mod
		cl=script_types_factory->registerScriptType("Json","",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);
		cl->registerStaticMemberFunction("serializeNative",static_cast<StringScriptObject * (*)(ZetScript *zs,StackElement *)>(JsonModule_serialize));
		cl->registerStaticMemberFunction("serializeNative",static_cast<StringScriptObject * (*)(ZetScript *zs,StackElement *, bool *)>(JsonModule_serialize));
		cl->registerStaticMemberFunction("deserialize",JsonModule_deserialize);
		//---------------------------------------------
		// TimeSpan
		cl=registerScriptType<TimeSpan>("TimeSpan",TimeSpanModule_new,TimeSpanModule_delete);
		registerMemberPropertyMetamethod<TimeSpan>("days","_get",TimeSpanModule_get_days);
		registerMemberPropertyMetamethod<TimeSpan>("hour","_get",TimeSpanModule_get_hours);
		registerMemberPropertyMetamethod<TimeSpan>("minute","_get",TimeSpanModule_get_minutes);
		registerMemberPropertyMetamethod<TimeSpan>("second","_get",TimeSpanModule_get_seconds);

		//---------------------------------------------
		// DateTime
		cl=registerScriptType<DateTime>("DateTime",DateTimeModule_new,DateTimeModule_delete);
		registerConstructor<DateTime>(DateTimeModule_constructor);
		/*registerStaticMemberFunction<DateTime>("_add",DateTimeModule_add);*/
		registerStaticMemberFunction<DateTime>("_sub",DateTimeModule_sub);

		registerMemberFunction<DateTime>("setUtc",DateTimeModule_setUtc);

		registerMemberFunction<DateTime>("addSeconds",DateTimeModule_addSeconds);
		registerMemberFunction<DateTime>("addMinutes",DateTimeModule_addMinutes);
		registerMemberFunction<DateTime>("addHours",DateTimeModule_addHours);
		registerMemberFunction<DateTime>("addDays",DateTimeModule_addDays);
		registerMemberFunction<DateTime>("addMonths",DateTimeModule_addMonths);
		registerMemberFunction<DateTime>("addYears",DateTimeModule_addYears);

		// metamethods...
		registerMemberFunction<DateTime>("_tostring",DateTimeModule_tostring);

		registerMemberPropertyMetamethod<DateTime>("week_day","_get",DateTimeModule_get_week_day);
		registerMemberPropertyMetamethod<DateTime>("month_day","_get",DateTimeModule_get_month_day);
		registerMemberPropertyMetamethod<DateTime>("year_day","_get",DateTimeModule_get_year_day);

		registerMemberPropertyMetamethod<DateTime>("second","_get",DateTimeModule_get_second);
		registerMemberPropertyMetamethod<DateTime>("minute","_get",DateTimeModule_get_minute);
		registerMemberPropertyMetamethod<DateTime>("hour","_get",DateTimeModule_get_hour);

		registerMemberPropertyMetamethod<DateTime>("day","_get",DateTimeModule_get_day);
		registerMemberPropertyMetamethod<DateTime>("month","_get",DateTimeModule_get_month);
		registerMemberPropertyMetamethod<DateTime>("year","_get",DateTimeModule_get_year);


		// Custom user function or classes
		eval(
			StringUtils::format(
				//------------------------------------------------
				// String
				"static String::format(_s,..._args){" // add static function format to String module
				"	return String::formatNative(_s,_args)"
				"}"
				//------------------------------------------------
				// DateTime
				"static DateTime::now(){" // add static function now
				"	return new DateTime()"
				"}"
				"static DateTime::nowUtc(){" // add static function nowUtc
				"   var t=new DateTime();"
				"   t.setUtc()"
				"   return t;"
				"}"
				//------------------------------------------------
				// Console
				"static Console::out(_str_out,..._args){"
				"	Console::outNative(_str_out,_args)"
				"}"
				"static Console::outln(_str_out=\"\",..._args){"
				"	Console::outlnNative(_str_out,_args)"
				"}"
				"static Console::error(_str_error,..._args){"
				"	Console::errorNative(_str_error,_args)"
				"}"
				"static Console::errorln(_str_error,..._args){"
				"	Console::errorlnNative(_str_error,_args)"
				"}"
				//------------------------------------------------
				// System
				"static System::assert(_check,_str_error,..._args){"
				"	if((typeof _check)!=Boolean){"
				"		System::errorNative(\"Error, executing  'System::assert' first parameter must be boolean but it was {0}\",[typeof _check])"
				"	}"
				"	if(_check==false){"
				"		System::errorNative(\"Assert error \"+(_str_error==undefined?\"\":\":\"+_str_error),_args)"
				"	}"
				"}"
				"static System::error(_str_error,..._args){"
				"	System::errorNative(_str_error,_args)"
				"}"
				/*"static System::eval(_str_eval,_params){"
				"	return System::evalNative(_str_eval,_params)"
				"}"*/
				"static System::getZetScript(){"
				"	return ptrToZetScriptPtr(%lu);" // ptrToZetScript it gets current this
				"}"
				//------------------------------------------------
				// Json
				"static Json::serialize(_stk,_formatted=false){"
				"	return Json::serializeNative(_stk,_formatted)"
				"}"
			,
			(intptr_t)this
			)
		);

		saveState();
	}
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	 // PRINT INFO

	void ZetScript::printAllStructSizes(){
		printf(
				"zs_char:%i\n"
				"zs_int:%i\n"
				"zs_float:%i\n"
				"String:%i\n"
				"Vector<StackElement>:%i\n"
				"MapString:%i\n"
				"MapInt:%i\n"
				"bool:%i\n"
				"ZetScript:%i\n"
				"VirtualMachineData:%i\n"
				"ScriptType:%i\n"
				"ScriptFunction:%i\n"
				"Instruction:%i\n"
				"StackElement:%i\n"
				"Symbol:%i\n"
				"ScriptScope:%i\n"
				"ScriptObject:%i\n"
				"StringScriptObject:%i\n"
				"ArrayScriptObject:%i\n"
				"DictionaryScriptObject:%i\n"
				"ClassScriptObject:%i\n"
				"ContainerSlot:%i\n"
				,(int)sizeof(zs_char)
				,(int)sizeof(zs_int)
				,(int)sizeof(zs_float)
				,(int)sizeof(String)
				,(int)sizeof(Vector<StackElement>)
				,(int)sizeof(MapString)
				,(int)sizeof(MapInt)
				,(int)sizeof(bool)
				,(int)sizeof(ZetScript)
				, (int)sizeof(VirtualMachineData)
				, (int)sizeof(ScriptType)
				, (int)sizeof(ScriptFunction)
				, (int)sizeof(Instruction)
				, (int)sizeof(StackElement)
				, (int)sizeof(Symbol)
				, (int)sizeof(ScriptScope)
				, (int)sizeof(ScriptObject)
				, (int)sizeof(StringScriptObject)
				, (int)sizeof(ArrayScriptObject)
				, (int)sizeof(DictionaryScriptObject)
				, (int)sizeof(ClassScriptObject)
				, (int)sizeof(ContainerSlot)
			);
	}

	void ZetScript::printGeneratedCode(bool show_system_code){

		 Vector<ScriptType *> *script_classes=script_types_factory->getScriptTypes();
		 // for all classes print code...
		 ScriptFunction *sf_main=ZS_MAIN_FUNCTION(this);

		 // list functions
		 Vector<Symbol *> *symbol_functions=sf_main->scope->symbol_functions;

		 // print main function
		 ScriptFunction::printGeneratedCode(sf_main);

		 // print defined functions in main function
		 for(int j =0; j < symbol_functions->length(); j++){
			Symbol *symbol=(Symbol *)symbol_functions->get(j);

			if(symbol->properties & SYMBOL_PROPERTY_FUNCTION){

				ScriptFunction *local_sf = (ScriptFunction *)symbol->ref_ptr;
				bool show_function=true;
				 if(show_system_code == false && (
						 StringUtils::startsWith(local_sf->name,"@_afun_defval")
				)){
					 show_function=false;
				 }

				if((show_function == true)
				 &&	(( local_sf->properties & SCRIPT_FUNCTION_PROPERTY_NATIVE_OBJECT_REF) != SCRIPT_FUNCTION_PROPERTY_NATIVE_OBJECT_REF)
				){
					ScriptFunction::printGeneratedCode(local_sf);
				}
			}
		}

		 for(int i = 1; i < script_classes->length(); i++){
			 ScriptType *sc=(ScriptType *)script_classes->get(i);
			 bool show_class=true;

			 // ignore builtin implementations if not chosen ...
			 if(show_system_code == false && (
					   sc->name == "System"
					|| sc->name == "String"
					|| sc->name == "IteratorString"
					|| sc->name == "ScriptObject"
					|| sc->name == "IteratorObject"
					|| sc->name == "Console"
					|| sc->name == "DateTime"
					|| sc->name == "Array"
					|| sc->name == "IteratorArray"
					|| sc->name == "Json"
				)){
				 show_class=false;
			 }

			 if(show_class){
				 symbol_functions=sc->scope->symbol_functions;
				 for(int f = 0; f < symbol_functions->length(); f++){
					 bool show_function=true;
					 Symbol *symbol=(Symbol *)symbol_functions->get(f);

					 if(show_function){
						 ScriptFunction::printGeneratedCode((ScriptFunction *)symbol->ref_ptr,sc);
					 }

				 }
			 }
		}
	}
	 // PRINT ASM INFO
	 //----------------------------------------------------------------------------------------------------------------------------------------------------------------
	 // STACKELEMENT

	String ZetScript::stackElementToStringTypeOf(StackElement *_stk){
		// PRE: _str_out should allocated a minimum of 100 bytes
		StackElement *stk=_stk;
		String result="unknow";
		if(STACK_ELEMENT_IS_UNDEFINED(stk))
			result=ZS_SCRIPT_TYPE_NAME_UNDEFINED; //"undefined";
		else if(STACK_ELEMENT_IS_NULL(stk))
			result=ZS_SCRIPT_TYPE_NAME_NULL; //"null";
		else if(STACK_ELEMENT_IS_INT(stk))
			result=ZS_SCRIPT_TYPE_NAME_INT;
		else if(STACK_ELEMENT_IS_FLOAT(stk))
			result=ZS_SCRIPT_TYPE_NAME_FLOAT;
		else if(STACK_ELEMENT_IS_BOOLEAN(stk))
			result=ZS_SCRIPT_TYPE_NAME_BOOL;
		else if(STACK_ELEMENT_IS_CONTAINER_SLOT(stk))
			result=((ContainerSlot *)stk->value)->getSrcContainerRef()->getScriptTypeName();
		else if(STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(stk))
			result=ZS_SCRIPT_TYPE_NAME_STRING_SCRIPT_OBJECT;
		else if(STACK_ELEMENT_IS_ARRAY_SCRIPT_OBJECT(stk))
			result=ZS_SCRIPT_TYPE_NAME_ARRAY_SCRIPT_OBJECT;
		else if(STACK_ELEMENT_IS_DICTIONARY_SCRIPT_OBJECT(stk))
			result=ZS_SCRIPT_TYPE_NAME_DICTIONARY_SCRIPT_OBJECT;
		else if(STACK_ELEMENT_IS_STRING_ITERATOR_SCRIPT_OBJECT(stk))
			result=ZS_SCRIPT_TYPE_NAME_STRING_ITERATOR_SCRIPT_OBJECT;
		else if(STACK_ELEMENT_IS_ITERATOR_ARRAY_SCRIPT_OBJECT(stk))
			result=ZS_SCRIPT_TYPE_NAME_ARRAY_ITERATOR_SCRIPT_OBJECT;
		else if(STACK_ELEMENT_IS_ITERATOR_DICTIONARY_SCRIPT_OBJECT(stk))
			result=ZS_SCRIPT_TYPE_NAME_DICTIONARY_ITERATOR_SCRIPT_OBJECT;
		else if(STACK_ELEMENT_IS_FUNCTION(stk))
			result=String("fun@")+((ScriptFunction *)(((Symbol *)stk->value)->ref_ptr))->name;
		else if(STACK_ELEMENT_IS_TYPE(stk)) // is a type
			result=String("type@")+this->getScriptTypesFactory()->getScriptTypeName((ScriptTypeId)stk->value);
		else if(STACK_ELEMENT_IS_MEMBER_PROPERTY(stk)){
			StackElementMemberProperty *ma=(StackElementMemberProperty *)stk->value;
			result="prop@"+String(ma->member_property->type->name)+"::"+String(ma->member_property->property_name);
		}else if(STACK_ELEMENT_IS_MEMBER_FUNCTION(stk)){
			Symbol *symbol=((Symbol *)stk->value);
			ScriptFunction *sf=(ScriptFunction *)symbol->ref_ptr;
			result="fun@"+String(sf->scope->owner_type->name)+"::"+sf->name;
		}else{
			if(stk->properties & STACK_ELEMENT_PROPERTY_PTR_STK){
				stk=(StackElement *)stk->value;
			}

			if(stk->properties & STACK_ELEMENT_PROPERTY_OBJECT){
				result=((ScriptObject *)stk->value)->getScriptTypeName();
			}
		}

		return result;
	}

	const char *ZetScript::stackElementToStringTypeOf(char *_str_out, StackElement *_stk){
		auto str=this->stackElementToStringTypeOf(_stk);

		strcpy(_str_out,str.toConstChar());

		return _str_out;
	}

	StackElement ZetScript::unwrapStackElement(StackElement _stk){
		if(_stk.properties & STACK_ELEMENT_PROPERTY_PTR_STK){
			_stk=*((StackElement *)_stk.value);
		}

		if(_stk.properties & STACK_ELEMENT_PROPERTY_CONTAINER_SLOT){
			_stk.value=(zs_int)(((ContainerSlot *)_stk.value)->getSrcContainerRef());
			_stk.properties=STACK_ELEMENT_PROPERTY_OBJECT;
		}

		if(_stk.properties & STACK_ELEMENT_PROPERTY_READ_ONLY){
			_stk.properties&=~STACK_ELEMENT_PROPERTY_READ_ONLY;
		}

		return _stk;
	}

	String ZetScript::stackElementToString(StackElement *_stk, const String  & _format ){
		// PRE: _str_out should allocated a minimum of 100 bytes
		String result="unknown";
		StackElement stk=unwrapStackElement(*_stk);

		if(STACK_ELEMENT_IS_UNDEFINED(&stk)){
			result=ZS_SCRIPT_TYPE_NAME_UNDEFINED;
		}else if(STACK_ELEMENT_IS_NULL(&stk)){
			result=ZS_SCRIPT_TYPE_NAME_NULL;
		}else if(stk.properties & STACK_ELEMENT_PROPERTY_CHAR_PTR){
			result= (const char *)stk.value;
		}else if(STACK_ELEMENT_IS_INT(&stk)){
			result= Integer::toString((zs_int)stk.value,_format);
		}else if(STACK_ELEMENT_IS_FLOAT(&stk)){
			result= Float::toString(ZS_READ_INTPTR_FLOAT(stk.value));
		}else if(STACK_ELEMENT_IS_BOOLEAN(&stk)){
			result= stk.value?"true":"false";
		}else if(STACK_ELEMENT_IS_MEMBER_PROPERTY(&stk)){
			StackElementMemberProperty *ma=(StackElementMemberProperty *)stk.value;
			result="prop@"+String(ma->member_property->type->name)+"::"+String(ma->member_property->property_name);
		}else if(STACK_ELEMENT_IS_FUNCTION(&stk)){
			Symbol *symbol=((Symbol *)stk.value);
			ScriptType *owner_type=symbol->scope->getOwnerType();
			if( owner_type->id == SCRIPT_TYPE_ID_CLASS_MAIN ){
				result= String("fun@")+symbol->name;
			}else{
				String s="";

				if(symbol->properties & SYMBOL_PROPERTY_STATIC){
					s=String("static_fun@");
				}else{
					s=String("member_function<");
				}
				result=s+owner_type->name+"::"+symbol->name;
			}
		}else if(STACK_ELEMENT_IS_TYPE(&stk)){
			result= String("type")+"@"+this->getScriptTypesFactory()->getScriptTypeName((ScriptTypeId)stk.value);
		}else{
			if(stk.properties & STACK_ELEMENT_PROPERTY_OBJECT){
				ScriptObject *so=(ScriptObject *)stk.value;
				if(so->script_type_id==SCRIPT_TYPE_ID_MEMBER_FUNCTION_SCRIPT_OBJECT){
					MemberFunctionObject *somf=(MemberFunctionObject *)so;
					ScriptType *st=somf->getRefObject()->getScriptType();
					result= String("member_function<")+st->name+"::"+somf->sf_ref->name+">";
				}else{
					result=so->toString();
				}
			}
		}

		return result;
	}

	const char		*ZetScript::stackElementToString(char *_str_out, int _str_out_len, StackElement *_stk,const String & _format){
		StackElement stk=unwrapStackElement(*_stk);
		String result;
		memset(_str_out,0,_str_out_len);

		// PROTECTION: do not give you big strings, instead they will retrieve from particular parts of code like JsonSerialize or Console::*)
		if(stk.properties & STACK_ELEMENT_PROPERTY_OBJECT){
			ScriptObject *so=(ScriptObject *)stk.value;
			if(so->script_type_id!=SCRIPT_TYPE_ID_MEMBER_FUNCTION_SCRIPT_OBJECT){
				result="ScriptObject::"+String(so->getScriptTypeName());
			}
		}

		if(result.empty()){
			result=stackElementToString(_stk,_format);
		}

		strncpy(_str_out,result.toConstChar(),(result.length()<(_str_out_len-1))?result.length():_str_out_len-1);

		return _str_out;
	}

	void			ZetScript::stackElementAssign(StackElement *_stk_dst, const StackElement *_stk_src){
		*_stk_dst=*_stk_src;

		if(_stk_dst->properties & STACK_ELEMENT_PROPERTY_PTR_STK){
			*_stk_dst=*((StackElement *)_stk_dst->value);
		}

		// erase read only property if set
		_stk_dst->properties&=~(STACK_ELEMENT_PROPERTY_READ_ONLY);


		// update n_refs +1
		if(_stk_dst->properties&STACK_ELEMENT_PROPERTY_OBJECT){
			ScriptObject *so=(ScriptObject *)_stk_dst->value;
			VirtualMachine *vm=this->getVirtualMachine();

			// create new if constant
			if(so->script_type_id == SCRIPT_TYPE_ID_STRING_SCRIPT_OBJECT && (so->properties & OBJECT_PROPERTY_CONSTANT)){
				StringScriptObject *sc=ZS_NEW_STRING_SCRIPT_OBJECT(this);
				vm_create_shared_object(
						vm
						,sc
				);
				sc->set(so->toString().toConstChar());
				so=sc;
			}

			vm_share_object(vm,so);

			// assign object value into stk
			_stk_dst->value=(zs_int)so;
			_stk_dst->properties=STACK_ELEMENT_PROPERTY_OBJECT;
		}
	}

	bool ZetScript::canStackElementCastTo(
		StackElement * _stack_element
		, ScriptTypeId _script_type_id_to_convert
		, bool _strict
	){
		zs_int val_ret=0;

		// save return type ...
		if(_stack_element->properties & STACK_ELEMENT_PROPERTY_PTR_STK){
			_stack_element=((StackElement *)_stack_element->value);
		}

		if(_script_type_id_to_convert == SCRIPT_TYPE_ID_STACK_ELEMENT){
			return true;
		}

		switch(STACK_ELEMENT_TYPE_PROPERTIES(_stack_element->properties)){
		case STACK_ELEMENT_PROPERTY_NULL:
			return true;
			break;
		case STACK_ELEMENT_PROPERTY_BOOL:
			switch(_script_type_id_to_convert){
			case SCRIPT_TYPE_ID_BOOL:
			case SCRIPT_TYPE_ID_BOOL_PTR:
				return true;
				break;
			default:
				break;
			}
			break;
		case STACK_ELEMENT_PROPERTY_FLOAT:
			switch(_script_type_id_to_convert){
			case SCRIPT_TYPE_ID_FLOAT:
			case SCRIPT_TYPE_ID_FLOAT_PTR:
				return true;
			case SCRIPT_TYPE_ID_INT:
				return _strict == false;
				break;
			default:
				break;

			}
			break;
		case STACK_ELEMENT_PROPERTY_INT:
			switch(_script_type_id_to_convert){
			case SCRIPT_TYPE_ID_INT:
			case SCRIPT_TYPE_ID_INT_PTR:
				return true;
			case SCRIPT_TYPE_ID_FLOAT:
			case SCRIPT_TYPE_ID_FLOAT_PTR:
				return _strict == false;
				break;
			default:
				break;

			}
			break;
		// it expects the ScriptFunction directly
		case STACK_ELEMENT_PROPERTY_FUNCTION:
			return true;
			break;
		default:
			// script variable by default ...
			if(_stack_element->properties & (STACK_ELEMENT_PROPERTY_OBJECT | STACK_ELEMENT_PROPERTY_CONTAINER_SLOT)){
				ScriptObject *object=NULL;
				if(_stack_element->properties & STACK_ELEMENT_PROPERTY_CONTAINER_SLOT){
					object=((ContainerSlot *)(_stack_element->value))->getSrcContainerRef();
				}else{
					object=(ScriptObject *)_stack_element->value;
				}

				if(_script_type_id_to_convert==object->script_type_id){
					return true;
				}

				if(object->script_type_id == SCRIPT_TYPE_ID_STRING_SCRIPT_OBJECT){ // string

					switch(_script_type_id_to_convert){
					case SCRIPT_TYPE_ID_STRING_PTR:
					case SCRIPT_TYPE_ID_STRING:
					case SCRIPT_TYPE_ID_CONST_CHAR_PTR:
						return true;
					default:
						break;
					}

				}else if(object->script_type_id >= SCRIPT_TYPE_ID_CLASS_SCRIPT_OBJECT){
					ClassScriptObject *object_class = NULL;
					ScriptType *c_class=NULL;
					// trivial case
					if(
						_script_type_id_to_convert != SCRIPT_TYPE_ID_CLASS_SCRIPT_OBJECT
					){
						object_class = (ClassScriptObject *)object;
						c_class=object_class->getNativeScriptClass(); // get the pointer directly ...
					}

					if(c_class != NULL){
						if((val_ret=c_class->extendsFrom(
								_script_type_id_to_convert
							))==true
						){
							return true;
						}
					}else{ // Is script class
						return true;
					}
				}
			}

			break;
		}

		return false;
	}

	bool ZetScript::stackElementTo(StackElement * _stack_element, ScriptTypeId _script_type_id_to_convert, zs_int *_ptr_var, String  & _error){
		zs_int val_ret=0;

		ScriptObject *object=NULL;

		// save return type ...
		if(_stack_element->properties & STACK_ELEMENT_PROPERTY_PTR_STK){
			_stack_element=((StackElement *)_stack_element->value);
		}

		if(_script_type_id_to_convert == SCRIPT_TYPE_ID_STACK_ELEMENT){
			*_ptr_var=(zs_int)_stack_element;
			return true;
		}

		switch(STACK_ELEMENT_TYPE_PROPERTIES(_stack_element->properties)){
		case STACK_ELEMENT_PROPERTY_NULL:
			break;
		case STACK_ELEMENT_PROPERTY_BOOL:
			if(_script_type_id_to_convert == SCRIPT_TYPE_ID_BOOL){// *ScriptType::k_str_bool_type){
				val_ret=(zs_int)(_stack_element->value);
			}else if(_script_type_id_to_convert == SCRIPT_TYPE_ID_BOOL_PTR){//*ScriptType::k_str_bool_type_ptr){
				val_ret=(zs_int)(&_stack_element->value);
			}else{
				_error="cannot convert '"
					+Rtti::demangle(k_str_bool_type_ptr)
					+"' to '"
					+Rtti::demangle(ZS_SCRIPT_TYPE_ID_TO_NATIVE_NAME(this->getScriptTypesFactory(),_script_type_id_to_convert)).toConstChar()
					+"'";
				return false;
			}

			break;
		case STACK_ELEMENT_PROPERTY_FLOAT:
			switch(_script_type_id_to_convert){
			case SCRIPT_TYPE_ID_FLOAT:
				ZS_FLOAT_COPY(&val_ret,&_stack_element->value);
				break;
			case SCRIPT_TYPE_ID_FLOAT_PTR:
				val_ret=(zs_int)(&_stack_element->value);
				break;
			case SCRIPT_TYPE_ID_INT:
				{
					zs_int *aux_dst = ((zs_int *)&val_ret);
					zs_float *aux_src=(zs_float *)&_stack_element->value;
					*aux_dst=(zs_int)(*aux_src);
				}
				break;
			default:
				_error="cannot convert '"
					+Rtti::demangle(k_str_float_type_ptr)
					+"' to '"
					+Rtti::demangle(ZS_SCRIPT_TYPE_ID_TO_NATIVE_NAME(this->getScriptTypesFactory(),_script_type_id_to_convert))
					+"'";
				return false;
			}
			break;
		case STACK_ELEMENT_PROPERTY_INT:
			switch(_script_type_id_to_convert){
			case SCRIPT_TYPE_ID_INT:
				val_ret=(zs_int)(_stack_element->value);
				break;
			case SCRIPT_TYPE_ID_INT_PTR:
				val_ret=(zs_int)(&_stack_element->value);
				break;
			default:
				_error="cannot convert 'int' to '"
				+Rtti::demangle(ZS_SCRIPT_TYPE_ID_TO_NATIVE_NAME(this->getScriptTypesFactory(),_script_type_id_to_convert))
				+"'";
				return false;
			}
			break;
		// it expects the ScriptFunction directly
		case STACK_ELEMENT_PROPERTY_FUNCTION:
			val_ret=((Symbol *)_stack_element->value)->ref_ptr;
			break;
		default: // script variable by default ...

			if(_stack_element->properties & (STACK_ELEMENT_PROPERTY_OBJECT | STACK_ELEMENT_PROPERTY_CONTAINER_SLOT)){

				if(_stack_element->properties & STACK_ELEMENT_PROPERTY_CONTAINER_SLOT){
					object=((ContainerSlot *)(_stack_element->value))->getSrcContainerRef();
				}else{
					object=(ScriptObject *)_stack_element->value;
				}

				ScriptType *c_class=NULL;
				val_ret=(zs_int)object;

				if(object==NULL){
					_error="Variable is not defined";
					return false;
				}

				if(_script_type_id_to_convert==object->script_type_id){
					val_ret=(zs_int)object->getNativeObject();
				}else{

					if(object->script_type_id == SCRIPT_TYPE_ID_STRING_SCRIPT_OBJECT){ // string
						if(_script_type_id_to_convert == SCRIPT_TYPE_ID_STRING_PTR){
							val_ret=(zs_int)(((StringScriptObject *)object)->str_ptr);
						}else if(_script_type_id_to_convert == SCRIPT_TYPE_ID_STRING){
							*((String *)_ptr_var)=*(((StringScriptObject *)object)->str_ptr);
							return true;
						}else if (_script_type_id_to_convert == SCRIPT_TYPE_ID_CONST_CHAR_PTR){
							val_ret=(zs_int)(((StringScriptObject *)object)->getConstChar());
						}else{
							_error="cannot convert '"
									+Rtti::demangle((k_str_string_type_ptr))
									+"' to '"
									+Rtti::demangle(ZS_SCRIPT_TYPE_ID_TO_NATIVE_NAME(this->getScriptTypesFactory(),_script_type_id_to_convert))
									+"'";
							return false;
						}
					}else if(object->script_type_id >= SCRIPT_TYPE_ID_CLASS_SCRIPT_OBJECT){
						ClassScriptObject *object_class = NULL;
						// trivial case
						if(
							_script_type_id_to_convert != SCRIPT_TYPE_ID_CLASS_SCRIPT_OBJECT
						){
							object_class = (ClassScriptObject *)object;
							c_class=object_class->getNativeScriptClass(); // get the pointer directly ...
						}

						if(c_class != NULL){
							if((val_ret=c_class->extendsFrom(
									_script_type_id_to_convert
								))==0
							){//c_class->id==idx_builtin_type){
								_error="cannot convert '"
										+Rtti::demangle(c_class->native_name.toConstChar())
										+"' to '"
										+Rtti::demangle(ZS_SCRIPT_TYPE_ID_TO_NATIVE_NAME(this->getScriptTypesFactory(),_script_type_id_to_convert))
										+"'";
								return false;
							}
							val_ret=(zs_int)object_class->getNativeObject();
						}else{ // Is script class, set directly
							val_ret=_stack_element->value;
							/*error = " Error calling function, no C-object parameter! Unexpected script variable (";
							error.append(Rtti::demangle(object->getScriptTypeName().toConstChar()));
							error.append(")");
							return false;*/
						}
					}else{ // cannot convert...
						_error="cannot convert '"
							+Rtti::demangle(object->getScriptTypeName())
							+"' to '"
							+Rtti::demangle(ZS_SCRIPT_TYPE_ID_TO_NATIVE_NAME(this->getScriptTypesFactory(),_script_type_id_to_convert))
							+"'";
						return false;
					}
				}
			}else{
				_error=StringUtils::format("Cannot know how to convert type '%s' from '%s'"
					,Rtti::demangle(ZS_SCRIPT_TYPE_ID_TO_NATIVE_NAME(this->getScriptTypesFactory(),_script_type_id_to_convert)).toConstChar()
					,this->stackElementToStringTypeOf(_stack_element).toConstChar()
				);
				return false;
			}
			break;
		}

		// prevent to
		if(_script_type_id_to_convert == SCRIPT_TYPE_ID_BOOL){
			*((bool *)_ptr_var) = val_ret;
		}else{
			*_ptr_var = val_ret;
		}


		return true;
	}

	// Helpers...
	 StackElement ZetScript::toStackElement(zs_int ptr_var, ScriptTypeId _var_script_type_id){
		//zs_int ptr_var = (zs_int)input_var;
			StackElement stk_result=k_stk_undefined;
			StringScriptObject *so=NULL;

			//int idx_builtin_type=getScriptTypeIdFromNamePtr(typeid(T).name());
			// save return type ...
			switch(_var_script_type_id){
			 case SCRIPT_TYPE_ID_VOID:
				break;
			 case SCRIPT_TYPE_ID_INT_PTR:
				 if(ptr_var==0) return stk_result;
				 stk_result={(*((zs_int *)ptr_var)),STACK_ELEMENT_PROPERTY_INT};
				 break;
			 case SCRIPT_TYPE_ID_INT:
				 stk_result={(((zs_int)ptr_var)),STACK_ELEMENT_PROPERTY_INT};
				 break;
			 case SCRIPT_TYPE_ID_FLOAT:
				 stk_result.properties=STACK_ELEMENT_PROPERTY_FLOAT;//{};
				 ZS_FLOAT_COPY(&stk_result.value,&ptr_var);
				 break;
			 case SCRIPT_TYPE_ID_FLOAT_PTR:
				 if(ptr_var==0) return stk_result;
				 stk_result.properties=STACK_ELEMENT_PROPERTY_FLOAT;//{};
				 ZS_FLOAT_COPY(&stk_result.value,&(*(zs_float *)ptr_var));
				 break;
			 case SCRIPT_TYPE_ID_BOOL_PTR:
				 if(ptr_var==0) return stk_result;
				 stk_result={(*((bool *)ptr_var)),STACK_ELEMENT_PROPERTY_BOOL};
				 break;
			 case SCRIPT_TYPE_ID_BOOL:
				 stk_result={(((bool)ptr_var)),STACK_ELEMENT_PROPERTY_BOOL};
				 break;
			 case SCRIPT_TYPE_ID_CONST_CHAR_PTR:
			 case SCRIPT_TYPE_ID_STRING_PTR:
			 case SCRIPT_TYPE_ID_STRING:


				 so=ZS_NEW_STRING_SCRIPT_OBJECT(this);
				 if(ptr_var!=0) { // not null
					 if(_var_script_type_id == SCRIPT_TYPE_ID_STRING_PTR){
						// assing String reference
						so->str_ptr=(String *)ptr_var;
					 }else if(_var_script_type_id == SCRIPT_TYPE_ID_STRING){ // String passed as pointer
						 so->set(*((String *)ptr_var));
					 }else{ // const char
						 so->set((const char *)ptr_var);
					 }
				 }

				 stk_result={(intptr_t)so,STACK_ELEMENT_PROPERTY_OBJECT};
				 break;
			 case SCRIPT_TYPE_ID_STACK_ELEMENT:
				 if(ptr_var==0) return stk_result;
				 stk_result=*((StackElement *)ptr_var);
				 break;
			 case SCRIPT_TYPE_ID_ARRAY_SCRIPT_OBJECT:
			 case SCRIPT_TYPE_ID_ARRAY_ITERATOR_SCRIPT_OBJECT:
			 case SCRIPT_TYPE_ID_DICTIONARY_SCRIPT_OBJECT:
			 case SCRIPT_TYPE_ID_CLASS_SCRIPT_OBJECT:
			 case SCRIPT_TYPE_ID_DICTIONARY_ITERATOR_SCRIPT_OBJECT:
			 case SCRIPT_TYPE_ID_STRING_SCRIPT_OBJECT:
			 case SCRIPT_TYPE_ID_STRING_ITERATOR_SCRIPT_OBJECT:
				 if(ptr_var==0) return stk_result;
				stk_result = {
					 (intptr_t)ptr_var
					 ,STACK_ELEMENT_PROPERTY_OBJECT
				 };

				 break;
			 default:
				 //if(ptr_var==0) return stk_result;
				 if(ptr_var == 0){ // null value
					 stk_result={0,STACK_ELEMENT_PROPERTY_NULL};
				 }else{
					 stk_result = {
						 (intptr_t)this->getScriptTypesFactory()->instanceObjectByScriptTypeId(_var_script_type_id,(void *)ptr_var)
						 ,STACK_ELEMENT_PROPERTY_OBJECT
					 };
				 }
				 break;
			}
			return stk_result;
	}

	 // STACKELEMENT
	 //----------------------------------------------------------------------------------------------------------------------------------------------------------------
	 // FILE MANAGEMENT
	bool ZetScript::isFilenameAlreadyParsed(const String & filename){
		for(int i = 0; i < parsed_files.length(); i++){
			if(((ParsedFile *)parsed_files.get(i))->filename==filename){
				return true;
			}
		}
		return false;
	}

	StackElement ZetScript::evalInternal(const char * _code, unsigned short _eval_options, const char * _filename, EvalData *_eval_data_from, const char *__invoke_file__, int __invoke_line__)  {
		StackElement stk_ret=k_stk_undefined;

		eval_parse_and_compile(this,_code,_eval_data_from,_filename);
		link();

		if(_eval_options & EVAL_OPTION_PRINT_BYTE_CODE){
			printGeneratedCode(_eval_options & EVAL_OPTION_PRINT_ALL_BYTE_CODE);
		}

		if((_eval_options & EVAL_OPTION_NO_EXECUTE)==0){
			// the first code to execute is the main function that in fact is a special member function inside our main class
			stk_ret=vm_execute(
					virtual_machine
					,NULL
					,script_types_factory->getMainFunction()
					,NULL
					,0
					,0
					,__invoke_file__
					,__invoke_line__
			);
		}

		return stk_ret;
	}
	//
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	//
	// REGISTER CONSTANTS
	//

	void ZetScript::registerConstant(const String & _key, int _value, const char *registered_file, short registered_line){
		Symbol *symbol_variable=ZS_MAIN_FUNCTION(this)->registerLocalVariable(
			ZS_MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, _key
		);

		StackElement *stk=vm_get_stack_element_at(this->virtual_machine,symbol_variable->idx_position);
		stk->value=_value;
		stk->properties=STACK_ELEMENT_PROPERTY_INT|STACK_ELEMENT_PROPERTY_READ_ONLY;
	}

	void ZetScript::registerConstant(const String & _key, bool _value, const char *registered_file, short registered_line){
		Symbol *symbol_variable=ZS_MAIN_FUNCTION(this)->registerLocalVariable(
			ZS_MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, _key
		);

		StackElement *stk=vm_get_stack_element_at(this->virtual_machine,symbol_variable->idx_position);

		stk->value=_value;
		stk->properties=STACK_ELEMENT_PROPERTY_BOOL|STACK_ELEMENT_PROPERTY_READ_ONLY;
	}

	void ZetScript::registerConstant(const String & _key, zs_float _value, const char *registered_file, short registered_line){
		Symbol *symbol_variable=ZS_MAIN_FUNCTION(this)->registerLocalVariable(
			ZS_MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, _key
		);

		StackElement *stk=vm_get_stack_element_at(this->virtual_machine,symbol_variable->idx_position);
		ZS_FLOAT_COPY(&stk->value,&_value);
		stk->properties=STACK_ELEMENT_PROPERTY_FLOAT|STACK_ELEMENT_PROPERTY_READ_ONLY;
	}

	void ZetScript::registerConstant(const String & _key, const String & _value, const char *registered_file, short registered_line){
		Symbol *symbol_variable=ZS_MAIN_FUNCTION(this)->registerLocalVariable(
			ZS_MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, _key
		);

		StackElement *stk=vm_get_stack_element_at(this->virtual_machine,symbol_variable->idx_position);
		StringScriptObject *so=ZS_NEW_STRING_SCRIPT_OBJECT(this);
		so->set(_value);

		// create and share pointer
		vm_create_shared_object(this->virtual_machine,so,vm_get_main_scope_block(this->virtual_machine));
		vm_share_object(this->virtual_machine,so);

		stk->value=(zs_int)so;
		stk->properties=STACK_ELEMENT_PROPERTY_OBJECT|STACK_ELEMENT_PROPERTY_READ_ONLY;
		//*stk=*(this->registerStkConstantStringScriptObject(_key,_value));
	}

	void ZetScript::registerConstant(const String & _key, const char * _value, const char *registered_file, short registered_line){
		registerConstant(_key, String(_value), registered_file, registered_line);
	}
	//
	// REGISTER CONSTANTS
	//-----------------------------------------------------------------------------------------
	//
	// STK REGISTER OBJECT
	//
	StackElement * ZetScript::registerStkConstantStringScriptObject(const String & _key,const String & _value){

		StackElement *stk=NULL;
		StringScriptObject *so=NULL;

		if((stk = getStkConstantStringScriptObject(_key))!=NULL){
			if(stk->properties & (STACK_ELEMENT_PROPERTY_OBJECT | STACK_ELEMENT_PROPERTY_READ_ONLY)){
				return stk;
			}
			else{
				// throw
				ZS_THROW_RUNTIME_ERROR(
					"Cannot register constant '%s' as 'StringScriptObject', because is already registered as '%s'"
					,_key.toConstChar()
					,this->stackElementToStringTypeOf(stk).toConstChar()
				);
			}
		}

		stk=new StackElement;

		stk_constants->set(_key.toConstChar(),(zs_int)stk);

		so=ZS_NEW_STRING_SCRIPT_OBJECT(this);
		so->properties=OBJECT_PROPERTY_CONSTANT;

		// swap values stk_ref/value
		so->set(_value.toConstChar());
		stk->value=(zs_int)so;
		stk->properties=STACK_ELEMENT_PROPERTY_OBJECT | STACK_ELEMENT_PROPERTY_READ_ONLY;

		return stk;
	}

	StackElement *ZetScript::getStkConstantStringScriptObject(const String & _key){
		return (StackElement *)stk_constants->get(_key.toConstChar());
	}

	//
	// STK REGISTER OBJECT
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------


	DictionaryScriptObject * ZetScript::newDictionaryScriptObject(){
		return DictionaryScriptObject::newDictionaryScriptObject(this);
	}

	StringScriptObject * ZetScript::newStringScriptObject(const String & _str){
		return StringScriptObject::newStringScriptObject(this,_str);
	}

	ArrayScriptObject * ZetScript::newArrayScriptObject(){
		return ArrayScriptObject::newArrayScriptObject(this);
	}

	StackElement ZetScript::eval(const String & _expresion, unsigned short _options, const char * _script_filename_by_ref, const char *__invoke_file__, int __invoke_line__)  {

		const char *script_filename_by_ref=getFilenameByRef(_script_filename_by_ref);

		return evalInternal(_expresion.toConstChar(), _options, script_filename_by_ref,NULL,__invoke_file__,__invoke_line__);
	}

	StackElement	ZetScript::eval(const String & _expresion, const char *__invoke_file__, int __invoke_line__){
		return evalInternal(_expresion.toConstChar(), 0, NULL,NULL,__invoke_file__,__invoke_line__);
	}

	StackElement ZetScript::evalFile(const String &  _filename, uint16_t _eval_options, EvalData *_eval_data_from, const char *__invoke_file__, int __invoke_line__){
		//int idx_file=-1;
		StackElement stk_ret;
		Buffer *buf_tmp=NULL;

		stk_ret.setUndefined();

		if(!isFilenameAlreadyParsed(_filename)){
			ParsedFile *ps=new ParsedFile();
			String current_directory="";
			ps->filename = _filename;
			parsed_files.push(ps);
			const char * const_file_char=ps->filename.toConstChar();
			//idx_file=parsed_files.length()-1;

			buf_tmp=File::readText(_filename);

			// file exist and can read ... set current pwd
			current_directory = Directory::getCurrentDirectory();
			Directory::changeDir(Path::getDirectory(_filename));

			String str_error="";
			String error_file="";

			bool error=false;


			int error_line=-1;
			try{
				stk_ret=evalInternal(
						(const char *)buf_tmp->ptr
						,_eval_options
						,const_file_char
						,_eval_data_from
						,__invoke_file__
						,__invoke_line__
				);
			}catch(Exception & e){
				error=true;
				error_file=e.getFilename();
				error_line=e.getLine();
				str_error=e.what();
			}catch(std::exception & e){
				error=true;
				str_error=e.what();
			}

			// restore previous directory
			Directory::changeDir(current_directory);

			// deallocate before throw errors...
			delete buf_tmp;

			if(error){
				if(error_file.empty()==false){
					ZS_THROW_EXCEPTION_FILE_LINE(error_file.toConstChar(),error_line,str_error.toConstChar());
				}else{
					ZS_THROW_EXCEPTION(str_error.toConstChar());
				}
			}


		}

		return stk_ret;
	}

	void ZetScript::clearGlobalVariables(int _idx_start_variable, int _idx_start_function){
		ZS_UNUSUED_PARAM(_idx_start_function);
		String global_symbol;
		int idx_start_variable = _idx_start_variable == ZS_UNDEFINED_IDX ?  idx_current_global_variable_checkpoint:_idx_start_variable;
		ScriptFunction *main_function_object=script_types_factory->getMainFunction();
		ScriptScope *main_scope=ZS_MAIN_SCOPE(this);
		Vector<Symbol *> *local_variables=main_function_object->local_variables;
		Vector<Symbol *> *global_symbol_variables= main_scope->symbol_variables;
		int n_global_symbol_found=0;
		int v=local_variables->length()-1;


		// Because all symbols are ordered by scope, have G as global symbols and L local symbols the disposition is the following,
		//
		// [G][G][G][G]..[G][L][L][L][L][L]..[L]
		//
		// So we only have to delete symbols
		vm_deref_cyclic_references(this->virtual_machine);

		// remove all shared 0 pointers
		if(local_variables->length() > 0){

			// remove cyclic container instances
			for (
				;v>=idx_start_variable;
			) {

				StackElement *vm_stk_element=&vm_get_stack_elements(virtual_machine)[v];

				Symbol *symbol=(Symbol *)local_variables->get(v);


				ScriptObject *object = NULL;

				if(symbol != NULL && symbol->scope == main_scope){ // if variable in global scope

					if(
							(vm_stk_element->properties & STACK_ELEMENT_PROPERTY_OBJECT)
					){
						object =((ScriptObject *)(vm_stk_element->value));
						if(object!=NULL){
							if(object->shared_pointer != NULL){
								vm_unref_shared_object(this->virtual_machine,object,NULL);
							}
						}
					}
					n_global_symbol_found++;

					// erase unused global symbol
					delete symbol;
				}

				STACK_ELEMENT_SET_UNDEFINED(vm_stk_element);

				--v;
			}

			// clear all garbage
			StackElement *vm_stack=vm_get_stack_elements(virtual_machine);
			memset(vm_stack+idx_start_variable,0,sizeof(StackElement)*(ZS_VM_STACK_MAX-idx_start_variable));

			// erase global elements that they weren't saved...
			int resize=local_variables->length()-(local_variables->length()-idx_start_variable);
			local_variables->resize(resize);
			global_symbol_variables->resize(global_symbol_variables->length()-n_global_symbol_found);
		}

		// remove any temporal memory pointers
		vm_remove_empty_shared_pointers(
				this->virtual_machine
			,vm_get_scope_block_main(this->virtual_machine)
		);
	}

	const char *ZetScript::getFilenameByRef(const char * _filename_by_ref){
		if(_filename_by_ref==NULL){
			return NULL;
		}

		bool exists=false;
		zs_int e=script_filenames_by_ref->get(_filename_by_ref,&exists);
		if(exists==false){
			char *s=(char *)malloc(strlen(_filename_by_ref)+1);
			strcpy(s,_filename_by_ref);
			script_filenames_by_ref->set(s,(zs_int)s);
			e=(zs_int)s;
		}
		return ((const char *)e);
	}


	void ZetScript::resetParsedFiles(){
		for(int i=0;i<parsed_files.length();i++){
			delete ((ParsedFile *)parsed_files.get(i));
		}
		parsed_files.clear();
	}

	void ZetScript::clear(){

		// clearGlobalVariables
		clearGlobalVariables();

		// clearGlobalFunctions
		ScriptScope *main_scope=ZS_MAIN_SCOPE(this);
		Vector<Symbol *> *global_symbol_functions= main_scope->symbol_functions;
		int v=global_symbol_functions->length()-1;
		// remove all shared 0 pointers
		if(v >= idx_current_global_function_checkpoint){
			for (
				;v>=idx_current_global_function_checkpoint;
			) {


				Symbol *symbol=(Symbol *)global_symbol_functions->get(v);//(Symbol *)main_function_object->registered_symbols->items[v];
				delete symbol;
				--v;
			}

			int resize=global_symbol_functions->length()-(global_symbol_functions->length()-idx_current_global_function_checkpoint);
			global_symbol_functions->resize(resize);
		}


		// clearTypes
		Vector<Symbol *> *types= main_scope->symbol_types;
		v=types->length()-1;
		// remove all shared 0 pointers
		if(v >=idx_current_types_checkpoint){
			for (
				;v>=idx_current_types_checkpoint;
			) {


				Symbol *symbol=(Symbol *)types->get(v);//(Symbol *)main_function_object->registered_symbols->items[v];
				delete symbol;
				--v;
			}

			int resize=types->length()-(types->length()-idx_current_types_checkpoint);
			types->resize(resize);
		}

		functions_with_unresolved_symbols.clear();

		scope_factory->clear();
		script_function_factory->clear();
		script_types_factory->clear();

		resetParsedFiles();


	}

	void ZetScript::saveState(){
		ScriptFunction *main_function_object=script_types_factory->getMainFunction();
		ScriptScope *main_scope=ZS_MAIN_SCOPE(this);
		idx_current_global_variable_checkpoint=main_function_object->local_variables->length();
		idx_current_global_function_checkpoint=main_scope->symbol_functions->length();
		idx_current_types_checkpoint=main_scope->symbol_types->length();

		scope_factory->saveState();
		script_function_factory->saveState();
		script_types_factory->saveState();
	}

	bool ZetScript::getFunctionWithUnresolvedSymbolExists(ScriptFunction *_sf){
		for(int i=0;i < functions_with_unresolved_symbols.length(); i++){
			if(functions_with_unresolved_symbols.get(i)==_sf){
				return true;
			}
		}

		return false;

	}

	void ZetScript::addUnresolvedSymbol(ScriptFunction *_sf, zs_int idx_instruction){
		if(getFunctionWithUnresolvedSymbolExists(_sf) ==false){
			functions_with_unresolved_symbols.push(_sf);
		}
		_sf->addUnresolvedSymbol(idx_instruction);
	}

	void ZetScript::link(){
		int i=0;
		while(i<functions_with_unresolved_symbols.length()){
			ScriptFunction *_sf=(ScriptFunction *)functions_with_unresolved_symbols.get(i);
			if(_sf->linkUnresolvedSymbols()){ // if link all symbols, erase
				functions_with_unresolved_symbols.erase(i);
			}else{ // next function
				i++;
			}
		}
	}

	void ZetScript::unrefLifetimeObject(ScriptObject *so){
		vm_unref_lifetime_object(this->virtual_machine,so);
	}

	void ZetScript::makeObjectShared(ScriptObject *so){
		if(so->shared_pointer != NULL){
			// share this variable++
			vm_create_shared_object(virtual_machine,so);
			vm_share_object(virtual_machine,so);
		}else{
			ZS_THROW_RUNTIME_ERRORF("Script object already shared");
		}
	}

	ZetScript::~ZetScript(){
		// delete system and string...
		clearGlobalVariables(0,0);

		// clear objects...
		vm_delete(virtual_machine);
		
		delete script_function_factory;
		delete script_types_factory;
		delete scope_factory;

		virtual_machine=NULL;

		if(script_filenames_by_ref != NULL){

			for(auto it=script_filenames_by_ref->begin(); !it.end();it.next()){
				char *script_filename_by_ref=(char *)(it.value);
				free(script_filename_by_ref);
			}

			delete script_filenames_by_ref;
			script_filenames_by_ref=NULL;
		}

		resetParsedFiles();

		functions_with_unresolved_symbols.clear();

		// clear compiled symbol name
		/*for(auto it=compiled_symbol_name->begin();!it.end(); it.next()){
				free((char *)it.value);
		}*/
		delete compiled_symbol_name;
		compiled_symbol_name=NULL;

		if(stk_constants != NULL){
			for(auto it=stk_constants->begin(); !it.end();it.next()){
				StackElement *stk=(StackElement *)(it.value);
				if(stk->properties & STACK_ELEMENT_PROPERTY_OBJECT){
					delete (StringScriptObject *)stk->value;
				}
				delete stk;
			}
			stk_constants->clear();
			delete stk_constants;
		}

	}
}
