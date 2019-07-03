#include "zetscript.h"

namespace zs{

	// define prototype ...
	int error_line;
	char error_filename[512];
	char error_description[1024];
	const char *error_type="NA";
	bool __init__=false;
	CVirtualMachine *main_vm=NULL;



	void  writeErrorMsg(const char *filename, int line, const  char  *input_text, ...){

		char output_text[4096];

		va_list  ap;
		va_start(ap,  input_text);
		vsprintf(output_text,  input_text,  ap);
		va_end(ap);


		memset(error_filename,0,sizeof(error_filename));

#ifdef __DEBUG__
		fprintf(stderr,"[ERR %s:%i] %s",filename,line,output_text);
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

	const char * getErrorFilename(){
		return error_filename;
	}


	void clear(){

		if(!__init__) {init();}

		main_vm->clearGlobalVars();

		CScriptFunction * main_function = MAIN_FUNCTION;

		// clean main functions ... remove script functions and leave c functions...
		for (unsigned f = 0;
			f < main_function->m_function.size()
			;) {
			// get function info
			CScriptFunction * local_function = main_function->m_function[f];

			if ((local_function->symbol_info.properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF) {
				main_function->m_function.erase(main_function->m_function.begin() + f);
			}
			else {
				f++;
			}

		}

		// remove c variables ...
		for (unsigned v = 0;
			v < main_function->m_variable.size(); ) {

			if ((main_function->m_variable[v].properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF) {

				main_function->m_variable.erase(main_function->m_variable.begin() + v);

			}
			else {
				v++;
			}
		}

		// remove scope vars...
		SCOPE_FACTORY->clear();
		SCRIPT_FUNCTION_FACTORY->clear();
		SCRIPT_CLASS_FACTORY->clear();

	}

	void quit(){

		CURRENT_VM->clearGlobalVars();

		// clear globals...
		CScopeFactory::destroySingleton();
		CScriptFunctionFactory::destroySingleton();
		CScriptClassFactory::destroySingleton();
		CNativeFunction::destroySingleton();
		CEval::destroySingleton();

		m_mainObject = NULL;
		m_mainFunction = NULL;
		show_filename_on_error=true;
		__init__ = false;
		main_vm=NULL;

	}


	void init(){

		CScopeFactory::getInstance();
		CNativeFunction::getInstance();
		CScriptFunctionFactory::getInstance();
		CScriptClassFactory::getInstance();
		CEval::getInstance();

		main_vm = new CVirtualMachine();

		__init__=true;
	}


	int eval_int_value(const std::string & str_to_eval){


		try{
			zs::eval_string(str_to_eval);
		}
		catch(exception::script_error & error){
			THROW_EXCEPTION(error);
		}

		tStackElement *se=CURRENT_VM->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_INTEGER){

				return (int)((intptr_t)se->stkValue);
			}
			else{
				THROW_RUNTIME_ERROR(CZetScriptUtils::sformat("eval_int_value(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));
			}
		}

		return 0;


	}

	bool eval_bool_value(const std::string & str_to_eval){



		try{
			zs::eval_string(str_to_eval);
		}
		catch(exception::script_error & error){
			THROW_EXCEPTION(error);
			return false;
		}

		tStackElement *se=CURRENT_VM->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_BOOLEAN){
				return (bool)((intptr_t)se->stkValue);

			}else{
				THROW_RUNTIME_ERROR(CZetScriptUtils::sformat("eval_bool_value(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));
			}
		}

		return false;
	}

	float eval_float_value(const std::string & str_to_eval){


		try{
			zs::eval_string(str_to_eval);
		}
		catch(exception::script_error & error){
			THROW_EXCEPTION(error);
		}

		tStackElement *se=CURRENT_VM->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_NUMBER){
				float *f = ((float *)(&se->stkValue));
				return *f;
			}
			else{
				CZetScriptUtils::sformat("eval_float_value(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);

			}
		}


		return 0.0f;
	}

	std::string eval_string_value(const std::string & str_to_eval){

		std::string value="---";

		try{
			zs::eval_string(str_to_eval);
		}
		catch(exception::script_error & error){
			THROW_EXCEPTION(error);
			return "";
		}

		tStackElement *se=CURRENT_VM->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_STRING){

				value = ((const char *)se->stkValue);
			}
			else{
				CZetScriptUtils::sformat("eval_string_value(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
			}
		}

		return value;
	}


	void execute(){

		if(!__init__) {THROW_RUNTIME_ERROR ("ZetScript not initialized"); return;}
		//ZS_CLEAR_ERROR_MSG();

		bool error=false;

		// the first code to execute is the main function that in fact is a special member function inside our main class
		main_vm->execute(MAIN_FUNCTION, NULL,error,NO_PARAMS);

		if(error){
			THROW_SCRIPT_ERROR();
		}
	}

	bool eval_string(const std::string & expression, bool exec_vm, const char *filename, bool show_bytecode)  {


		if(!__init__) {init();}

		if(!CEval::evalString(expression)){
			return false;
		}

		if(show_bytecode){
			CEval::printGeneratedCode();
		}

		if(exec_vm){
			execute();
		}

		return true;
	}

	bool eval_file(const std::string & filename, bool exec_vm, bool show_bytecode){

		if(!__init__) {init();}

		char *buf_tmp=NULL;

		bool status = false;

		try{
			if(!CEval::evalFile(filename)){
				return false;
			}
		}catch(exception::script_error & e){
			THROW_EXCEPTION(e);
			return false;
		}

		if(show_bytecode){
			CEval::printGeneratedCode();
		}

		if(exec_vm){
			execute();
		}

		return true;

	}





}
