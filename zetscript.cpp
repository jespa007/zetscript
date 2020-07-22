#include "zetscript.h"

namespace zetscript{

	const char * k_str_void_type =typeid(void).name();
	const char * k_str_int_type_ptr=typeid(int *).name();
	const char * k_str_float_type_ptr=typeid(float *).name();
	const char * k_str_string_type_ptr=typeid(std::string *).name();
	const char * k_str_const_char_type_ptr=typeid(std::string *).name();
	const char * k_str_bool_type_ptr=typeid(bool *).name();
	const char * k_str_int_type=typeid(int).name();
	const char * k_str_unsigned_int_type=typeid(unsigned int).name();
	const char * k_str_intptr_t_type=typeid(intptr_t).name();
	const char * k_str_float_type=typeid(int).name();
	const char * k_str_bool_type=typeid(bool).name();
	const char * k_str_stack_element_type=typeid(bool).name();

	// define prototype ...
	int error_line;
	char error_filename[512];
	char error_description[4096];
	const char *error_type="NA";

	void  writeError(const char *filename, int line, const  char  *input_text, ...){

		char output_text[4096];

		va_list  ap;
		va_start(ap,  input_text);
		vsprintf(output_text,  input_text,  ap);
		va_end(ap);

		memset(error_filename,0,sizeof(error_filename));

#ifdef __DEBUG__
		fprintf(stderr,"\n[ERR %s:%i] %s",filename,line,output_text);
#endif

		if(filename){
			strcpy(error_filename,filename);
		}

		error_line=line;
		strcpy(error_description,output_text);
	}

	int getErrorLine(){
		return error_line;
	}

	const char * getErrorDescription(){
		return error_description;
	}

	const char * getErrorSourceFilename(){
		return error_filename;
	}

	namespace eval{
		void initEval();
	}

	ZetScript::ZetScript(){

		eval::initEval();
		scope_factory = new ScopeFactory(this);
		proxy_function_factory = new FunctionProxyFactory();
		script_function_factory= new ScriptFunctionFactory(this);
		//eval_obj = new ScriptEval(this);
		virtual_machine = new VirtualMachine(this);
		script_class_factory = new ScriptClassFactory(this);

		script_class_factory->init();

		eval_int=0;
		eval_float=0;
		eval_string="";
		eval_bool = false;
	}
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	 // PRINT ASM INFO
	 void ZetScript::printGeneratedCode(){

		 std::vector<ScriptClass *> *script_classes=script_class_factory->getScriptClasses();
		 // for all classes print code...
		 for(unsigned i = 0; i < script_classes->size(); i++){
			 ScriptClass *rc=script_classes->at(i);
			 for(unsigned f = 0; f < rc->local_function.size(); f++){

				 ScriptFunction::printGeneratedCode(rc->local_function[f]);

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

	ConstantValue * ZetScript::registerConstantValue(const std::string & const_name, void *obj, unsigned short properties){

		ConstantValue * info_ptr=NULL;

		if(getRegisteredConstantValue(const_name) == NULL){
			info_ptr=new ConstantValue;
			*info_ptr={obj,NULL,properties};
			(constant_values)[const_name]=info_ptr;
		}else{
			THROW_RUNTIME_ERROR(zs_strutils::format("internal:constant %s already exist",const_name.c_str()));
		}
		return info_ptr;
	}

	ConstantValue * ZetScript::registerConstantValue(const std::string & const_name, int _value){
		intptr_t value = _value;
		unsigned short type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER;
		StackElement *stk;

		if((stk = getRegisteredConstantValue(const_name))!=NULL){
			return stk;
		}
		return registerConstantValue(const_name,(void *)value,type);
	}

	ConstantValue * ZetScript::registerConstantValue(const std::string & const_name, float _value){
		intptr_t value;// = _value;
		*((float *)&value)=_value;
		unsigned short type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT;
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

		ScriptFunction * main_function = this->script_function_factory->getScriptFunction(IDX_MAIN_FUNCTION);

		// clean main functions ... remove script functions and leave c functions...
		for (unsigned f = 0;
			f < main_function->local_function.size()
			;) {
			// get function info
			ScriptFunction * local_function = main_function->local_function[f];

			if ((local_function->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTY_C_OBJECT_REF) {
				main_function->local_function.erase(main_function->local_function.begin() + f);
			}
			else {
				f++;
			}
		}

		// remove variables except c variables ...
		for (unsigned v = 0;
			v < main_function->local_variable.size(); ) {

			if ((main_function->local_variable[v].symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTY_C_OBJECT_REF) {

				main_function->local_variable.erase(main_function->local_variable.begin() + v);

			}
			else {
				v++;
			}
		}

		// remove scope vars...
		scope_factory->clear();
		script_function_factory->clear();
		script_class_factory->clear();
	}

	int * ZetScript::evalIntValue(const std::string & str_to_eval){

		if(!eval(str_to_eval.c_str())){
			return NULL;
		}

		StackElement *se=virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){

				eval_int=(int)((intptr_t)se->stk_value);
				return &eval_int;
			}
			else{
				THROW_RUNTIME_ERROR(zs_strutils::format("evalIntValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));
			}
		}

		return NULL;
	}

	bool * ZetScript::evalBoolValue(const std::string & str_to_eval){

		if(!eval(str_to_eval.c_str())){
			return NULL;
		}

		StackElement *se=virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN){
				eval_bool=(bool)((intptr_t)se->stk_value);
				return &eval_bool;

			}else{
				THROW_RUNTIME_ERROR(zs_strutils::format("evalBoolValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));
			}
		}

		return NULL;
	}

	float * ZetScript::evalFloatValue(const std::string & str_to_eval){

		if(!eval(str_to_eval.c_str())){
			return NULL;
		}

		StackElement *se=virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
				eval_float = *((float *)(&se->stk_value));
				return &eval_float;
			}
			else{
				THROW_RUNTIME_ERROR(zs_strutils::format("evalFloatValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));
			}
		}
		return NULL;
	}

	std::string * ZetScript::evalStringValue(const std::string & str_to_eval){

		if(!eval(str_to_eval.c_str())){
			return NULL;
		}

		StackElement *se=virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){

				eval_string = ((const char *)se->stk_value);
				return &eval_string;
			}
			else{
				zs_strutils::format("evalStringValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
			}
		}

		return NULL;
	}

	bool ZetScript::evalInternal(const char * code, bool vm_exec, bool show_bytecode, const char * filename)  {
		if(!eval::eval(this,code,filename)){
			return false;
		}

		virtual_machine->buildCache();

		if(show_bytecode){
			printGeneratedCode();
		}

		if(vm_exec){
			bool error=false;

			// the first code to callFunction is the main function that in fact is a special member function inside our main class
			virtual_machine->callFunction(script_class_factory->getMainFunction(), NULL,error,NO_PARAMS);

			if(error){
				THROW_SCRIPT_ERROR();
			}
		}

		return true;
	}

	bool ZetScript::eval(const std::string & code, bool exec_vm, bool show_bytecode, const char * filename)  {

		return evalInternal(code.c_str(), exec_vm, show_bytecode, filename);

	}

	bool ZetScript::evalFile(const std::string &  filename, bool vm_exec, bool show_bytecode){
		//int idx_file=-1;
		bool error=false;
		char *buf_tmp=NULL;

		if(!isFilenameAlreadyParsed(filename)){
			ParsedFile ps;
			ps.filename = filename;
			parsed_files.push_back(ps);
			//idx_file=parsed_files.size()-1;
			int n_bytes;

			if((buf_tmp=zs_io::readFile(filename, n_bytes))!=NULL){
				try{
					evalInternal(buf_tmp, vm_exec, show_bytecode,filename.c_str());
				}
				catch(exception::ScriptExceptionError & e){
					free(buf_tmp);
					buf_tmp=NULL;
					THROW_EXCEPTION(e);
					error=true;
				}
			}

		}else{
			// already parsed
			THROW_RUNTIME_ERROR(zs_strutils::format("Filename \"%s\" already parsed",filename));
			error=true;
		}

		if(buf_tmp!=NULL){
			free(buf_tmp);
		}

		return !error;
	}

	bool ZetScript::getScriptObject(const std::string &function_access,ScriptVar **calling_obj,ScriptFunction **fun_obj ){

		//ZS_CLEAR_ERROR_MSG();

		std::vector<std::string> access_var = zs_strutils::split(function_access,'.');
		//ScriptFunction * main_function = main_function;

		/*if(main_function == NULL){
			zs_strutils::format("main_function is not initialized");
			return false;
		}*/
		ScriptFunction * main_function = script_class_factory->getMainFunction();
		*calling_obj = NULL;
		FunctionSymbol *is=NULL;
		StackElement *se=NULL;
		*fun_obj=NULL;

		// 1. some variable in main function ...
		if(access_var.size()>1){
			for(unsigned i=0; i < access_var.size()-1; i++){

				std::string symbol_to_find=access_var[i];
				if(i==0){ // get variable through main_class.main_function (global element)
					//symbol_to_find= ScriptEval::makeSymbolRef(symbol_to_find,IDX_GLOBAL_SCOPE);
					for(unsigned j = 0; j < main_function->local_variable.size() && *calling_obj==NULL; j++){
						if(main_function->local_variable[j].symbol->name==symbol_to_find
						&& main_function->local_variable[j].symbol->idx_scope == IDX_GLOBAL_SCOPE){
							StackElement *stk = virtual_machine->getStackElement(j); // main_function->object_info.local_symbols.variable[j].
							if(stk!=NULL){
								if(stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
									*calling_obj=(ScriptVar *)stk->var_ref;
								}
							}
							else{
								zs_strutils::format("cannot access i (%i)",j);
								return false;
							}
						}
					}

					if((*calling_obj) == NULL){
						zs_strutils::format("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
						return false;
					}

				}else{ // we have got the calling_obj from last iteration ...
					se = (*calling_obj)->getVariableSymbol(symbol_to_find);

					if(se!=NULL){

						if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
							*calling_obj=(ScriptVar *)se->var_ref;
						}else{
							zs_strutils::format("error evaluating \"%s\". Variable name \"%s\" not script variable",function_access.c_str(),symbol_to_find.c_str());
							return false;
						}
					}
					else{
						zs_strutils::format("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
						return false;
					}
				}
			}

			is=(*calling_obj)->getFunctionSymbol(access_var[access_var.size()-1]);
			if(is!=NULL){
				if(is->object.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION){
					*fun_obj=(ScriptFunction *)is->object.stk_value;
				}
			}else{

				zs_strutils::format("error evaluating \"%s\". Cannot find function \"%s\"",function_access.c_str(),access_var[access_var.size()-1].c_str());
				return false;
			}

		}else{ // some function in main function
			//*calling_obj = m_mainObject;
			std::string symbol_to_find=access_var[0];
			for(unsigned i = 0; i < main_function->local_function.size() && *fun_obj==NULL; i++){
				ScriptFunction *aux_fun_obj=main_function->local_function[i];
				if(		aux_fun_obj->symbol_info.symbol->name  == symbol_to_find
				  && aux_fun_obj->symbol_info.symbol->idx_scope == IDX_GLOBAL_SCOPE){
					*fun_obj=aux_fun_obj;
				}
			}
		}

		if(*fun_obj==NULL){
			THROW_RUNTIME_ERROR(zs_strutils::format("error evaluating \"%s\". Variable name \"%s\" is not function type",function_access.c_str(),access_var[access_var.size()-1].c_str()));
			return false;
		}
		return true;
	}

	ZetScript::~ZetScript(){

		virtual_machine->clearGlobalVars();

		// clear objects...
		delete scope_factory;
		delete script_function_factory;
		delete proxy_function_factory;
		delete script_class_factory;

		virtual_machine=NULL;

		for(std::map<std::string,ConstantValue *>::iterator it=constant_values.begin();it!=constant_values.end();it++){
			ConstantValue *icv=it->second;
			switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(icv->properties)){
			default:
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
				//delete (int *)icv->var_ref;
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
				//delete (bool *)icv->stk_value;
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
				//delete (float *)icv->stk_value;
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
