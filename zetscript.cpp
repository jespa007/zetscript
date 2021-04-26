#include "zetscript.h"

namespace zetscript{

	const char * k_str_void_type =typeid(void).name();
	const char * k_str_zs_int_type_ptr=typeid(zs_int *).name();
	const char * k_str_const_zs_int_type_ptr=typeid(const zs_int *).name();
	const char * k_str_float_type_ptr=typeid(zs_float *).name();
	const char * k_str_const_float_type_ptr=typeid(const zs_float *).name();
	const char * k_str_string_type_ptr=typeid(std::string *).name();
	const char * k_str_char_type_ptr=typeid(char *).name();
	const char * k_str_const_char_type_ptr=typeid(const char *).name();
	const char * k_str_bool_type_ptr=typeid(bool *).name();
	const char * k_str_const_bool_type_ptr=typeid(const bool *).name();
	//const char * k_str_zs_int_type=typeid(zs_int).name();
	//const char * k_str_unsigned_int_type=typeid(unsigned int).name();
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
		constant_string_objects=NULL;

		eval_init();
		scope_factory = new ScopeFactory(this);
		script_function_factory= new ScriptFunctionFactory(this);

		virtual_machine = vm_new(this);
		script_class_factory = new ScriptClassFactory(this);
		script_class_factory->init();
		vm_init(virtual_machine);

		script_class_factory->registerSystem();

		//-------------------------
		// Register built in modules


		// Math mod
		script_class_factory->registerNativeSingletonClass<ModuleMath>("Math");
		script_class_factory->registerNativeStaticConstMember<ModuleMath>("PI",&ModuleMath::PI);
		script_class_factory->registerNativeMemberFunctionStatic<ModuleMath>("sin",ModuleMath::sin);
		script_class_factory->registerNativeMemberFunctionStatic<ModuleMath>("cos",ModuleMath::cos);
		script_class_factory->registerNativeMemberFunctionStatic<ModuleMath>("abs",ModuleMath::abs);
		script_class_factory->registerNativeMemberFunctionStatic<ModuleMath>("pow",ModuleMath::pow);
		script_class_factory->registerNativeMemberFunctionStatic<ModuleMath>("degToRad",ModuleMath::degToRad);
		script_class_factory->registerNativeMemberFunctionStatic<ModuleMath>("random",ModuleMath::random);

		// Console mod
		script_class_factory->registerNativeSingletonClass<ModuleConsole>("ModuleConsole");
		script_class_factory->registerNativeMemberFunctionStatic<ModuleConsole>("readChar",ModuleConsole::readChar);
		script_class_factory->registerNativeMemberFunctionStatic<ModuleConsole>("print",ModuleConsole::print);
		script_class_factory->registerNativeMemberFunctionStatic<ModuleConsole>("println",ModuleConsole::println);
		script_class_factory->registerNativeMemberFunctionStatic<ModuleConsole>("error",ModuleConsole::error);
		script_class_factory->registerNativeMemberFunctionStatic<ModuleConsole>("errorln",ModuleConsole::errorln);


		// System mod
		script_class_factory->registerNativeSingletonClass<ModuleSystem>("ModuleSystem");
		script_class_factory->registerNativeMemberFunctionStatic<ModuleSystem>("clock",ModuleSystem::clock);
		script_class_factory->registerNativeMemberFunctionStatic<ModuleSystem>("eval",ModuleSystem::eval);
		script_class_factory->registerNativeMemberFunctionStatic<ModuleSystem>("assert",ModuleSystem::assert);

		// Custom user function or classes
		eval(
			zs_strutils::format(
				//------------------------------------------------
				// String
				"static String::format(s,...args){" // add static function format to String module
				"	String::formatSf(System::getZetScript(),s,args)"
				"}"
				//------------------------------------------------
				// Console
				"class Console{\n"
				"	static readChar(){"
				"		return ModuleConsole::readChar()"
				"	}"
				"	static print(s,...args){"
				"		ModuleConsole::print(System::getZetScript(),s,args)"
				"	}"
				"	static println(s,...args){"
				"		ModuleConsole::println(System::getZetScript(),s,args)"
				"	}"
				"	static error(s,...args){"
				"		ModuleConsole::error(System::getZetScript(),s,args)"
				"	}"
				"	static errorln(s,...args){"
				"		ModuleConsole::errorln(System::getZetScript(),s,args)"
				"	}"
				"}"
				//------------------------------------------------
				// System
				"class System{\n"
				"	static clock(){"
				"		return ModuleSystem::clock()"
				"	}"
				"	static assert(check,s,...args){"
				"		ModuleSystem::assert(System::getZetScript(),check,s,args)"
				"	}"
				"	static getZetScript(){"
				"		return ptrToZetScriptPtr(0x%x);" // ptrToZetScript it gets current this
				"   }"
				"}"
			,
			(void *)this
			)
		);


	/*	zs->eval("function test_function(){ print(\"hola\")}",false);
		zs->eval("class TestClass{test(){print(\"hola\")}} var test_class=new TestClass()",false);*/

		saveState();
	}
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	 // PRINT ASM INFO
	 void ZetScript::printGeneratedCode(bool show_system_code){

		 zs_vector *script_classes=script_class_factory->getScriptClasses();
		 // for all classes print code...
		 ScriptFunction *sf_main=MAIN_FUNCTION(this);

		 // print main function
		 ScriptFunction::printGeneratedCode(sf_main);

		 // print defined functions in main function
		 for(unsigned j =0; j < sf_main->registered_symbols->count; j++){
			Symbol *symbol=(Symbol *)sf_main->registered_symbols->items[j];

			if(symbol->properties & SYMBOL_PROPERTY_FUNCTION){

				ScriptFunction *local_sf = (ScriptFunction *)symbol->ref_ptr;

				if(( local_sf->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF) != SYMBOL_PROPERTY_C_OBJECT_REF){
					ScriptFunction::printGeneratedCode(local_sf);
				}
			}
		}

		 for(unsigned i = 1; i < script_classes->count; i++){
			 ScriptClass *rc=(ScriptClass *)script_classes->get(i);
			 bool show_class=true;

			 if(show_system_code == false && (
					 	rc->symbol_class.name == "System"
					 ||	rc->symbol_class.name == "String"
				)){
				 show_class=false;
			 }

			 if(show_class){
				 for(unsigned f = 0; f < rc->symbol_members->count; f++){
					 Symbol *symbol=(Symbol *)rc->symbol_members->items[f];
					 if(symbol->properties & SYMBOL_PROPERTY_FUNCTION){
						 ScriptFunction::printGeneratedCode((ScriptFunction *)symbol->ref_ptr,rc);
					 }
				 }
			 }
		 }
	 }
	 // PRINT ASM INFO
	 //----------------------------------------------------------------------------------------------------------------------------------------------------------------

	//--------------------------------
	 // FILE MANAGEMENT
	bool ZetScript::isFilenameAlreadyParsed(const std::string & filename){
		for(unsigned i = 0; i < parsed_files.size(); i++){
			if(parsed_files.at(i).filename==filename){
				return true;
			}
		}
		return false;
	}

	const char * ZetScript::getParsedFilenameFromIdx(unsigned idx){

		if(idx >= parsed_files.size()){
			THROW_RUNTIME_ERROR("out of bounds");
			return DEFAULT_NO_FILENAME;
		}
		return parsed_files.at(idx).filename.c_str();
	}

	//-----------------------------------------------------------------------------------------------------------------------------------------
	//
	// CONSTANT MANAGEMENT

	StackElement *ZetScript::getRegisteredConstantScriptObjectString(const std::string & const_name){

		if(constant_string_objects==NULL){
			constant_string_objects=new std::map<std::string,StackElement *>();
		}

		if((constant_string_objects)->count(const_name) == 1){
			return (constant_string_objects)->at(const_name);
		}
		return NULL;
	}

	StackElement * ZetScript::registerConstantScriptObjectString(const std::string & const_name){

		StackElement *stk=NULL;
		ScriptObjectString *so=NULL;

		if((stk = getRegisteredConstantScriptObjectString(const_name))!=NULL){
			return stk;
		}

		stk=new StackElement;

		(*constant_string_objects)[const_name]=stk;

		so=ZS_NEW_OBJECT_STRING(this);
		// swap values stk_ref/stk_value
		so->set(const_name);

		stk->stk_value=so;
		stk->properties=MSK_STK_PROPERTY_SCRIPT_OBJECT | MSK_STK_PROPERTY_READ_ONLY;

		return stk;

	}

	// CONSTANT MANAGEMENT
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------
	void ZetScript::setPrintOutCallback(void (* _printout_callback)(const char *)){
		print_out_callback=_printout_callback;
	}

	void (* ZetScript::print_out_callback)(const char *) = NULL;

	zs_int * ZetScript::evalIntValue(const std::string & str_to_eval){

		eval(str_to_eval.c_str());

		StackElement *se=vm_get_top_stack_element_from_stack(virtual_machine);

		if(se != NULL){

			if(se->properties & MSK_STK_PROPERTY_ZS_INT){

				eval_int=(zs_int)se->stk_value;
				return &eval_int;
			}
			else{
				THROW_RUNTIME_ERROR("evalIntValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
			}
		}

		return NULL;
	}

	bool * ZetScript::evalBoolValue(const std::string & str_to_eval){

		eval(str_to_eval.c_str());

		StackElement *se=vm_get_top_stack_element_from_stack(virtual_machine);

		if(se != NULL){

			if(se->properties & MSK_STK_PROPERTY_BOOL){
				eval_bool=(bool)((zs_int)se->stk_value);
				return &eval_bool;

			}else{
				THROW_RUNTIME_ERROR("evalBoolValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
			}
		}

		return NULL;
	}

	zs_float * ZetScript::evalFloatValue(const std::string & str_to_eval){
		eval(str_to_eval.c_str());
		StackElement *se=vm_get_top_stack_element_from_stack(virtual_machine);

		if(se != NULL){
			if(se->properties & MSK_STK_PROPERTY_ZS_FLOAT){
				eval_float = *((zs_float *)(&se->stk_value));
				return &eval_float;
			}
			else{
				THROW_RUNTIME_ERROR("evalFloatValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
			}
		}
		return NULL;
	}

	std::string * ZetScript::evalStringValue(const std::string & str_to_eval){

		// eval and preserve zero shares to get value...
		evalInternal(str_to_eval.c_str());

		StackElement *se=vm_get_top_stack_element_from_stack(virtual_machine);

		if(se != NULL){

			if(STK_IS_SCRIPT_OBJECT_STRING(se)){

				eval_string = ((ScriptObjectString *)se->stk_value)->toString();
				return &eval_string;
			}
			else{
				THROW_RUNTIME_ERROR("evalStringValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
			}
		}
		return NULL;
	}

	void ZetScript::evalInternal(const char * code, unsigned short options, const char * filename)  {

		eval_parse_and_compile(this,code,filename);

		if(options & EvalOption::EVAL_OPTION_SHOW_USER_CODE){
			printGeneratedCode(options & EvalOption::EVAL_OPTION_SHOW_SYSTEM_CODE);
		}

		if(options & EvalOption::EVAL_OPTION_EXECUTE){
			// the first code to execute is the main function that in fact is a special member function inside our main class
			vm_execute(
					virtual_machine
					,NULL
					,script_class_factory->getMainFunction()
			);
		}
	}

	void ZetScript::eval(const std::string & code, unsigned short options, const char * filename)  {

		evalInternal(code.c_str(), options, filename);
	}

	void ZetScript::evalFile(const std::string &  filename, unsigned short options){
		//int idx_file=-1;
		char *buf_tmp=NULL;

		if(!isFilenameAlreadyParsed(filename)){
			ParsedFile ps;
			ps.filename = filename;
			parsed_files.push_back(ps);
			//idx_file=parsed_files.size()-1;
			size_t n_bytes;

			if((buf_tmp=zs_file::read(filename, n_bytes))!=NULL){
				bool error=false;
				std::string error_str;
				try{
					evalInternal(buf_tmp,options,filename.c_str());
				}
				catch(std::exception & e){
					error=true;
					error_str=e.what();
				}

				// deallocate before throw errors...
				free(buf_tmp);
				buf_tmp=NULL;

				if(error){
					THROW_EXCEPTION(error_str.c_str());
				}

			}

		}else{
			// already parsed
			THROW_RUNTIME_ERROR("Filename \"%s\" already parsed",filename);
		}

		if(buf_tmp!=NULL){
			free(buf_tmp);
		}
	}

	void ZetScript::getScriptObject(const std::string &function_access,ScriptObjectClass **calling_obj,ScriptFunction **fun_obj ){

	}

	void ZetScript::clearGlobalVariables(int _idx_start){

		int idx_start = _idx_start == ZS_IDX_UNDEFINED ?  idx_current_global_variable_checkpoint:_idx_start;
		ScriptFunction *main_function_object=script_class_factory->getMainFunction();

		// remove all shared 0 pointers
		if(main_function_object->registered_symbols->count > 0){
			// set global top stack element
			StackElement *vm_stk_element=&vm_get_stack_elements(virtual_machine)[main_function_object->registered_symbols->count-1];

			for (
					int v = main_function_object->registered_symbols->count-1;
					v >= idx_start;
					v--) {
				Symbol *symbol=(Symbol *)main_function_object->registered_symbols->items[v];
				ScriptObjectObject *var = NULL;

				if(vm_stk_element->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
					var =((ScriptObjectObject *)(vm_stk_element->stk_value));
					if(var){
						if(var->shared_pointer != NULL){
							if(!vm_unref_shared_script_object(this->virtual_machine,var,IDX_CALL_STACK_MAIN)){
								THROW_RUNTIME_ERROR("error clearing variables: %s",vm_get_error(this->virtual_machine));
							}
						}
					}
				}

				main_function_object->registered_symbols->pop_back();
				*vm_stk_element--=stk_undefined;
			}
		}

		vm_remove_empty_shared_pointers(virtual_machine,IDX_CALL_STACK_MAIN);
	}

	void ZetScript::setClearGlobalVariablesCheckpoint(){
		idx_current_global_variable_checkpoint=this->script_class_factory->getMainFunction()->registered_symbols->count-1;
	}

	void ZetScript::clear(){

		clearGlobalVariables();

		scope_factory->clear();
		script_function_factory->clear();
		script_class_factory->clear();
	}

	void ZetScript::saveState(){

	}

	ZetScript::~ZetScript(){

		// delete system and string...
		//eval("/*delete System;*/delete String;");

		clearGlobalVariables(0);
		/*scope_factory->clear(IDX_SCRIPT_SCOPE_MAIN+1);
		script_function_factory->clear(IDX_SCRIPT_FUNCTION_MAIN+1);
		script_class_factory->clear(IDX_SCRIPT_CLASS_MAIN+1);*/

		// clear objects...
		vm_delete(virtual_machine);
		delete scope_factory;
		delete script_function_factory;
		delete script_class_factory;

		virtual_machine=NULL;

		if(constant_string_objects != NULL){

			for(std::map<std::string,StackElement *>::iterator it=constant_string_objects->begin();it!=constant_string_objects->end();it++){
				StackElement *icv=it->second;
				delete (ScriptObjectString *)icv->stk_value;
				delete icv;
			}
			constant_string_objects->clear();
			delete constant_string_objects;
		}

		eval_deinit();


	}
}
