#include "zetscript.h"

namespace zetscript{

	const char * k_str_void_type =typeid(void).name();
	const char * k_str_zs_int_type_ptr=typeid(zs_int *).name();
	const char * k_str_const_zs_int_type_ptr=typeid(const zs_int *).name();
	const char * k_str_float_type_ptr=typeid(float *).name();
	const char * k_str_const_float_type_ptr=typeid(const float *).name();
	const char * k_str_string_type_ptr=typeid(std::string *).name();
	const char * k_str_char_type_ptr=typeid(char *).name();
	const char * k_str_const_char_type_ptr=typeid(const char *).name();
	const char * k_str_bool_type_ptr=typeid(bool *).name();
	const char * k_str_const_bool_type_ptr=typeid(const bool *).name();
	//const char * k_str_zs_int_type=typeid(zs_int).name();
	//const char * k_str_unsigned_int_type=typeid(unsigned int).name();
	const char * k_str_zs_int_type=typeid(zs_int).name();
	const char * k_str_float_type=typeid(float).name();
	const char * k_str_bool_type=typeid(bool).name();
	const char * k_str_stack_element_type=typeid(StackElement).name();


	namespace eval {
		void init();
		void deinit();
	}


	ZetScript::ZetScript(){
		eval_int=0;
		eval_float=0;
		eval_string="";
		eval_bool = false;
		idx_current_global_variable_checkpoint=0;

		eval::init();
		scope_factory = new ScopeFactory(this);
		function_proxy_factory = new FunctionProxyFactory();
		script_function_factory= new ScriptFunctionFactory(this);

		virtual_machine = new VirtualMachine(this);
		script_class_factory = new ScriptClassFactory(this);
		script_class_factory->init();
		virtual_machine->init();

		script_class_factory->registerSystem();

		//-------------------------
		// Register built in modules

		// String
		script_class_factory->registerNativeClass<StringBuiltIn>("StringBuiltIn");
		script_class_factory->registerNativeMemberFunctionStatic<StringBuiltIn>("format",StringBuiltIn::formatSf);

		// Math
		script_class_factory->registerNativeSingletonClass<MathBuiltIn>("Math");
		script_class_factory->registerNativeStaticConstMember<MathBuiltIn>("PI",&MathBuiltIn::PI);
		script_class_factory->registerNativeMemberFunctionStatic<MathBuiltIn>("sin",MathBuiltIn::sin);
		script_class_factory->registerNativeMemberFunctionStatic<MathBuiltIn>("cos",MathBuiltIn::cos);
		script_class_factory->registerNativeMemberFunctionStatic<MathBuiltIn>("abs",MathBuiltIn::abs);
		script_class_factory->registerNativeMemberFunctionStatic<MathBuiltIn>("pow",MathBuiltIn::pow);
		script_class_factory->registerNativeMemberFunctionStatic<MathBuiltIn>("degToRad",MathBuiltIn::degToRad);

		// System
		script_class_factory->registerNativeSingletonClass<SystemBuiltIn>("SystemBuiltIn");
		script_class_factory->registerNativeMemberFunctionStatic<SystemBuiltIn>("readChar",SystemBuiltIn::readChar);
		script_class_factory->registerNativeMemberFunctionStatic<SystemBuiltIn>("clock",SystemBuiltIn::clock);
		script_class_factory->registerNativeMemberFunctionStatic<SystemBuiltIn>("print",SystemBuiltIn::printSf);
		script_class_factory->registerNativeMemberFunctionStatic<SystemBuiltIn>("println",SystemBuiltIn::printlnSf);
		script_class_factory->registerNativeMemberFunctionStatic<SystemBuiltIn>("eval",SystemBuiltIn::evalSf);

		// Custom user function or classes
		eval(
			zs_strutils::format(

				"class String{\n"
				"	static format(s,...args){"
				"		StringBuiltIn::format(ptrToZetScriptPtr(0x%x),s,args)" // passing this because is registered as static
				"	}"
				"}"
				""
				"class System{\n"
				"	static readChar(){"
				"		return SystemBuiltIn::readChar()"
				"	}"
				"	static clock(){"
				"		return SystemBuiltIn::clock()"
				"	}"
				"	static print(s,...args){"
				"		SystemBuiltIn::print(ptrToZetScriptPtr(0x%x),s,args)"  // passing this because is registered as static
				"	}"
				"	static println(s,...args){"
				"		SystemBuiltIn::println(ptrToZetScriptPtr(0x%x),s,args)"  // passing this because is registered as static
				"	}"
				"}"
			,
			(void *)this,
			(void *)this,
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

	ConstantValue *ZetScript::getRegisteredConstantValue(const std::string & const_name){

		if((constant_values).count(const_name) == 1){
			return (constant_values)[const_name];
		}
		return NULL;
	}

	ConstantValue * ZetScript::registerConstantValue(const std::string & const_name, ConstantValue constant_value){
		if(getRegisteredConstantValue(const_name) != NULL){
			THROW_RUNTIME_ERROR("internal:constant %s already exist",const_name.c_str());
		}

		ConstantValue *info_ptr=new ConstantValue;
		*info_ptr=constant_value;
		(constant_values)[const_name]=info_ptr;
		return info_ptr;
	}

	ConstantValue * ZetScript::registerConstantValue(const std::string & const_name, void *obj, unsigned short properties){
		return registerConstantValue(const_name,{obj,NULL,properties});
	}

	ConstantValue * ZetScript::registerConstantValue(const std::string & const_name, ScriptObjectString * _value){

		unsigned short properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING | MSK_STACK_ELEMENT_PROPERTY_READ_ONLY;
		StackElement *stk;

		if((stk = getRegisteredConstantValue(const_name))!=NULL){
			return stk;
		}


		stk=registerConstantValue(const_name,(void *)_value,properties);
		// swap values stk_ref/stk_value
		stk->var_ref=(void *)_value;
		stk->stk_value=(char *)_value->str_value.c_str();

		return stk;

	}

	ConstantValue * ZetScript::registerConstantValue(const std::string & const_name, zs_int _value){
		zs_int value = _value;
		unsigned short properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT;
		StackElement *stk;

		if((stk = getRegisteredConstantValue(const_name))!=NULL){
			return stk;
		}
		return registerConstantValue(const_name,(void *)value,properties);
	}

	ConstantValue * ZetScript::registerConstantValue(const std::string & const_name, float _value){
		zs_int value;// = _value;
		*((float *)&value)=_value;
		unsigned short properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT;
		StackElement *stk;

		if((stk = getRegisteredConstantValue(const_name))!=NULL){
			return stk;
		}
		return registerConstantValue(const_name,(void *)value,properties);
	}

	// CONSTANT MANAGEMENT
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------

	/*void  ZetScript::internalPrintError(const char *s){
		 virtual_machine->SetError(s);
	}*/
	void ZetScript::setPrintOutCallback(void (* _printout_callback)(const char *)){
		print_out_callback=_printout_callback;
	}

	void (* ZetScript::print_out_callback)(const char *) = NULL;

	zs_int * ZetScript::evalIntValue(const std::string & str_to_eval){

		eval(str_to_eval.c_str());

		StackElement *se=virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT){

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

		StackElement *se=virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL){
				eval_bool=(bool)((zs_int)se->stk_value);
				return &eval_bool;

			}else{
				THROW_RUNTIME_ERROR("evalBoolValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
			}
		}

		return NULL;
	}

	float * ZetScript::evalFloatValue(const std::string & str_to_eval){
		eval(str_to_eval.c_str());
		StackElement *se=virtual_machine->getLastStackValue();

		if(se != NULL){
			if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
				eval_float = *((float *)(&se->stk_value));
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
		evalInternal(str_to_eval.c_str(),EvalOption::EVAL_OPTION_EXECUTE|EvalOption::EVAL_OPTION_PRESERVE_0_SHARES);

		StackElement *se=virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){

				eval_string = ((const char *)se->stk_value);
				return &eval_string;
			}
			else{
				THROW_RUNTIME_ERROR("evalStringValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
			}
		}

		return NULL;
	}

	void ZetScript::evalInternal(const char * code, unsigned short options, const char * filename)  {

		eval::eval(this,code,filename);

		if(options & EvalOption::EVAL_OPTION_SHOW_USER_CODE){
			printGeneratedCode(options & EvalOption::EVAL_OPTION_SHOW_SYSTEM_CODE);
		}

		if(options & EvalOption::EVAL_OPTION_EXECUTE){
			// the first code to execute is the main function that in fact is a special member function inside our main class
			virtual_machine->execute(
					NULL
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

	void ZetScript::getScriptObject(const std::string &function_access,ScriptObject **calling_obj,ScriptFunction **fun_obj ){

	}

	void ZetScript::clearGlobalVariables(int _idx_start){

		int idx_start = _idx_start == ZS_IDX_UNDEFINED ?  idx_current_global_variable_checkpoint:_idx_start;
		ScriptFunction *main_function_object=script_class_factory->getMainFunction();

		// remove all shared 0 pointers
		if(main_function_object->registered_symbols->count > 0){
			// set global top stack element
			StackElement *vm_stk_element=&virtual_machine->getVmStackPtr()[main_function_object->registered_symbols->count-1];

			for (
					int v = main_function_object->registered_symbols->count-1;
					v >= idx_start;
					v--) {
				Symbol *symbol=(Symbol *)main_function_object->registered_symbols->items[v];
				ScriptObject *var = NULL;

				if(vm_stk_element->properties &MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT){
					var =((ScriptObject *)(vm_stk_element->var_ref));
					if(var){
						if(var->shared_pointer != NULL){
							if(!var->unrefSharedPtr(IDX_CALL_STACK_MAIN)){
								THROW_RUNTIME_ERROR("error clearing variables: %s",this->virtual_machine->getError());
							}
						}
					}
				}

				main_function_object->registered_symbols->pop_back();
				*vm_stk_element--={0,NULL,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
			}
		}

		this->virtual_machine->removeEmptySharedPointers(IDX_CALL_STACK_MAIN);
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
		delete virtual_machine;
		delete scope_factory;
		delete script_function_factory;
		delete function_proxy_factory;
		delete script_class_factory;

		virtual_machine=NULL;

		for(std::map<std::string,ConstantValue *>::iterator it=constant_values.begin();it!=constant_values.end();it++){
			ConstantValue *icv=it->second;
			switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(icv->properties)){
			default:
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
				delete (ScriptObjectString *)icv->var_ref;
				break;
			}
			delete icv;
		}
		constant_values.clear();

		eval::deinit();


	}
}
