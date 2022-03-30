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
		stk_constants=NULL;
		script_filenames_by_ref=NULL;

		eval_init();
		scope_factory = new ScopeFactory(this);
		scope_factory->init();
		script_function_factory= new ScriptFunctionFactory(this);

		virtual_machine = vm_new(this);
		script_type_factory = new ScriptTypeFactory(this);
		script_type_factory->init();
		vm_init(virtual_machine,this);

		script_type_factory->registerSystem();

		stk_constants=new zs_map();
		script_filenames_by_ref=new zs_map();
		//-------------------------
		// Register built in modules

		// Math mod
		ScriptType *cl=script_type_factory->registerScriptType("Math","",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);

		cl->bindMemberVariableStaticConst("PI",&MathModule_PI);
		cl->bindMemberFunctionStatic("sin",MathModule_sin);
		cl->bindMemberFunctionStatic("cos",MathModule_cos);
		cl->bindMemberFunctionStatic("abs",MathModule_abs);
		cl->bindMemberFunctionStatic("pow",MathModule_pow);
		cl->bindMemberFunctionStatic("degToRad",MathModule_degToRad);
		cl->bindMemberFunctionStatic("random",MathModule_random);

		// Console mod
		cl=script_type_factory->registerScriptType("Console","",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);
		cl->bindMemberFunctionStatic("readChar",ConsoleModule_readChar);
		cl->bindMemberFunctionStatic("outNative",ConsoleModule_out);
		cl->bindMemberFunctionStatic("outlnNative",ConsoleModule_outln);
		cl->bindMemberFunctionStatic("errorNative",ConsoleModule_error);
		cl->bindMemberFunctionStatic("errorlnNative",ConsoleModule_errorln);

		// System mod
		cl=script_type_factory->registerScriptType("System","",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);
		cl->bindMemberFunctionStatic("clock",SystemModule_clock);
		cl->bindMemberFunctionStatic("evalNative",SystemModule_eval);
		//cl->bindMemberFunctionStatic("assertNative",SystemModule_assert);
		cl->bindMemberFunctionStatic("errorNative",SystemModule_error);

		// Json mod
		cl=script_type_factory->registerScriptType("Json","",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);
		cl->bindMemberFunctionStatic("serializeNative",static_cast<ScriptObjectString * (*)(ZetScript *zs,StackElement *)>(JsonModule_serialize));
		cl->bindMemberFunctionStatic("serializeNative",static_cast<ScriptObjectString * (*)(ZetScript *zs,StackElement *, bool *)>(JsonModule_serialize));
		cl->bindMemberFunctionStatic("deserialize",JsonModule_deserialize);
		//---------------------------------------------
		// DateTime
		cl=bindType<zs_datetime>("DateTime",DateTimeModule_new,DateTimeModule_delete);

		/*registerMemberFunctionStatic<zs_datetime>("_add",DateTimeModule_add);
		registerMemberFunctionStatic<zs_datetime>("_sub",DateTimeModule_sub);*/

		bindMemberFunction<zs_datetime>("setUtc",DateTimeModule_setUtc);

		bindMemberFunction<zs_datetime>("addSeconds",DateTimeModule_addSeconds);
		bindMemberFunction<zs_datetime>("addMinutes",DateTimeModule_addMinutes);
		bindMemberFunction<zs_datetime>("addHours",DateTimeModule_addHours);
		bindMemberFunction<zs_datetime>("addDays",DateTimeModule_addDays);
		bindMemberFunction<zs_datetime>("addMonths",DateTimeModule_addMonths);
		bindMemberFunction<zs_datetime>("addYears",DateTimeModule_addYears);

		// metamethods...
		bindMemberFunction<zs_datetime>("_toString",DateTimeModule_toString);

		bindMemberPropertyGetter<zs_datetime>("week_day",DateTimeModule_get_week_day);
		bindMemberPropertyGetter<zs_datetime>("month_day",DateTimeModule_get_month_day);
		bindMemberPropertyGetter<zs_datetime>("year_day",DateTimeModule_get_year_day);

		bindMemberPropertyGetter<zs_datetime>("second",DateTimeModule_get_second);
		bindMemberPropertyGetter<zs_datetime>("minute",DateTimeModule_get_minute);
		bindMemberPropertyGetter<zs_datetime>("hour",DateTimeModule_get_hour);

		bindMemberPropertyGetter<zs_datetime>("day",DateTimeModule_get_day);
		bindMemberPropertyGetter<zs_datetime>("month",DateTimeModule_get_month);
		bindMemberPropertyGetter<zs_datetime>("year",DateTimeModule_get_year);

		// Custom user function or classes
		eval(
			zs_strutils::format(
				//------------------------------------------------
				// String
				"static String::format(s,...args){" // add static function format to String module
				"	String::formatNative(s,args)"
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
				"static Console::out(s,...args){"
				"	Console::outNative(s,args)"
				"}"
				"static Console::outln(s,...args){"
				"	Console::outlnNative(s,args)"
				"}"
				"static Console::error(s,...args){"
				"	Console::errorNative(s,args)"
				"}"
				"static Console::errorln(s,...args){"
				"	Console::errorlnNative(s,args)"
				"}"
				//------------------------------------------------
				// System
				"static System::assert(check,s,...args){"
				"	if((typeof check)!=bool){"
				"		System::errorNative(\"Error, executing  'System::assert' first parameter must be boolean but it was {0}\",[typeof check])"
				"	}"
				"	if(check==false){"
				"		System::errorNative(\"Assert error: \"+s,args)"
				"	}"
				"}"
				"static System::error(s,...args){"
				"	System::errorNative(s,args)"
				"}"
				"static System::eval(s,params){"
				"	return System::evalNative(s,params)"
				"}"
				"static System::getZetScript(){"
				"	return ptrToZetScriptPtr(%lu);" // ptrToZetScript it gets current this
				"}"
				//------------------------------------------------
				// Json
				"static Json::serialize(stk,formatted=false){"
				"	return Json::serializeNative(stk,formatted)"
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
		printf("ZetScript:%i\n"
				"VirtualMachineData:%i\n"
				"ScriptType:%i\n"
				"ScriptFunction:%i\n"
				"StackElement:%i\n"
				"Symbol:%i\n"
				"Scope:%i\n"
				"ScriptObject:%i\n"
				"ScriptObjectString:%i\n"
				"ScriptObjectVector:%i\n"
				"ScriptObjectObject:%i\n"
				"ScriptObjectClass:%i\n"
				,(int)sizeof(ZetScript)
				, (int)sizeof(VirtualMachineData)
				, (int)sizeof(ScriptType)
				, (int)sizeof(ScriptFunction)
				, (int)sizeof(StackElement)
				, (int)sizeof(Symbol)
				, (int)sizeof(Scope)
				, (int)sizeof(ScriptObject)
				, (int)sizeof(ScriptObjectString)
				, (int)sizeof(ScriptObjectVector)
				, (int)sizeof(ScriptObjectObject)
				, (int)sizeof(ScriptObjectClass)
			);
	}


	 void ZetScript::printGeneratedCode(bool show_system_code){

		 zs_vector *script_classes=script_type_factory->getScriptTypes();
		 // for all classes print code...
		 ScriptFunction *sf_main=MAIN_FUNCTION(this);

		 // list functions
		 zs_vector *symbol_functions=sf_main->scope_script_function->symbol_functions;

		 // print main function
		 ScriptFunction::printGeneratedCode(sf_main);

		 // print defined functions in main function
		 for(int j =0; j < symbol_functions->count; j++){
			Symbol *symbol=(Symbol *)symbol_functions->items[j];

			if(symbol->properties & SYMBOL_PROPERTY_FUNCTION){

				ScriptFunction *local_sf = (ScriptFunction *)symbol->ref_ptr;
				bool show_function=true;
				 if(show_system_code == false && (
						 zs_strutils::starts_with(local_sf->name_script_function,"_@afun_defval")
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

		 for(int i = 1; i < script_classes->count; i++){
			 ScriptType *sc=(ScriptType *)script_classes->get(i);
			 bool show_class=true;

			 // ignore builtin implementations if not chosen ...
			 if(show_system_code == false && (
					 	sc->script_type_name == "System"
					||	sc->script_type_name == "String"
					||	sc->script_type_name == "IteratorString"
					||	sc->script_type_name == "Object"
					||	sc->script_type_name == "IteratorObject"
					||	sc->script_type_name == "Console"
					||	sc->script_type_name == "DateTime"
					||	sc->script_type_name == "Vector"
					||	sc->script_type_name == "IteratorVector"
					||	sc->script_type_name == "Json"
				)){
				 show_class=false;
			 }

			 if(show_class){
				 symbol_functions=sc->script_type_scope->symbol_functions;
				 for(int f = 0; f < symbol_functions->count; f++){
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
	 // FILE MANAGEMENT
	bool ZetScript::isFilenameAlreadyParsed(const zs_string & filename){
		for(int i = 0; i < parsed_files.count; i++){
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
					,key_name.c_str()
					,stk_to_typeof_str(this,stk).c_str()
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

	//
	// STK STRING OBJECT
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------
	StackElement ZetScript::evalInternal(const char * code, unsigned short options, const char * filename, const char *__invoke_file__, int __invoke_line__)  {
		StackElement stk_ret=k_stk_undefined;

		eval_parse_and_compile(this,code,filename);
		link();

		if(options & EvalOption::EVAL_OPTION_SHOW_USER_BYTE_CODE){
			printGeneratedCode(options & EvalOption::EVAL_OPTION_SHOW_SYSTEM_BYTE_CODE);
		}

		if((options & EvalOption::EVAL_OPTION_NO_EXECUTE)==0){
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

	StackElement ZetScript::eval(const zs_string & _expresion, unsigned short _options, const char * _script_filename_by_ref, const char *__invoke_file__, int __invoke_line__)  {

		const char *script_filename_by_ref=getFilenameByRef(_script_filename_by_ref);

		return evalInternal(_expresion.c_str(), _options, script_filename_by_ref,__invoke_file__,__invoke_line__);
	}

	StackElement	ZetScript::eval(const zs_string & _expresion, const char *__invoke_file__, int __invoke_line__){
		return evalInternal(_expresion.c_str(), 0, NULL,__invoke_file__,__invoke_line__);
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
						THROW_SCRIPT_ERROR_FILE_LINEF(error_file.c_str(),error_line,error_str.c_str());
					}else{
						THROW_EXCEPTION(error_str.c_str());
					}
				}
			}

		}

		if(buf_tmp!=NULL){
			free(buf_tmp);
		}

		return stk_ret;
	}

	void ZetScript::clearGlobalVariables(int _idx_start_variable, int _idx_start_function){
		ZS_UNUSUED_PARAM(_idx_start_function);

		zs_string global_symbol;
		int idx_start_variable = _idx_start_variable == ZS_IDX_UNDEFINED ?  idx_current_global_variable_checkpoint:_idx_start_variable;
		ScriptFunction *main_function_object=script_type_factory->getMainFunction();
		Scope *main_scope=MAIN_SCOPE(this);
		zs_vector *local_variables=main_function_object->local_variables;
		zs_vector *global_symbol_variables= main_scope->symbol_variables;
		int n_global_symbol_found=0;
		int v=local_variables->count-1;

		// Because all symbols are ordered by scope, have G as global symbols and L local symbols the disposition is the following,
		//
		// [G][G][G][G]..[G][L][L][L][L][L]..[L]
		//
		// So we only have to delete symbols

		// remove all shared 0 pointers
		if(local_variables->count > 0){
			for (
				;v>=idx_start_variable;
			) {

				StackElement *vm_stk_element=&vm_get_stack_elements(virtual_machine)[v];

				Symbol *symbol=(Symbol *)local_variables->items[v];//(Symbol *)main_function_object->registered_symbols->items[v];


				ScriptObjectObject *var = NULL;

				if(symbol != NULL && symbol->scope == main_scope){ // if variable in global scope

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
					n_global_symbol_found++;

					// erase unused global symbol
					delete symbol;
				}

				STK_SET_NULL(vm_stk_element);

				--v;
			}

			// clear all garbage
			StackElement *vm_stack=vm_get_stack_elements(virtual_machine);
			memset(vm_stack+idx_start_variable,0,sizeof(StackElement)*(VM_STACK_MAX-idx_start_variable));

			// erase global elements that they weren't saved...
			int resize=local_variables->count-(local_variables->count-idx_start_variable);
			local_variables->resize(resize);
			global_symbol_variables->resize(global_symbol_variables->count-n_global_symbol_found);

		}


		vm_remove_empty_shared_pointers(virtual_machine,IDX_CALL_STACK_MAIN);
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
		for(int i=0;i<parsed_files.count;i++){
			delete ((ParsedFile *)parsed_files.items[i]);
		}
		parsed_files.clear();
	}

	void ZetScript::clear(){

		// clearGlobalVariables
		clearGlobalVariables();

		// clearGlobalFunctions
		Scope *main_scope=MAIN_SCOPE(this);
		zs_vector *global_symbol_functions= main_scope->symbol_functions;
		int v=global_symbol_functions->count-1;
		// remove all shared 0 pointers
		if(v >= idx_current_global_function_checkpoint){
			for (
				;v>=idx_current_global_function_checkpoint;
			) {


				Symbol *symbol=(Symbol *)global_symbol_functions->items[v];//(Symbol *)main_function_object->registered_symbols->items[v];
				delete symbol;
				--v;
			}

			int resize=global_symbol_functions->count-(global_symbol_functions->count-idx_current_global_function_checkpoint);
			global_symbol_functions->resize(resize);
		}


		// clearScriptTypes
		zs_vector *script_types= main_scope->symbol_types;
		v=script_types->count-1;
		// remove all shared 0 pointers
		if(v >=idx_current_script_types_checkpoint){
			for (
				;v>=idx_current_script_types_checkpoint;
			) {


				Symbol *symbol=(Symbol *)script_types->items[v];//(Symbol *)main_function_object->registered_symbols->items[v];
				delete symbol;
				--v;
			}

			int resize=script_types->count-(script_types->count-idx_current_script_types_checkpoint);
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
		Scope *main_scope=MAIN_SCOPE(this);
		idx_current_global_variable_checkpoint=main_function_object->local_variables->count;
		idx_current_global_function_checkpoint=main_scope->symbol_functions->count;
		idx_current_script_types_checkpoint=main_scope->symbol_types->count;

		scope_factory->saveState();
		script_function_factory->saveState();
		script_type_factory->saveState();
	}

	bool ZetScript::getFunctionWithUnresolvedSymbolExists(ScriptFunction *_sf){
		for(int i=0;i < functions_with_unresolved_symbols.count; i++){
			if(functions_with_unresolved_symbols.items[i]==(zs_int)_sf){
				return true;
			}
		}

		return false;

	}

	void ZetScript::addUnresolvedSymbol(ScriptFunction *_sf, zs_int idx_instruction){
		if(getFunctionWithUnresolvedSymbolExists(_sf) ==false){
			functions_with_unresolved_symbols.push_back((zs_int)_sf);
		}
		_sf->addUnresolvedSymbol(idx_instruction);
	}

	void ZetScript::link(){
		int i=0;
		while(i<functions_with_unresolved_symbols.count){
			ScriptFunction *_sf=(ScriptFunction *)functions_with_unresolved_symbols.items[i];
			if(_sf->linkUnresolvedSymbols()){ // if link all symbols, erase
				functions_with_unresolved_symbols.erase(i);
			}else{ // next function
				i++;
			}
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

		if(stk_constants != NULL){

			for(auto it=stk_constants->begin(); !it.end();it.next()){//std::map<zs_string,StackElement *>::iterator it=stk_constants->begin();it!=stk_constants->end();it++){
				StackElement *stk=(StackElement *)(it.value);
				if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){
					delete (ScriptObjectString *)stk->value;
				}
				delete stk;
			}
			stk_constants->clear();
			delete stk_constants;

		}

		if(script_filenames_by_ref != NULL){

			for(auto it=script_filenames_by_ref->begin(); !it.end();it.next()){//std::map<zs_string,StackElement *>::iterator it=stk_constants->begin();it!=stk_constants->end();it++){
				char *script_filename_by_ref=(char *)(it.value);
				free(script_filename_by_ref);
			}

			delete script_filenames_by_ref;
			script_filenames_by_ref=NULL;
		}

		eval_deinit();

		resetParsedFiles();

		functions_with_unresolved_symbols.clear();
	}
}
