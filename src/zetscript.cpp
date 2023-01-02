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

		compiled_symbol_name = new zs_map();
		//-------------------------
		// Register built in modules

		// Math mod
		ScriptType *cl=script_type_factory->registerScriptType("Math","",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);

		cl->bindStaticConstMemberVariable("PI",&MathModule_PI);
		cl->bindStaticMemberFunction("sin",MathModule_sin);
		cl->bindStaticMemberFunction("cos",MathModule_cos);
		cl->bindStaticMemberFunction("abs",MathModule_abs);
		cl->bindStaticMemberFunction("pow",MathModule_pow);
		cl->bindStaticMemberFunction("degToRad",MathModule_degToRad);
		cl->bindStaticMemberFunction("random",MathModule_random);
		cl->bindStaticMemberFunction("max",MathModule_max);
		cl->bindStaticMemberFunction("min",MathModule_min);
		cl->bindStaticMemberFunction("sqrt",MathModule_sqrt);
		cl->bindStaticMemberFunction("floor",MathModule_floor);

		// Console mod
		cl=script_type_factory->registerScriptType("Console","",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);
		cl->bindStaticMemberFunction("readChar",ConsoleModule_readChar);
		cl->bindStaticMemberFunction("outNative",ConsoleModule_out);
		cl->bindStaticMemberFunction("outlnNative",ConsoleModule_outln);
		cl->bindStaticMemberFunction("errorNative",ConsoleModule_error);
		cl->bindStaticMemberFunction("errorlnNative",ConsoleModule_errorln);

		// System mod
		cl=script_type_factory->registerScriptType("System","",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);
		cl->bindStaticMemberFunction("clock",SystemModule_clock);
		cl->bindStaticMemberFunction("evalNative",SystemModule_eval);
		//cl->bindStaticMemberFunction("assertNative",SystemModule_assert);
		cl->bindStaticMemberFunction("errorNative",SystemModule_error);

		// Json mod
		cl=script_type_factory->registerScriptType("Json","",SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE);
		cl->bindStaticMemberFunction("serializeNative",static_cast<StringScriptObject * (*)(ZetScript *zs,StackElement *)>(JsonModule_serialize));
		cl->bindStaticMemberFunction("serializeNative",static_cast<StringScriptObject * (*)(ZetScript *zs,StackElement *, bool *)>(JsonModule_serialize));
		cl->bindStaticMemberFunction("deserialize",JsonModule_deserialize);
		//---------------------------------------------
		// DateTime
		cl=bindType<zs_datetime>("DateTime",DateTimeModule_new,DateTimeModule_delete);

		/*registerStaticMemberFunction<zs_datetime>("_add",DateTimeModule_add);
		registerStaticMemberFunction<zs_datetime>("_sub",DateTimeModule_sub);*/

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
				"	if((typeof _check)!=bool){"
				"		System::errorNative(\"Error, executing  'System::assert' first parameter must be boolean but it was {0}\",[typeof _check])"
				"	}"
				"	if(_check==false){"
				"		System::errorNative(\"Assert error: \"+_str_error,_args)"
				"	}"
				"}"
				"static System::error(_str_error,..._args){"
				"	System::errorNative(_str_error,_args)"
				"}"
				"static System::eval(_str_eval,_params){"
				"	return System::evalNative(_str_eval,_params)"
				"}"
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
				"VectorScriptObject:%i\n"
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
				, (int)sizeof(VectorScriptObject)
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
					|| sc->str_script_type == "Vector"
					|| sc->str_script_type == "IteratorVector"
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

	ObjectScriptObject * ZetScript::newScriptObjectObject(){
		return ObjectScriptObject::newScriptObjectObject(this);
	}

	StringScriptObject * ZetScript::newStringScriptObject(){
		return StringScriptObject::newStringScriptObject(this);
	}

	VectorScriptObject * ZetScript::newVectorScriptObject(){
		return VectorScriptObject::newVectorScriptObject(this);
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

				Symbol *symbol=(Symbol *)local_variables->items[v];//(Symbol *)main_function_object->registered_symbols->items[v];


				ScriptObject *script_object = NULL;

				if(symbol != NULL && symbol->scope == main_scope){ // if variable in global scope

					if(vm_stk_element->properties & STK_PROPERTY_SCRIPT_OBJECT){
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

	}
}
