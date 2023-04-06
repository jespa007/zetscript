/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#include "ByteCode.cpp"
#include "Instruction.cpp"
#include "StackElement.cpp"
#include "Symbol.cpp"
#include "zs_exception.cpp"

namespace zetscript{

	ZetScript::ZetScript(){
		eval_int=0;
		eval_float=0;
		eval_string="";
		eval_bool = false;
		idx_current_global_variable_checkpoint=0;
		idx_current_global_function_checkpoint=0;
		idx_current_script_types_checkpoint=0;

		script_filenames_by_ref=NULL;

		eval_init();
		scope_factory = new ScopeFactory(this);
		scope_factory->init();
		script_function_factory= new ScriptFunctionFactory(this);

		virtual_machine = vm_new(this);
		script_type_factory = new ScriptTypeFactory(this);
		script_type_factory->init();
		vm_init(virtual_machine,this);

		script_type_factory->setup();

		script_filenames_by_ref=new zs_map();
		stk_constants=new zs_map();
		compiled_symbol_name = new zs_map();
		//-------------------------
		// Register built in modules

		// Math mod
		ScriptType *cl=script_type_factory->registerScriptType("Math","",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);

		cl->registerStaticMemberPropertyGetter("PI",MathModule_PI);
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
		cl=script_type_factory->registerScriptType("Console","",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);
		cl->registerStaticMemberFunction("readChar",ConsoleModule_readChar);
		cl->registerStaticMemberFunction("outNative",ConsoleModule_out);
		cl->registerStaticMemberFunction("outlnNative",ConsoleModule_outln);
		cl->registerStaticMemberFunction("errorNative",ConsoleModule_error);
		cl->registerStaticMemberFunction("errorlnNative",ConsoleModule_errorln);

		// System mod
		cl=script_type_factory->registerScriptType("System","",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);
		cl->registerStaticMemberFunction("clock",SystemModule_clock);
		cl->registerStaticMemberFunction("eval",static_cast<void(*)(ZetScript *, StringScriptObject *)>(SystemModule_eval));
		cl->registerStaticMemberFunction("eval",static_cast<void(*)(ZetScript *, StringScriptObject *,ObjectScriptObject *)>(SystemModule_eval));
		//cl->registerStaticMemberFunction("assertNative",SystemModule_assert);
		cl->registerStaticMemberFunction("errorNative",SystemModule_error);

		// Json mod
		cl=script_type_factory->registerScriptType("Json","",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);
		cl->registerStaticMemberFunction("serializeNative",static_cast<StringScriptObject * (*)(ZetScript *zs,StackElement *)>(JsonModule_serialize));
		cl->registerStaticMemberFunction("serializeNative",static_cast<StringScriptObject * (*)(ZetScript *zs,StackElement *, bool *)>(JsonModule_serialize));
		cl->registerStaticMemberFunction("deserialize",JsonModule_deserialize);
		//---------------------------------------------
		// TimeSpan
		cl=registerClass<zs_timespan>("TimeSpan",TimeSpanModule_new,TimeSpanModule_delete);
		registerMemberPropertyGetter<zs_timespan>("days",TimeSpanModule_get_days);
		registerMemberPropertyGetter<zs_timespan>("hour",TimeSpanModule_get_hours);
		registerMemberPropertyGetter<zs_timespan>("minute",TimeSpanModule_get_minutes);
		registerMemberPropertyGetter<zs_timespan>("second",TimeSpanModule_get_seconds);

		//---------------------------------------------
		// DateTime
		cl=registerClass<zs_datetime>("DateTime",DateTimeModule_new,DateTimeModule_delete);
		registerConstructor<zs_datetime>(DateTimeModule_constructor);
		/*registerStaticMemberFunction<zs_datetime>("_add",DateTimeModule_add);*/
		registerStaticMemberFunction<zs_datetime>("_sub",DateTimeModule_sub);

		registerMemberFunction<zs_datetime>("setUtc",DateTimeModule_setUtc);

		registerMemberFunction<zs_datetime>("addSeconds",DateTimeModule_addSeconds);
		registerMemberFunction<zs_datetime>("addMinutes",DateTimeModule_addMinutes);
		registerMemberFunction<zs_datetime>("addHours",DateTimeModule_addHours);
		registerMemberFunction<zs_datetime>("addDays",DateTimeModule_addDays);
		registerMemberFunction<zs_datetime>("addMonths",DateTimeModule_addMonths);
		registerMemberFunction<zs_datetime>("addYears",DateTimeModule_addYears);

		// metamethods...
		registerMemberFunction<zs_datetime>("_tostring",DateTimeModule_tostring);

		registerMemberPropertyGetter<zs_datetime>("week_day",DateTimeModule_get_week_day);
		registerMemberPropertyGetter<zs_datetime>("month_day",DateTimeModule_get_month_day);
		registerMemberPropertyGetter<zs_datetime>("year_day",DateTimeModule_get_year_day);

		registerMemberPropertyGetter<zs_datetime>("second",DateTimeModule_get_second);
		registerMemberPropertyGetter<zs_datetime>("minute",DateTimeModule_get_minute);
		registerMemberPropertyGetter<zs_datetime>("hour",DateTimeModule_get_hour);

		registerMemberPropertyGetter<zs_datetime>("day",DateTimeModule_get_day);
		registerMemberPropertyGetter<zs_datetime>("month",DateTimeModule_get_month);
		registerMemberPropertyGetter<zs_datetime>("year",DateTimeModule_get_year);


		// Custom user function or classes
		eval(
			zs_strutils::format(
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
				"bool:%i\n"
				"ZetScript:%i\n"
				"VirtualMachineData:%i\n"
				"ScriptType:%i\n"
				"ScriptFunction:%i\n"
				"Instruction:%i\n"
				"StackElement:%i\n"
				"Symbol:%i\n"
				"Scope:%i\n"
				"ScriptObject:%i\n"
				"StringScriptObject:%i\n"
				"ArrayScriptObject:%i\n"
				"ObjectScriptObject:%i\n"
				"ClassScriptObject:%i\n"
				"ContainerSlot:%i\n"
				,(int)sizeof(zs_char)
				,(int)sizeof(zs_int)
				,(int)sizeof(zs_float)
				,(int)sizeof(bool)
				,(int)sizeof(ZetScript)
				, (int)sizeof(VirtualMachineData)
				, (int)sizeof(ScriptType)
				, (int)sizeof(ScriptFunction)
				, (int)sizeof(Instruction)
				, (int)sizeof(StackElement)
				, (int)sizeof(Symbol)
				, (int)sizeof(Scope)
				, (int)sizeof(ScriptObject)
				, (int)sizeof(StringScriptObject)
				, (int)sizeof(ArrayScriptObject)
				, (int)sizeof(ObjectScriptObject)
				, (int)sizeof(ClassScriptObject)
				, (int)sizeof(ContainerSlot)
			);
	}


	 void ZetScript::printGeneratedCode(bool show_system_code){

		 zs_vector<ScriptType *> *script_classes=script_type_factory->getScriptTypes();
		 // for all classes print code...
		 ScriptFunction *sf_main=ZS_MAIN_FUNCTION(this);

		 // list functions
		 zs_vector<Symbol *> *symbol_functions=sf_main->scope_script_function->symbol_functions;

		 // print main function
		 ScriptFunction::printGeneratedCode(sf_main);

		 // print defined functions in main function
		 for(int j =0; j < symbol_functions->size(); j++){
			Symbol *symbol=(Symbol *)symbol_functions->items[j];

			if(symbol->properties & SYMBOL_PROPERTY_FUNCTION){

				ScriptFunction *local_sf = (ScriptFunction *)symbol->ref_ptr;
				bool show_function=true;
				 if(show_system_code == false && (
						 zs_strutils::starts_with(local_sf->name_script_function,"@_afun_defval")
				)){
					 show_function=false;
				 }

				if((show_function == true)
				 &&	(( local_sf->properties & FUNCTION_PROPERTY_C_OBJECT_REF) != FUNCTION_PROPERTY_C_OBJECT_REF)
				){
					ScriptFunction::printGeneratedCode(local_sf);
				}
			}
		}

		 for(int i = 1; i < script_classes->size(); i++){
			 ScriptType *sc=(ScriptType *)script_classes->get(i);
			 bool show_class=true;

			 // ignore builtin implementations if not chosen ...
			 if(show_system_code == false && (
					   sc->str_script_type == "System"
					|| sc->str_script_type == "String"
					|| sc->str_script_type == "IteratorString"
					|| sc->str_script_type == "Object"
					|| sc->str_script_type == "IteratorObject"
					|| sc->str_script_type == "Console"
					|| sc->str_script_type == "DateTime"
					|| sc->str_script_type == "Array"
					|| sc->str_script_type == "IteratorArray"
					|| sc->str_script_type == "Json"
				)){
				 show_class=false;
			 }

			 if(show_class){
				 symbol_functions=sc->scope_script_type->symbol_functions;
				 for(int f = 0; f < symbol_functions->size(); f++){
					 bool show_function=true;
					 Symbol *symbol=(Symbol *)symbol_functions->items[f];

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

	zs_string ZetScript::stackElementToStringTypeOf(StackElement *_stk){
		// PRE: _str_out should allocated a minimum of 100 bytes
		StackElement *stk=_stk;
		zs_string result="unknow";
		if(STK_VALUE_IS_UNDEFINED(stk))
			result=ZS_TYPE_NAME_UNDEFINED; //"undefined";
		else if(STK_VALUE_IS_NULL(stk))
			result=ZS_TYPE_NAME_NULL; //"null";
		else if(STK_VALUE_IS_ZS_INT(stk))
			result=ZS_TYPE_NAME_INT;
		else if(STK_VALUE_IS_ZS_FLOAT(stk))
			result=ZS_TYPE_NAME_FLOAT;
		else if(STK_VALUE_IS_BOOLEAN(stk))
			result=ZS_TYPE_NAME_BOOL;
		else if(STK_VALUE_IS_CONTAINER_SLOT(stk))
			result=((ContainerSlot *)stk->value)->getSrcContainerRef()->getTypeName();
		else if(STK_IS_STRING_SCRIPT_OBJECT(stk))
			result=ZS_TYPE_NAME_OBJECT_STRING;
		else if(STK_IS_ARRAY_SCRIPT_OBJECT(stk))
			result=ZS_TYPE_NAME_OBJECT_ARRAY;
		else if(STK_IS_OBJECT_SCRIPT_OBJECT(stk))
			result=ZS_TYPE_NAME_OBJECT_OBJECT;
		else if(STK_IS_ITERATOR_STRING_SCRIPT_OBJECT(stk))
			result=ZS_TYPE_NAME_OBJECT_ITERATOR_STRING;
		else if(STK_IS_ITERATOR_ARRAY_SCRIPT_OBJECT(stk))
			result=ZS_TYPE_NAME_OBJECT_ITERATOR_ARRAY;
		else if(STK_IS_ITERATOR_OBJECT_SCRIPT_OBJECT(stk))
			result=ZS_TYPE_NAME_OBJECT_ITERATOR_OBJECT;
		else if(STK_VALUE_IS_FUNCTION(stk))
			result=zs_string("fun@")+((ScriptFunction *)(((Symbol *)stk->value)->ref_ptr))->name_script_function;
		else if(STK_VALUE_IS_TYPE(stk)) // is a type
			result=zs_string("type@")+this->getScriptTypeFactory()->getScriptTypeName(stk->value);
		else if(STK_VALUE_IS_MEMBER_PROPERTY(stk)){
			StackMemberProperty *ma=(StackMemberProperty *)stk->value;
			result="prop@"+zs_string(ma->member_property->script_type->str_script_type)+"::"+zs_string(ma->member_property->property_name);
		}else if(STK_VALUE_IS_MEMBER_FUNCTION(stk)){
			Symbol *symbol=((Symbol *)stk->value);
			ScriptFunction *sf=(ScriptFunction *)symbol->ref_ptr;
			result="fun@"+zs_string(sf->scope_script_function->script_type_owner->str_script_type)+"::"+sf->name_script_function;
		}else{
			if(stk->properties & STK_PROPERTY_PTR_STK){
				stk=(StackElement *)stk->value;
			}

			if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){
				result=((ScriptObject *)stk->value)->getTypeName();
			}
		}

		return result;
	}

	const char *ZetScript::stackElementToStringTypeOf(char *_str_out, StackElement *_stk){
		auto str=this->stackElementToString(_stk);

		strcpy(_str_out,str.c_str());

		return _str_out;
	}

	StackElement ZetScript::unwrapStackElement(StackElement _stk){
		if(_stk.properties & STK_PROPERTY_PTR_STK){
			_stk=*((StackElement *)_stk.value);
		}

		if(_stk.properties & STK_PROPERTY_CONTAINER_SLOT){
			_stk.value=(zs_int)(((ContainerSlot *)_stk.value)->getSrcContainerRef());
			_stk.properties=STK_PROPERTY_SCRIPT_OBJECT;
		}

		if(_stk.properties & STK_PROPERTY_READ_ONLY){
			_stk.properties&=~STK_PROPERTY_READ_ONLY;
		}

		return _stk;
	}

	zs_string ZetScript::stackElementToString(StackElement *_stk, const zs_string  & _format ){
		// PRE: _str_out should allocated a minimum of 100 bytes
		zs_string result="unknown";
		StackElement stk=unwrapStackElement(*_stk);

		if(STK_VALUE_IS_UNDEFINED(&stk)){
			result=ZS_TYPE_NAME_UNDEFINED;
		}else if(STK_VALUE_IS_NULL(&stk)){
			result=ZS_TYPE_NAME_NULL;
		}else if(stk.properties & STK_PROPERTY_ZS_CHAR_PTR){
			result= (const char *)stk.value;
		}else if(STK_VALUE_IS_ZS_INT(&stk)){
			result= zs_strutils::zs_int_to_str((zs_int)stk.value,_format);
		}else if(STK_VALUE_IS_ZS_FLOAT(&stk)){
			result= zs_strutils::zs_float_to_str(ZS_READ_INTPTR_FLOAT(stk.value));
		}else if(STK_VALUE_IS_BOOLEAN(&stk)){
			result= stk.value?"true":"false";
		}else if(STK_VALUE_IS_MEMBER_PROPERTY(&stk)){
			StackMemberProperty *ma=(StackMemberProperty *)stk.value;
			result="prop@"+zs_string(ma->member_property->script_type->str_script_type)+"::"+zs_string(ma->member_property->property_name);
		}else if(STK_VALUE_IS_FUNCTION(&stk)){
			Symbol *symbol=((Symbol *)stk.value);
			ScriptType *st=symbol->scope->getScriptTypeOwner();
			if(st->idx_script_type==IDX_TYPE_CLASS_MAIN){
				result= zs_string("fun@")+symbol->name;
			}else{
				zs_string s="";

				if(symbol->properties & SYMBOL_PROPERTY_STATIC){
					s=zs_string("static_fun@");
				}else{
					s=zs_string("member_function<");
				}
				result=s+st->str_script_type+"::"+symbol->name;
			}
		}else if(STK_VALUE_IS_TYPE(&stk)){
			result= zs_string("type")+"@"+this->getScriptTypeFactory()->getScriptTypeName(stk.value);
		}else{
			if(stk.properties & STK_PROPERTY_SCRIPT_OBJECT){
				ScriptObject *so=(ScriptObject *)stk.value;
				if(so->idx_script_type==IDX_TYPE_SCRIPT_OBJECT_FUNCTION_MEMBER){
					MemberFunctionScriptObject *somf=(MemberFunctionScriptObject *)so;
					ScriptType *st=somf->getRefObject()->getScriptType();
					result= zs_string("member_function<")+st->str_script_type+"::"+somf->sf_ref->name_script_function+">";
				}else{
					result=so->toString();
				}
			}
		}

		return result;
	}

	const char		*ZetScript::stackElementToString(char *_str_out, int _str_out_len, StackElement *_stk,const zs_string & _format){
		StackElement stk=unwrapStackElement(*_stk);
		zs_string result;
		memset(_str_out,0,_str_out_len);

		// PROTECTION: do not give you big strings, instead they will retrieve from particular parts of code like JsonSerialize or Console::*)
		if(stk.properties & STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *so=(ScriptObject *)stk.value;
			if(so->idx_script_type!=IDX_TYPE_SCRIPT_OBJECT_FUNCTION_MEMBER){
				result="Object::"+zs_string(so->getTypeName());
			}
		}

		if(result.empty()){
			result=stackElementToString(_stk,_format);
		}

		strncpy(_str_out,result.c_str(),(result.length()<(_str_out_len-1))?result.length():_str_out_len-1);

		return _str_out;
	}

	void			ZetScript::stackElementAssign(StackElement *_stk_dst, const StackElement *_stk_src){
		*_stk_dst=*_stk_src;

		if(_stk_dst->properties & STK_PROPERTY_PTR_STK){
			*_stk_dst=*((StackElement *)_stk_dst->value);
		}

		// erase read only property if set
		_stk_dst->properties&=~(STK_PROPERTY_READ_ONLY);


		// update n_refs +1
		if(_stk_dst->properties&STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *so=(ScriptObject *)_stk_dst->value;
			VirtualMachine *vm=this->getVirtualMachine();
			if(so->idx_script_type == IDX_TYPE_SCRIPT_OBJECT_STRING && so->shared_pointer==NULL){
				//STK_IS_STRING_SCRIPT_OBJECT(stk_arg)){ // remove
				StringScriptObject *sc=ZS_NEW_STRING_OBJECT(this);
				if(!vm_create_shared_script_object(
						vm
						,sc
				)){
					return;
				}
				sc->set(so->toString().c_str());
				so=sc;
			}

			vm_share_script_object(vm,so);

			// assign object value into stk
			_stk_dst->value=(zs_int)so;
			_stk_dst->properties=STK_PROPERTY_SCRIPT_OBJECT;
		}
	}

	bool ZetScript::stackElementTo(StackElement * _stack_element, int _idx_builtin_type, zs_int *_ptr_var, zs_string  & _error){
		zs_int val_ret=0;

		ScriptObject *script_object=NULL;

		// save return type ...
		if(_stack_element->properties & STK_PROPERTY_PTR_STK){
			_stack_element=((StackElement *)_stack_element->value);
		}

		if(_idx_builtin_type == IDX_TYPE_STACK_ELEMENT){
			*_ptr_var=(zs_int)_stack_element;
			return true;
		}

		switch(GET_STK_PROPERTY_TYPES(_stack_element->properties)){
		case STK_PROPERTY_NULL:
			break;
		case STK_PROPERTY_BOOL:
			if(_idx_builtin_type == IDX_TYPE_BOOL_C){// *ScriptType::k_str_bool_type){
				val_ret=(zs_int)(_stack_element->value);
			}else if(_idx_builtin_type == IDX_TYPE_BOOL_PTR_C){//*ScriptType::k_str_bool_type_ptr){
				val_ret=(zs_int)(&_stack_element->value);
			}else{
				_error="cannot convert '"
					+zs_rtti::demangle(k_str_bool_type_ptr)
					+"' to '"
					+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this->getScriptTypeFactory(),_idx_builtin_type)).c_str()
					+"'";
				return false;
			}

			break;
		case STK_PROPERTY_ZS_FLOAT:
			switch(_idx_builtin_type){
			case IDX_TYPE_ZS_FLOAT_C:
				ZS_FLOAT_COPY(&val_ret,&_stack_element->value);
				break;
			case IDX_TYPE_ZS_FLOAT_PTR_C:
				val_ret=(zs_int)(&_stack_element->value);
				break;
			case IDX_TYPE_ZS_INT_C:
				{
					zs_int *aux_dst = ((zs_int *)&val_ret);
					zs_float *aux_src=(zs_float *)&_stack_element->value;
					*aux_dst=(zs_int)(*aux_src);
				}
				break;
			default:
				_error="cannot convert '"
					+zs_rtti::demangle(k_str_zs_float_type_ptr)
					+"' to '"
					+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this->getScriptTypeFactory(),_idx_builtin_type))
					+"'";
				return false;
			}
			break;
		case STK_PROPERTY_ZS_INT:
			switch(_idx_builtin_type){
			case IDX_TYPE_ZS_INT_C:
				val_ret=(zs_int)(_stack_element->value);
				break;
			case IDX_TYPE_ZS_INT_PTR_C:
				val_ret=(zs_int)(&_stack_element->value);
				break;
			default:
				_error="cannot convert 'int' to '"
				+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this->getScriptTypeFactory(),_idx_builtin_type))
				+"'";
				return false;
			}
			break;
		// it expects the ScriptFunction directly
		case STK_PROPERTY_FUNCTION:
			val_ret=((Symbol *)_stack_element->value)->ref_ptr;
			break;
		default: // script variable by default ...

			if(_stack_element->properties & (STK_PROPERTY_SCRIPT_OBJECT | STK_PROPERTY_CONTAINER_SLOT)){

				if(_stack_element->properties & STK_PROPERTY_CONTAINER_SLOT){
					script_object=((ContainerSlot *)(_stack_element->value))->getSrcContainerRef();
				}else{
					script_object=(ScriptObject *)_stack_element->value;
				}
				ScriptType *c_class=NULL;
				val_ret=(zs_int)script_object;

				if(script_object==NULL){
					_error="Variable is not defined";
					return false;
				}

				if(_idx_builtin_type!=script_object->idx_script_type){

					if(script_object->idx_script_type == IDX_TYPE_SCRIPT_OBJECT_STRING){ // string
						if(_stack_element->value == 0){ // if not created try to create a tmp scriptvar it will be removed...
							_error="internal error var_ref is NULL";
							return false;
						}

						if(_idx_builtin_type == IDX_TYPE_ZS_STRING_PTR_C){
							val_ret=(zs_int)(((StringScriptObject *)script_object)->str_ptr);
						}else if(_idx_builtin_type == IDX_TYPE_ZS_STRING_C){
							*((zs_string *)_ptr_var)=*(((StringScriptObject *)script_object)->str_ptr);
							return true;
						}else if (_idx_builtin_type == IDX_TYPE_CONST_CHAR_PTR_C){
							val_ret=(zs_int)(((StringScriptObject *)script_object)->getConstChar());
						}else{
							_error="cannot convert '"
									+zs_rtti::demangle((k_str_zs_string_type_ptr))
									+"' to '"
									+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this->getScriptTypeFactory(),_idx_builtin_type))
									+"'";
							return false;
						}
					}else if(script_object->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_CLASS){
						ClassScriptObject *script_object_class = (ClassScriptObject *)script_object;
						c_class=script_object_class->getNativeScriptClass(); // get the pointer directly ...

						if(c_class != NULL){
							if((val_ret=c_class->extendsFrom(
									_idx_builtin_type
								))==0
							){//c_class->idx_script_type==idx_builtin_type){
								_error="cannot convert '"
										+zs_rtti::demangle(c_class->str_script_type_ptr.c_str())
										+"' to '"
										+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this->getScriptTypeFactory(),_idx_builtin_type))
										+"'";
								return false;
							}
							val_ret=(zs_int)script_object_class->getNativeObject();
						}else{ // Is script class, set directly
							val_ret=_stack_element->value;
							/*error = " Error calling function, no C-object parameter! Unexpected script variable (";
							error.append(zs_rtti::demangle(script_object->getTypeName().c_str()));
							error.append(")");
							return false;*/
						}
					}else{ // cannot convert...
						_error="cannot convert '"
							+zs_rtti::demangle(script_object->getTypeName())
							+"' to '"
							+zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this->getScriptTypeFactory(),_idx_builtin_type))
							+"'";
						return false;
					}
				}else{ // get native object...
					val_ret=(zs_int)script_object->getNativeObject();
				}
			}else{
				_error=zs_strutils::format("Cannot know how to convert type '%s' from '%s'"
					,zs_rtti::demangle(GET_IDX_2_CLASS_C_STR(this->getScriptTypeFactory(),_idx_builtin_type)).c_str()
					,this->stackElementToStringTypeOf(_stack_element).c_str()
				);
				return false;
			}
			break;
		}

		*_ptr_var = val_ret;

		return true;
	}

	// Helpers...
	 StackElement ZetScript::toStackElement(zs_int ptr_var, short idx_builtin_type_var){
		//zs_int ptr_var = (zs_int)input_var;
			zs_string s_return_value;
			StackElement stk_result=k_stk_undefined;
			StringScriptObject *so=NULL;

			//int idx_builtin_type=getIdxScriptTypeFromTypeNamePtr(typeid(T).name());
			// save return type ...
			switch(idx_builtin_type_var){
			 case IDX_TYPE_VOID_C:
				break;
			 case IDX_TYPE_ZS_INT_PTR_C:
				 if(ptr_var==0) return stk_result;
				 stk_result={(*((zs_int *)ptr_var)),STK_PROPERTY_ZS_INT};
				 break;
			 case IDX_TYPE_ZS_INT_C:
				 stk_result={(((zs_int)ptr_var)),STK_PROPERTY_ZS_INT};
				 break;
			 case IDX_TYPE_ZS_FLOAT_C:
				 stk_result.properties=STK_PROPERTY_ZS_FLOAT;//{};
				 ZS_FLOAT_COPY(&stk_result.value,&ptr_var);
				 break;
			 case IDX_TYPE_ZS_FLOAT_PTR_C:
				 if(ptr_var==0) return stk_result;
				 stk_result.properties=STK_PROPERTY_ZS_FLOAT;//{};
				 ZS_FLOAT_COPY(&stk_result.value,&(*(zs_float *)ptr_var));
				 break;
			 case IDX_TYPE_BOOL_PTR_C:
				 if(ptr_var==0) return stk_result;
				 stk_result={(*((bool *)ptr_var)),STK_PROPERTY_BOOL};
				 break;
			 case IDX_TYPE_BOOL_C:
				 stk_result={(((bool)ptr_var)),STK_PROPERTY_BOOL};
				 break;
			 case IDX_TYPE_CONST_CHAR_PTR_C:
			 case IDX_TYPE_ZS_STRING_PTR_C:
			 case IDX_TYPE_ZS_STRING_C:


				 so=ZS_NEW_STRING_OBJECT(this);
				 if(ptr_var!=0) { // not null
					 if(idx_builtin_type_var==IDX_TYPE_ZS_STRING_PTR_C){
						// assing zs_string reference
						so->str_ptr=(zs_string *)ptr_var;
					 }else if(idx_builtin_type_var==IDX_TYPE_ZS_STRING_C){ // zs_string passed as pointer
						 so->set(*((zs_string *)ptr_var));
					 }else{ // const char
						 so->set((const char *)ptr_var);
					 }
				 }

				 stk_result={(intptr_t)so,STK_PROPERTY_SCRIPT_OBJECT};
				 break;
			 case IDX_TYPE_STACK_ELEMENT:
				 if(ptr_var==0) return stk_result;
				 stk_result=*((StackElement *)ptr_var);
				 break;
			 case IDX_TYPE_SCRIPT_OBJECT_ARRAY:
			 case IDX_TYPE_SCRIPT_OBJECT_ITERATOR_ARRAY:
			 case IDX_TYPE_SCRIPT_OBJECT_OBJECT:
			 case IDX_TYPE_SCRIPT_OBJECT_CLASS:
			 case IDX_TYPE_SCRIPT_OBJECT_ITERATOR_OBJECT:
			 case IDX_TYPE_SCRIPT_OBJECT_STRING:
			 case IDX_TYPE_SCRIPT_OBJECT_ITERATOR_STRING:
				 if(ptr_var==0) return stk_result;
				stk_result = {
					 (intptr_t)ptr_var
					 ,STK_PROPERTY_SCRIPT_OBJECT
				 };

				 break;
			 default:
				 //if(ptr_var==0) return stk_result;
				 if(ptr_var == 0){ // null value
					 stk_result={0,STK_PROPERTY_NULL};
				 }else{
					 stk_result = {
						 (intptr_t)this->getScriptTypeFactory()->instanceScriptObjectByTypeIdx(idx_builtin_type_var,(void *)ptr_var)
						 ,STK_PROPERTY_SCRIPT_OBJECT
					 };
				 }
				 break;
			}
			return stk_result;
	}

	 // STACKELEMENT
	 //----------------------------------------------------------------------------------------------------------------------------------------------------------------
	 // FILE MANAGEMENT
	bool ZetScript::isFilenameAlreadyParsed(const zs_string & filename){
		for(int i = 0; i < parsed_files.size(); i++){
			if(((ParsedFile *)parsed_files.items[i])->filename==filename){
				return true;
			}
		}
		return false;
	}

	StackElement ZetScript::evalInternal(const char * _code, unsigned short _eval_options, const char * _filename, EvalData *_eval_data_from, const char *__invoke_file__, int __invoke_line__)  {
		StackElement stk_ret=k_stk_undefined;

		eval_parse_and_compile(this,_code,_eval_data_from,_filename);
		link();

		if(_eval_options & EvalOption::EVAL_OPTION_PRINT_BYTE_CODE){
			printGeneratedCode(_eval_options & EvalOption::EVAL_OPTION_PRINT_ALL_BYTE_CODE);
		}

		if((_eval_options & EvalOption::EVAL_OPTION_NO_EXECUTE)==0){
			// the first code to execute is the main function that in fact is a special member function inside our main class
			stk_ret=vm_execute(
					virtual_machine
					,NULL
					,script_type_factory->getMainFunction()
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

	void ZetScript::registerConstant(const zs_string & _key, int _value, const char *registered_file, short registered_line){
		Symbol *symbol_variable=ZS_MAIN_FUNCTION(this)->registerLocalVariable(
			ZS_MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, _key
		);

		StackElement *stk=vm_get_stack_element_at(this->virtual_machine,symbol_variable->idx_position);
		stk->value=_value;
		stk->properties=STK_PROPERTY_ZS_INT|STK_PROPERTY_READ_ONLY;
	}

	void ZetScript::registerConstant(const zs_string & _key, bool _value, const char *registered_file, short registered_line){
		Symbol *symbol_variable=ZS_MAIN_FUNCTION(this)->registerLocalVariable(
			ZS_MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, _key
		);

		StackElement *stk=vm_get_stack_element_at(this->virtual_machine,symbol_variable->idx_position);

		stk->value=_value;
		stk->properties=STK_PROPERTY_BOOL|STK_PROPERTY_READ_ONLY;
	}

	void ZetScript::registerConstant(const zs_string & _key, zs_float _value, const char *registered_file, short registered_line){
		Symbol *symbol_variable=ZS_MAIN_FUNCTION(this)->registerLocalVariable(
			ZS_MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, _key
		);

		StackElement *stk=vm_get_stack_element_at(this->virtual_machine,symbol_variable->idx_position);
		ZS_FLOAT_COPY(&stk->value,&_value);
		stk->properties=STK_PROPERTY_ZS_FLOAT|STK_PROPERTY_READ_ONLY;
	}

	void ZetScript::registerConstant(const zs_string & _key, const zs_string & _value, const char *registered_file, short registered_line){
		Symbol *symbol_variable=ZS_MAIN_FUNCTION(this)->registerLocalVariable(
			ZS_MAIN_SCOPE(this)
			, registered_file
			, registered_line
			, _key
		);

		StackElement *stk=vm_get_stack_element_at(this->virtual_machine,symbol_variable->idx_position);
		*stk=*(this->registerStkConstantStringObject(_key,_value));
	}

	void ZetScript::registerConstant(const zs_string & _key, const char * _value, const char *registered_file, short registered_line){
		registerConstant(_key, zs_string(_value), registered_file, registered_line);
	}
	//
	// REGISTER CONSTANTS
	//-----------------------------------------------------------------------------------------
	//
	// STK REGISTER OBJECT
	//
	StackElement * ZetScript::registerStkConstantStringObject(const zs_string & _key,const zs_string & _value){

		StackElement *stk=NULL;
		StringScriptObject *so=NULL;

		if((stk = getStkConstantStringObject(_key))!=NULL){
			if(stk->properties & (STK_PROPERTY_SCRIPT_OBJECT | STK_PROPERTY_READ_ONLY)){
				return stk;
			}
			else{
				// throw
				ZS_THROW_RUNTIME_ERROR(
					"Cannot register constant '%s' as 'StringScriptObject', because is already registered as '%s'"
					,_key.c_str()
					,this->stackElementToStringTypeOf(stk).c_str()
				);
			}
		}

		stk=new StackElement;

		stk_constants->set(_key.c_str(),(zs_int)stk);

		so=ZS_NEW_STRING_OBJECT(this);

		// swap values stk_ref/value
		so->set(_value.c_str());
		stk->value=(zs_int)so;
		stk->properties=STK_PROPERTY_SCRIPT_OBJECT | STK_PROPERTY_READ_ONLY;

		return stk;
	}

	StackElement *ZetScript::getStkConstantStringObject(const zs_string & _key){
		return (StackElement *)stk_constants->get(_key.c_str());
	}

	//
	// STK REGISTER OBJECT
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------


	ObjectScriptObject * ZetScript::newObjectScriptObject(){
		return ObjectScriptObject::newObjectScriptObject(this);
	}

	StringScriptObject * ZetScript::newStringScriptObject(){
		return StringScriptObject::newStringScriptObject(this);
	}

	ArrayScriptObject * ZetScript::newArrayScriptObject(){
		return ArrayScriptObject::newArrayScriptObject(this);
	}

	StackElement ZetScript::eval(const zs_string & _expresion, unsigned short _options, const char * _script_filename_by_ref, const char *__invoke_file__, int __invoke_line__)  {

		const char *script_filename_by_ref=getFilenameByRef(_script_filename_by_ref);

		return evalInternal(_expresion.c_str(), _options, script_filename_by_ref,NULL,__invoke_file__,__invoke_line__);
	}

	StackElement	ZetScript::eval(const zs_string & _expresion, const char *__invoke_file__, int __invoke_line__){
		return evalInternal(_expresion.c_str(), 0, NULL,NULL,__invoke_file__,__invoke_line__);
	}

	StackElement ZetScript::evalFile(const zs_string &  _filename, unsigned short _eval_options, EvalData *_eval_data_from, const char *__invoke_file__, int __invoke_line__){
		//int idx_file=-1;
		StackElement stk_ret;
		zs_buffer *buf_tmp=NULL;

		stk_ret.setUndefined();

		if(!isFilenameAlreadyParsed(_filename)){
			ParsedFile *ps=new ParsedFile();
			zs_string current_directory="";
			ps->filename = _filename;
			parsed_files.push_back(ps);
			const char * const_file_char=ps->filename.c_str();
			//idx_file=parsed_files.size()-1;

			buf_tmp=zs_file::read_text(_filename);

			// file exist and can read ... set current pwd
			current_directory = zs_dir::get_current_directory();
			zs_dir::change_dir(zs_path::get_directory(_filename));

			zs_string str_error="";
			zs_string error_file="";

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
			}catch(zs_exception & e){
				error=true;
				error_file=e.getFilename();
				error_line=e.getLine();
				str_error=e.getDescription();
			}catch(std::exception & e){
				error=true;
				str_error=e.what();
			}

			// restore previous directory
			zs_dir::change_dir(current_directory);

			// deallocate before throw errors...
			delete buf_tmp;

			if(error){
				if(error_file.empty()==false){
					ZS_THROW_SCRIPT_ERROR_FILE_LINEF(error_file.c_str(),error_line,str_error.c_str());
				}else{
					ZS_THROW_EXCEPTION(str_error.c_str());
				}
			}


		}

		return stk_ret;
	}

	void ZetScript::clearGlobalVariables(int _idx_start_variable, int _idx_start_function){
		ZS_UNUSUED_PARAM(_idx_start_function);
		zs_string global_symbol;
		int idx_start_variable = _idx_start_variable == ZS_IDX_UNDEFINED ?  idx_current_global_variable_checkpoint:_idx_start_variable;
		ScriptFunction *main_function_object=script_type_factory->getMainFunction();
		Scope *main_scope=ZS_MAIN_SCOPE(this);
		zs_vector<Symbol *> *local_variables=main_function_object->local_variables;
		zs_vector<Symbol *> *global_symbol_variables= main_scope->symbol_variables;
		int n_global_symbol_found=0;
		int v=local_variables->size()-1;


		// Because all symbols are ordered by scope, have G as global symbols and L local symbols the disposition is the following,
		//
		// [G][G][G][G]..[G][L][L][L][L][L]..[L]
		//
		// So we only have to delete symbols
		vm_deref_cyclic_references(this->virtual_machine);

		// remove all shared 0 pointers
		if(local_variables->size() > 0){

			// remove cyclic container instances
			for (
				;v>=idx_start_variable;
			) {

				StackElement *vm_stk_element=&vm_get_stack_elements(virtual_machine)[v];

				Symbol *symbol=(Symbol *)local_variables->items[v];


				ScriptObject *script_object = NULL;

				if(symbol != NULL && symbol->scope == main_scope){ // if variable in global scope

					if(
							(vm_stk_element->properties & STK_PROPERTY_SCRIPT_OBJECT)
											&&
							(vm_stk_element->properties & STK_PROPERTY_READ_ONLY)==0  // if not constant
					){
						script_object =((ScriptObject *)(vm_stk_element->value));
						if(script_object!=NULL){
							if(script_object->shared_pointer != NULL){
								if(!vm_unref_shared_script_object(this->virtual_machine,script_object,NULL)){
									ZS_THROW_RUNTIME_ERROR("error clearing variables: %s",vm_get_error(this->virtual_machine).c_str());
								}
							}
						}
					}
					n_global_symbol_found++;

					// erase unused global symbol
					delete symbol;
				}

				STK_SET_UNDEFINED(vm_stk_element);

				--v;
			}

			// clear all garbage
			StackElement *vm_stack=vm_get_stack_elements(virtual_machine);
			memset(vm_stack+idx_start_variable,0,sizeof(StackElement)*(VM_STACK_MAX-idx_start_variable));

			// erase global elements that they weren't saved...
			int resize=local_variables->size()-(local_variables->size()-idx_start_variable);
			local_variables->resize(resize);
			global_symbol_variables->resize(global_symbol_variables->size()-n_global_symbol_found);
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
		for(int i=0;i<parsed_files.size();i++){
			delete ((ParsedFile *)parsed_files.items[i]);
		}
		parsed_files.clear();
	}

	void ZetScript::clear(){

		// clearGlobalVariables
		clearGlobalVariables();

		// clearGlobalFunctions
		Scope *main_scope=ZS_MAIN_SCOPE(this);
		zs_vector<Symbol *> *global_symbol_functions= main_scope->symbol_functions;
		int v=global_symbol_functions->size()-1;
		// remove all shared 0 pointers
		if(v >= idx_current_global_function_checkpoint){
			for (
				;v>=idx_current_global_function_checkpoint;
			) {


				Symbol *symbol=(Symbol *)global_symbol_functions->items[v];//(Symbol *)main_function_object->registered_symbols->items[v];
				delete symbol;
				--v;
			}

			int resize=global_symbol_functions->size()-(global_symbol_functions->size()-idx_current_global_function_checkpoint);
			global_symbol_functions->resize(resize);
		}


		// clearScriptTypes
		zs_vector<Symbol *> *script_types= main_scope->symbol_types;
		v=script_types->size()-1;
		// remove all shared 0 pointers
		if(v >=idx_current_script_types_checkpoint){
			for (
				;v>=idx_current_script_types_checkpoint;
			) {


				Symbol *symbol=(Symbol *)script_types->items[v];//(Symbol *)main_function_object->registered_symbols->items[v];
				delete symbol;
				--v;
			}

			int resize=script_types->size()-(script_types->size()-idx_current_script_types_checkpoint);
			script_types->resize(resize);
		}

		functions_with_unresolved_symbols.clear();

		scope_factory->clear();
		script_function_factory->clear();
		script_type_factory->clear();

		resetParsedFiles();


	}

	void ZetScript::saveState(){
		ScriptFunction *main_function_object=script_type_factory->getMainFunction();
		Scope *main_scope=ZS_MAIN_SCOPE(this);
		idx_current_global_variable_checkpoint=main_function_object->local_variables->size();
		idx_current_global_function_checkpoint=main_scope->symbol_functions->size();
		idx_current_script_types_checkpoint=main_scope->symbol_types->size();

		scope_factory->saveState();
		script_function_factory->saveState();
		script_type_factory->saveState();
	}

	bool ZetScript::getFunctionWithUnresolvedSymbolExists(ScriptFunction *_sf){
		for(int i=0;i < functions_with_unresolved_symbols.size(); i++){
			if(functions_with_unresolved_symbols.items[i]==_sf){
				return true;
			}
		}

		return false;

	}

	void ZetScript::addUnresolvedSymbol(ScriptFunction *_sf, zs_int idx_instruction){
		if(getFunctionWithUnresolvedSymbolExists(_sf) ==false){
			functions_with_unresolved_symbols.push_back(_sf);
		}
		_sf->addUnresolvedSymbol(idx_instruction);
	}

	void ZetScript::link(){
		int i=0;
		while(i<functions_with_unresolved_symbols.size()){
			ScriptFunction *_sf=(ScriptFunction *)functions_with_unresolved_symbols.items[i];
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

	void ZetScript::makeScriptObjectShared(ScriptObject *so){
		if(so->shared_pointer != NULL){
			// share this variable++
			vm_create_shared_script_object(virtual_machine,so);
			vm_share_script_object(virtual_machine,so);
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
		delete script_type_factory;
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
				if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){
					delete (StringScriptObject *)stk->value;
				}
				delete stk;
			}
			stk_constants->clear();
			delete stk_constants;
		}

	}
}
