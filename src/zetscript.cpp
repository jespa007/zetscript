#include "zetscript.h"


#include "ByteCode.cpp"
#include "Instruction.cpp"
#include "StackElement.cpp"
#include "Symbol.cpp"
#include "zs_exception.cpp"

namespace zetscript{

	const char * k_str_void_type =typeid(void).name();
	const char * k_str_zs_int_type_ptr=typeid(zs_int *).name();
	const char * k_str_const_zs_int_type_ptr=typeid(const zs_int *).name();
	const char * k_str_float_type_ptr=typeid(zs_float *).name();
	const char * k_str_const_float_type_ptr=typeid(const zs_float *).name();
	const char * k_str_string_type_ptr=typeid(zs_string *).name();
	const char * k_str_char_type_ptr=typeid(char *).name();
	const char * k_str_const_char_type_ptr=typeid(const char *).name();
	const char * k_str_bool_type_ptr=typeid(bool *).name();
	const char * k_str_const_bool_type_ptr=typeid(const bool *).name();
	const char * k_str_zs_int_type=typeid(zs_int).name();
	const char * k_str_float_type=typeid(zs_float).name();
	const char * k_str_bool_type=typeid(bool).name();
	const char * k_str_stack_element_type=typeid(StackElement).name();

	ZetScript::ZetScript(){
		eval_int=0;
		eval_float=0;
		eval_string="";
		eval_bool = false;
		idx_current_global_variable_checkpoint=0;
		idx_current_global_function_checkpoint=0;
		stk_constants=NULL;

		eval_init();
		scope_factory = new ScopeFactory(this);
		script_function_factory= new ScriptFunctionFactory(this);

		virtual_machine = vm_new(this);
		script_class_factory = new ScriptClassFactory(this);
		script_class_factory->init();
		vm_init(virtual_machine,this);

		script_class_factory->registerSystem();

		stk_constants=new zs_map();

		//-------------------------
		// Register built in modules

		// Math mod
		ScriptClass *cl=script_class_factory->registerClass("Math");
		cl->registerNativeMemberVariableStaticConst("PI",&MathModule_PI);
		cl->registerNativeMemberFunctionStatic("sin",MathModule_sin);
		cl->registerNativeMemberFunctionStatic("cos",MathModule_cos);
		cl->registerNativeMemberFunctionStatic("abs",MathModule_abs);
		cl->registerNativeMemberFunctionStatic("pow",MathModule_pow);
		cl->registerNativeMemberFunctionStatic("degToRad",MathModule_degToRad);
		cl->registerNativeMemberFunctionStatic("random",MathModule_random);

		// Console mod
		cl=script_class_factory->registerClass("Console");
		cl->registerNativeMemberFunctionStatic("readChar",ConsoleModule_readChar);
		cl->registerNativeMemberFunctionStatic("outNative",ConsoleModule_out);
		cl->registerNativeMemberFunctionStatic("outlnNative",ConsoleModule_outln);
		cl->registerNativeMemberFunctionStatic("errorNative",ConsoleModule_error);
		cl->registerNativeMemberFunctionStatic("errorlnNative",ConsoleModule_errorln);

		// System mod
		cl=script_class_factory->registerClass("System");
		cl->registerNativeMemberFunctionStatic("clock",SystemModule_clock);
		cl->registerNativeMemberFunctionStatic("evalNative",SystemModule_eval);
		//cl->registerNativeMemberFunctionStatic("assertNative",SystemModule_assert);
		cl->registerNativeMemberFunctionStatic("errorNative",SystemModule_error);

		// Json mod
		cl=script_class_factory->registerClass("Json");
		cl->registerNativeMemberFunctionStatic("serializeNative",static_cast<ScriptObjectString * (*)(ZetScript *zs,StackElement *)>(JsonModule_serialize));
		cl->registerNativeMemberFunctionStatic("serializeNative",static_cast<ScriptObjectString * (*)(ZetScript *zs,StackElement *, bool *)>(JsonModule_serialize));
		cl->registerNativeMemberFunctionStatic("deserialize",JsonModule_deserialize);
		//---------------------------------------------
		// DateTime
		cl=registerClass<zs_datetime>("DateTime",DateTimeModule_new,DateTimeModule_delete);

		registerMemberFunctionStatic<zs_datetime>("_add",DateTimeModule_add);
		registerMemberFunctionStatic<zs_datetime>("_sub",DateTimeModule_sub);

		registerMemberFunction<zs_datetime>("setUtc",DateTimeModule_setUtc);

		registerMemberFunction<zs_datetime>("addSeconds",DateTimeModule_addSeconds);
		registerMemberFunction<zs_datetime>("addMinutes",DateTimeModule_addMinutes);
		registerMemberFunction<zs_datetime>("addHours",DateTimeModule_addHours);
		registerMemberFunction<zs_datetime>("addDays",DateTimeModule_addDays);
		registerMemberFunction<zs_datetime>("addMonths",DateTimeModule_addMonths);
		registerMemberFunction<zs_datetime>("addYears",DateTimeModule_addYears);

		// metamethods...
		registerMemberFunction<zs_datetime>("_toString",DateTimeModule_toString);

		registerMemberAttributeGetter<zs_datetime>("week_day",DateTimeModule_get_week_day);
		registerMemberAttributeGetter<zs_datetime>("month_day",DateTimeModule_get_month_day);
		registerMemberAttributeGetter<zs_datetime>("year_day",DateTimeModule_get_year_day);

		registerMemberAttributeGetter<zs_datetime>("second",DateTimeModule_get_second);
		registerMemberAttributeGetter<zs_datetime>("minute",DateTimeModule_get_minute);
		registerMemberAttributeGetter<zs_datetime>("hour",DateTimeModule_get_hour);

		registerMemberAttributeGetter<zs_datetime>("day",DateTimeModule_get_day);
		registerMemberAttributeGetter<zs_datetime>("month",DateTimeModule_get_month);
		registerMemberAttributeGetter<zs_datetime>("year",DateTimeModule_get_year);

		// Custom user function or classes
		eval(
			zs_strutils::format(
				//------------------------------------------------
				// String
				"static String::format(s,...args){" // add static function format to String module
				"	String::formatNative(System::getZetScript(),s,args)"
				"}"
				//------------------------------------------------
				// DateTime
				"static DateTime::now(zs){" // add static function now
				"	return new DateTime()"
				"}"

				"static DateTime::nowUtc(zs){" // add static function nowUtc
				"   var t=new DateTime();"
				"   t.setUtc()"
				"   return t;"
				"}"
				//------------------------------------------------
				// Console
				"static Console::out(s,...args){"
				"	Console::outNative(System::getZetScript(),s,args)"
				"}"
				"static Console::outln(s,...args){"
				"	Console::outlnNative(System::getZetScript(),s,args)"
				"}"
				"static Console::error(s,...args){"
				"	Console::errorNative(System::getZetScript(),s,args)"
				"}"
				"static Console::errorln(s,...args){"
				"	Console::errorlnNative(System::getZetScript(),s,args)"
				"}"
				//------------------------------------------------
				// System
				"static System::assert(check,s,...args){"
				"	if((typeof check)==bool){"
				"		System::errorNative(System::getZetScript(),\"Error, executing  'System::assert' first parameter must be boolean but it was {0}\",[typeof check])"
				"		return;"
				"	}"
				"	if(check==false){"
				"		System::errorNative(System::getZetScript(),\"Assert error: \"+s,args)"
				"	}"
				""
				"}"
				"static System::error(s,...args){"
				"	System::errorNative(System::getZetScript(),s,args)"
				"}"
				"static System::eval(s,params){"
				"	return System::evalNative(System::getZetScript(),s,params)"
				"}"
				"static System::getZetScript(){"
				"	return ptrToZetScriptPtr(0x%x);" // ptrToZetScript it gets current this
				"}"
				//------------------------------------------------
				// Json
				"static Json::serialize(stk,formatted=false){"
				"	return Json::serializeNative(System::getZetScript(),stk,formatted)"
				"}"
				//------------------------------------------------
				// DateTime
				"static DateTime::now(){"
				"	return new DateTime();"
				"}"
				"static DateTime::nowUtc(){"
				"	var dt=new DateTime();"
				"   dt.setUtc();"
				"   return dt; "
				"}"


			,
			(void *)this
			)
		);

		saveState();
	}
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	 // PRINT INFO

	void ZetScript::printAllStructSizes(){
		printf("ZetScript:%lu\n"
				"VirtualMachineData:%lu\n"
				"ScriptClass:%lu\n"
				"ScriptFunction:%lu\n"
				"StackElement:%lu\n"
				"Symbol:%lu\n"
				"Scope:%lu\n"
				"ScriptObject:%lu\n"
				"ScriptObjectString:%lu\n"
				"ScriptObjectVector:%lu\n"
				"ScriptObjectObject:%lu\n"
				"ScriptObjectClass:%lu\n"
				,sizeof(ZetScript)
				,sizeof(VirtualMachineData)
				,sizeof(ScriptClass)
				,sizeof(ScriptFunction)
				,sizeof(StackElement)
				,sizeof(Symbol)
				,sizeof(Scope)
				,sizeof(ScriptObject)
				,sizeof(ScriptObjectString)
				,sizeof(ScriptObjectVector)
				,sizeof(ScriptObjectObject)
				,sizeof(ScriptObjectClass)
			);
	}


	 void ZetScript::printGeneratedCode(bool show_system_code){

		 zs_vector *script_classes=script_class_factory->getScriptClasses();
		 // for all classes print code...
		 ScriptFunction *sf_main=MAIN_FUNCTION(this);

		 // list functions
		 zs_vector *symbol_functions=sf_main->symbol.scope->symbol_functions;

		 // print main function
		 ScriptFunction::printGeneratedCode(sf_main);

		 // print defined functions in main function
		 for(unsigned j =0; j < symbol_functions->count; j++){
			Symbol *symbol=(Symbol *)symbol_functions->items[j];

			if(symbol->properties & SYMBOL_PROPERTY_FUNCTION){

				ScriptFunction *local_sf = (ScriptFunction *)symbol->ref_ptr;

				if(( local_sf->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF) != SYMBOL_PROPERTY_C_OBJECT_REF){
					ScriptFunction::printGeneratedCode(local_sf);
				}
			}
		}

		 for(unsigned i = 1; i < script_classes->count; i++){
			 ScriptClass *sc=(ScriptClass *)script_classes->get(i);
			 bool show_class=true;

			 // ignore builtin implementations if not chosen ...
			 if(show_system_code == false && (
					 	sc->class_name == "System"
					||	sc->class_name == "String"
					||	sc->class_name == "StringIterator"
					||	sc->class_name == "Object"
					||	sc->class_name == "ObjectIterator"
					||	sc->class_name == "Console"
					||	sc->class_name == "DateTime"
					||	sc->class_name == "Vector"
					||	sc->class_name == "VectorIterator"
					||	sc->class_name == "Json"
				)){
				 show_class=false;
			 }

			 if(show_class){
				 symbol_functions=sc->class_scope->symbol_functions;
				 for(unsigned f = 0; f < symbol_functions->count; f++){
					 Symbol *symbol=(Symbol *)symbol_functions->items[f];
					 ScriptFunction::printGeneratedCode((ScriptFunction *)symbol->ref_ptr,sc);

				 }
			 }
		 }
	 }
	 // PRINT ASM INFO
	 //----------------------------------------------------------------------------------------------------------------------------------------------------------------
	 // FILE MANAGEMENT
	bool ZetScript::isFilenameAlreadyParsed(const zs_string & filename){
		for(unsigned i = 0; i < parsed_files.count; i++){
			if(((ParsedFile *)parsed_files.items[i])->filename==filename){
				return true;
			}
		}
		return false;
	}
	//-----------------------------------------------------------------------------------------
	// STK STRING OBJECT
	StackElement * ZetScript::registerStkStringObject(const zs_string & key_name,const zs_string & const_name){

		StackElement *stk=NULL;
		ScriptObjectString *so=NULL;

		if((stk = getStkStringObject(key_name))!=NULL){
			if(stk->properties & (STK_PROPERTY_SCRIPT_OBJECT | STK_PROPERTY_READ_ONLY)){
				return stk;
			}
			else{
				// throw
				THROW_RUNTIME_ERROR(
						"Cannot register constant '%s' as 'ScriptObjectString', because is already registered as '%s'"
						,key_name.c_str(),stk_to_typeof_str(this,stk)
						);
			}
		}

		stk=new StackElement;

		stk_constants->set(key_name.c_str(),(zs_int)stk);

		so=ZS_NEW_OBJECT_STRING(this);
		// swap values stk_ref/value
		so->set(const_name);

		stk->value=(zs_int)so;
		stk->properties=STK_PROPERTY_SCRIPT_OBJECT | STK_PROPERTY_READ_ONLY;

		return stk;

	}

	StackElement *ZetScript::getStkStringObject(const zs_string & key_name){
		return (StackElement *)stk_constants->get(key_name.c_str());
	}


	/*void registerConstantVariable(const zs_string & var_name, zs_int value, const char *registered_file, short registered_line){

	}

	void registerConstantVariable(const zs_string & var_name, zs_float value, const char *registered_file, short registered_line){

	}

	void registerConstantVariable(const zs_string & var_name, bool value, const char *registered_file, short registered_line){

	}

	void registerConstantVariable(const zs_string & var_name, const zs_string v, const char *registered_file, short registered_line){

	}*/


	//
	// STK STRING OBJECT
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------
	StackElement ZetScript::evalInternal(const char * code, unsigned short options, const char * filename, const char *__invoke_file__, int __invoke_line__)  {
		ScriptFunction *sf_main=MAIN_FUNCTION(this);
		Scope *sc_main=MAIN_SCOPE(this);
		StackElement stk_ret=k_stk_null;

		eval_parse_and_compile(this,code,filename);

		if(options & EvalOption::EVAL_OPTION_SHOW_USER_BYTE_CODE){
			printGeneratedCode(options & EvalOption::EVAL_OPTION_SHOW_SYSTEM_BYTE_CODE);
		}

		if((options & EvalOption::EVAL_OPTION_NO_EXECUTE)==0){
			// the first code to execute is the main function that in fact is a special member function inside our main class
			stk_ret=vm_execute(
					virtual_machine
					,NULL
					,script_class_factory->getMainFunction()
					,NULL
					,0
					,0
					, __invoke_file__
					,__invoke_line__
			);
		}

		return stk_ret;
	}

	StackElement ZetScript::eval(const zs_string & code, unsigned short options, const char * filename, const char *__invoke_file__, int __invoke_line__)  {

		return evalInternal(code.c_str(), options, filename,__invoke_file__,__invoke_line__);
	}

	StackElement ZetScript::evalFile(const zs_string &  filename, unsigned short eval_options, const char *__invoke_file__, int __invoke_line__){
		//int idx_file=-1;
		StackElement stk_ret;
		char *buf_tmp=NULL;

		stk_ret.setUndefined();

		if(!isFilenameAlreadyParsed(filename)){
			ParsedFile *ps=new ParsedFile();
			zs_string current_directory="";
			ps->filename = filename;
			parsed_files.push_back((zs_int)ps);
			const char * const_file_char=ps->filename.c_str();
			//idx_file=parsed_files.size()-1;
			size_t n_bytes;

			if((buf_tmp=zs_file::read(filename, n_bytes))!=NULL){
				// file exist and can read ... set current pwd
				//if(eval_options & EVAL_OPTION_CHDIR_SCRIPT_DIRECTORY){
				current_directory = zs_dir::get_current_directory();
				zs_dir::change_dir(zs_path::get_directory(filename));
				//}
				zs_string error_str="";
				zs_string error_file="";


				bool error=false;


				int error_line=-1;
				try{
					stk_ret=evalInternal(buf_tmp,eval_options,const_file_char,__invoke_file__,__invoke_line__);
				}catch(zs_exception & e){
					error=true;
					error_file=e.getErrorSourceFilename();
					error_line=e.getErrorLine();
					error_str=e.getErrorDescription();
				}catch(std::exception & e){
					error=true;
					error_str=e.what();
				}

				// restore previous directory
				//if(eval_options & EVAL_OPTION_CHDIR_SCRIPT_DIRECTORY){
				zs_dir::change_dir(current_directory);
				//}

				// deallocate before throw errors...
				free(buf_tmp);
				buf_tmp=NULL;

				if(error){
					if(error_file.empty()==false){
						THROW_SCRIPT_ERROR_FILE_LINE(error_file.c_str(),error_line,error_str.c_str());
					}else{
						THROW_EXCEPTION(error_str.c_str());
					}
				}
			}

		}/*else{
			// already parsed
			THROW_RUNTIME_ERROR("Filename \"%s\" already parsed",filename);
		}*/

		if(buf_tmp!=NULL){
			free(buf_tmp);
		}

		return stk_ret;
	}

	void ZetScript::getScriptObject(const zs_string &function_access,ScriptObjectClass **calling_obj,ScriptFunction **fun_obj ){

	}

	void ZetScript::clearGlobalVariables(int _idx_start_variable, int _idx_start_function){
		zs_string global_symbol;
		int idx_start_variable = _idx_start_variable == ZS_IDX_UNDEFINED ?  idx_current_global_variable_checkpoint:_idx_start_variable;
		//int idx_start_function = _idx_start_function == ZS_IDX_UNDEFINED ?  idx_current_global_function_checkpoint:_idx_start_function;
		ScriptFunction *main_function_object=script_class_factory->getMainFunction();
		Scope *main_scope=MAIN_SCOPE(this);
		zs_vector *local_variables=main_function_object->local_variables;

		/*int idx_stk_start_function_element=symbol_functions->count-1;
		// unregister all global functions
		for (
				int v = idx_stk_start_function_element;
				v > idx_start_function;
				v--) {

			//Symbol *symbol=(Symbol *)main_function_object->symbol_registered_functions->pop_back();//(Symbol *)main_function_object->registered_symbols->items[v];

			if(symbol->scope == main_scope){
				if(main_scope->unregisterSymbol(symbol) == false){
					THROW_RUNTIME_ERROR("error clearing variables: global symbol '%s' not exist",symbol->name.c_str());
				}
			}

		}*/
		//int n_global_symbols_cleared=0;
		//VirtualMachine *vm=this->virtual_machine;
		//int total_main_scope_symbols_before_clear=main_scope->registered_symbols->count;

		// clear all stack element to null
		//&vm_get_stack_elements(virtual_machine)[main_function_object->registered_symbols->count-1];

		// remove all shared 0 pointers
		if(local_variables->count > 0){
			int idx_stk_start_element=local_variables->count-1;
			//StackElement *vm_stack=&vm_get_stack_elements(virtual_machine);
			// set global top stack element
			StackElement *vm_stk_element=&vm_get_stack_elements(virtual_machine)[local_variables->count-1];
			for (
					int v = idx_stk_start_element;
					v >= idx_start_variable;
					v--,vm_stk_element--) {

				Symbol *symbol=(Symbol *)local_variables->items[idx_stk_start_element];//(Symbol *)main_function_object->registered_symbols->items[v];

				ScriptObjectObject *var = NULL;

				if(symbol->scope == main_scope){ // if variable in global scope
					if(vm_stk_element->properties & STK_PROPERTY_SCRIPT_OBJECT){
						var =((ScriptObjectObject *)(vm_stk_element->value));
						if(var){
							if(var->shared_pointer != NULL){
								if(!vm_unref_shared_script_object(this->virtual_machine,var,IDX_CALL_STACK_MAIN)){
									THROW_RUNTIME_ERROR("error clearing variables: %s",vm_get_error(this->virtual_machine).c_str());
								}
							}
						}
					}

					if(main_scope->unregisterSymbol(symbol) == false){
						THROW_RUNTIME_ERROR("error clearing variables: global symbol '%s' not exist",symbol->name.c_str());
					}

				}

				//;
				*vm_stk_element=k_stk_null;
			}

			// reset stk_vm_current ...
			//VirtualMachineData *data=(VirtualMachineData *)virtual_machine->data;
			//data->stk_vm_current=vm_stk_element;
			// clear all garbage
			StackElement *vm_stack=vm_get_stack_elements(virtual_machine);
			memset(vm_stack+idx_stk_start_element,0,sizeof(StackElement)*(VM_STACK_LOCAL_VAR_MAX-idx_stk_start_element));

		}


		/*if(n_global_symbols_cleared != total_main_scope_symbols_before_clear){
			THROW_RUNTIME_ERROR("Internal: main_scope and main_functions should have same symbols (fun:%i scope:%i)",n_global_symbols_cleared,total_main_scope_symbols_before_clear);
		}*/

		vm_remove_empty_shared_pointers(virtual_machine,IDX_CALL_STACK_MAIN);
	}

	/*void ZetScript::setClearGlobalVariablesCheckpoint(){
		idx_current_global_variable_checkpoint=this->script_class_factory->getMainFunction()->registered_symbols->count-1;
	}*/

	void ZetScript::resetParsedFiles(){
		for(unsigned i=0;i<parsed_files.count;i++){
			delete ((ParsedFile *)parsed_files.items[i]);
		}
		parsed_files.clear();
	}

	void ZetScript::clear(){

		clearGlobalVariables();

		scope_factory->clear();
		script_function_factory->clear();
		script_class_factory->clear();

		resetParsedFiles();
	}

	void ZetScript::saveState(){
		ScriptFunction *main_function_object=script_class_factory->getMainFunction();
		idx_current_global_variable_checkpoint=main_function_object->local_variables->count-1;
		//idx_current_global_function_checkpoint=main_function_object->symbol_functions->count-1;
		scope_factory->saveState();
		script_function_factory->saveState();
		script_class_factory->saveState();
	}

	ZetScript::~ZetScript(){
		// delete system and string...
		clearGlobalVariables(0,0);

		// clear objects...
		vm_delete(virtual_machine);
		
		delete script_function_factory;
		delete script_class_factory;
		delete scope_factory;

		virtual_machine=NULL;

		if(stk_constants != NULL){

			for(auto it=stk_constants->begin();!it.end();it.next()){//std::map<zs_string,StackElement *>::iterator it=stk_constants->begin();it!=stk_constants->end();it++){
				StackElement *stk=(StackElement *)it.value;
				if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){
					delete (ScriptObjectString *)stk->value;
				}
				delete stk;
			}
			stk_constants->clear();
			delete stk_constants;
		}

		eval_deinit();

		resetParsedFiles();
	}
}
