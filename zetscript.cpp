#include "zetscript.h"

namespace zetscript{

	const char * k_str_void_type =typeid(void).name();
	const char * k_str_int_type_ptr=typeid(int *).name();
	const char * k_str_const_int_type_ptr=typeid(const int *).name();
	const char * k_str_float_type_ptr=typeid(float *).name();
	const char * k_str_const_float_type_ptr=typeid(const float *).name();
	const char * k_str_string_type_ptr=typeid(std::string *).name();
	const char * k_str_char_type_ptr=typeid(char *).name();
	const char * k_str_const_char_type_ptr=typeid(const char *).name();
	const char * k_str_bool_type_ptr=typeid(bool *).name();
	const char * k_str_const_bool_type_ptr=typeid(const bool *).name();
	const char * k_str_int_type=typeid(int).name();
	const char * k_str_unsigned_int_type=typeid(unsigned int).name();
	const char * k_str_intptr_t_type=typeid(intptr_t).name();
	const char * k_str_float_type=typeid(float).name();
	const char * k_str_bool_type=typeid(bool).name();
	const char * k_str_stack_element_type=typeid(StackElement).name();

	namespace eval{
		void init_eval();
	}

	ZetScript::ZetScript(){

		eval::init_eval();
		scope_factory = new ScopeFactory(this);
		function_proxy_factory = new FunctionProxyFactory();
		script_function_factory= new ScriptFunctionFactory(this);

		virtual_machine = new VirtualMachine(this);
		script_class_factory = new ScriptClassFactory(this);

		script_class_factory->init();
		virtual_machine->init();

		eval_int=0;
		eval_float=0;
		eval_string="";
		eval_bool = false;
	}
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	 // PRINT ASM INFO
	 void ZetScript::printGeneratedCode(){

		 zs_vector *script_classes=script_class_factory->getScriptClasses();
		 // for all classes print code...
		 for(unsigned i = 0; i < script_classes->count; i++){
			 ScriptClass *rc=(ScriptClass *)script_classes->get(i);
			 for(unsigned f = 0; f < rc->symbol_members->count; f++){
				 Symbol *symbol=(Symbol *)rc->symbol_members->items[f];
				 if(symbol->symbol_properties & SYMBOL_PROPERTY_IS_SCRIPT_FUNCTION){
					 ScriptFunction::printGeneratedCode((ScriptFunction *)symbol->ref_ptr);
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

	ConstantValue * ZetScript::registerConstantValue(const std::string & const_name, int _value){
		intptr_t value = _value;
		unsigned short type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_READ_ONLY;
		StackElement *stk;

		if((stk = getRegisteredConstantValue(const_name))!=NULL){
			return stk;
		}
		return registerConstantValue(const_name,(void *)value,type);
	}

	ConstantValue * ZetScript::registerConstantValue(const std::string & const_name, float _value){
		intptr_t value;// = _value;
		*((float *)&value)=_value;
		unsigned short type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT|MSK_STACK_ELEMENT_PROPERTY_READ_ONLY;
		StackElement *stk;

		if((stk = getRegisteredConstantValue(const_name))!=NULL){
			return stk;
		}
		return registerConstantValue(const_name,(void *)value,type);
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

	void ZetScript::clear(){

		virtual_machine->clearGlobalVars();

		// clear all script artifacts excepts C ones and global/main function/scope...
		scope_factory->clear();
		script_function_factory->clear();
		script_class_factory->clear();
	}

	int * ZetScript::evalIntValue(const std::string & str_to_eval){

		eval(str_to_eval.c_str());

		StackElement *se=virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){

				eval_int=(int)((intptr_t)se->stk_value);
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

			if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN){
				eval_bool=(bool)((intptr_t)se->stk_value);
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

		eval(str_to_eval.c_str());

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

	void ZetScript::evalInternal(const char * code, bool vm_exec, bool show_bytecode, const char * filename)  {
		eval::eval(this,code,filename);

		if(show_bytecode){
			printGeneratedCode();
		}

		if(vm_exec){
			// the first code to execute is the main function that in fact is a special member function inside our main class
			virtual_machine->execute(script_class_factory->getMainFunction(), NULL);
		}
	}

	void ZetScript::eval(const std::string & code, bool exec_vm, bool show_bytecode, const char * filename)  {

		evalInternal(code.c_str(), exec_vm, show_bytecode, filename);
	}

	void ZetScript::evalFile(const std::string &  filename, bool vm_exec, bool show_bytecode){
		//int idx_file=-1;
		char *buf_tmp=NULL;

		if(!isFilenameAlreadyParsed(filename)){
			ParsedFile ps;
			ps.filename = filename;
			parsed_files.push_back(ps);
			//idx_file=parsed_files.size()-1;
			size_t n_bytes;

			if((buf_tmp=zs_file::read(filename, n_bytes))!=NULL){
				bool error;
				std::string error_str;
				try{
					evalInternal(buf_tmp, vm_exec, show_bytecode,filename.c_str());
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

	void ZetScript::getScriptObject(const std::string &function_access,ScriptVar **calling_obj,ScriptFunction **fun_obj ){
		//ZS_CLEAR_ERROR_MSG();
		std::vector<std::string> access_var = zs_strutils::split(function_access,'.');
		//ScriptFunction * main_function = main_function;

		/*if(main_function == NULL){
			zs_strutils::format("main_function is not initialized");
			return false;
		}*/
		ScriptFunction * main_function = script_class_factory->getMainFunction();
		*calling_obj = NULL;
		StackElement *se=NULL;
		StackElement *is;
		*fun_obj=NULL;

		// 1. some variable in main function ...
		if(access_var.size()>1){
			for(unsigned i=0; i < access_var.size()-1; i++){
				std::string symbol_to_find=access_var[i];
				if(i==0){ // get variable through main_class.main_function (global element)
					for(unsigned j = 0; j < main_function->registered_symbols->count && *calling_obj==NULL; j++){
						Symbol * registered_symbol=(Symbol *)main_function->registered_symbols->items[j];
						if(registered_symbol->name==symbol_to_find
						&& registered_symbol->scope == MAIN_SCOPE(this)){
							StackElement *stk = virtual_machine->getStackElement(j); // main_function->object_info.local_symbols.variable[j].
							if(stk!=NULL){
								if(stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
									*calling_obj=(ScriptVar *)stk->var_ref;
								}
							}
							else{
								THROW_RUNTIME_ERROR("cannot access i (%i)",j);
							}
						}
					}

					if((*calling_obj) == NULL){
						THROW_RUNTIME_ERROR("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
					}

				}else{ // we have got the calling_obj from last iteration ...
					se = (*calling_obj)->getProperty(symbol_to_find);
					if(se!=NULL){
						if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
							*calling_obj=(ScriptVar *)se->var_ref;
						}else{
							THROW_RUNTIME_ERROR("error evaluating \"%s\". Variable name \"%s\" not script variable",function_access.c_str(),symbol_to_find.c_str());
						}
					}
					else{
						THROW_RUNTIME_ERROR("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
					}
				}
			}

			is=(*calling_obj)->getProperty(access_var[access_var.size()-1]);
			if(is!=NULL){
				if(is->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION){
					*fun_obj=(ScriptFunction *)is->var_ref;
				}
			}else{
				THROW_RUNTIME_ERROR("error evaluating \"%s\". Cannot find function \"%s\"",function_access.c_str(),access_var[access_var.size()-1].c_str());
			}

		}else{ // some function in main function
			//*calling_obj = m_mainObject;
			std::string symbol_to_find=access_var[0];
			for(unsigned i = 0; i < main_function->registered_symbols->count && *fun_obj==NULL; i++){
				Symbol *symbol=(Symbol *)main_function->registered_symbols->items[i];
				if(symbol->symbol_properties & SYMBOL_PROPERTY_IS_SCRIPT_FUNCTION){
					ScriptFunction *aux_fun_obj=(ScriptFunction *)symbol->ref_ptr;
					if(		aux_fun_obj->symbol.name  == symbol_to_find
					  && aux_fun_obj->symbol.scope == MAIN_SCOPE(this)){
						*fun_obj=aux_fun_obj;
					}
				}
			}
		}

		if(*fun_obj==NULL){
			THROW_RUNTIME_ERROR("error evaluating \"%s\". Variable name \"%s\" is not function type",function_access.c_str(),access_var[access_var.size()-1].c_str());
		}
	}

	ZetScript::~ZetScript(){

		virtual_machine->clearGlobalVars();

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
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
				delete (ScriptVarString *)icv->var_ref;
				break;
			}
			delete icv;
		}
		constant_values.clear();


	}
}
