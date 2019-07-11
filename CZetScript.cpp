#include "zetscript.h"

namespace zetscript{

	// define prototype ...
	int error_line;
	char error_filename[512];
	char error_description[1024];
	const char *error_type="NA";


	void  write_error(const char *filename, int line, const  char  *input_text, ...){

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


	void CZetScript::clear(){

		_virtual_machine->clearGlobalVars();

		_main_function = this->_script_function_factory->getScriptFunction(IDX_MAIN_FUNCTION);

		// clean main functions ... remove script functions and leave c functions...
		for (unsigned f = 0;
			f < _main_function->m_function.size()
			;) {
			// get function info
			CScriptFunction * local_function = _main_function->m_function[f];

			if ((local_function->symbol_info.properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF) {
				_main_function->m_function.erase(_main_function->m_function.begin() + f);
			}
			else {
				f++;
			}

		}

		// remove c variables ...
		for (unsigned v = 0;
			v < _main_function->m_variable.size(); ) {

			if ((_main_function->m_variable[v].properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF) {

				_main_function->m_variable.erase(_main_function->m_variable.begin() + v);

			}
			else {
				v++;
			}
		}

		// remove scope vars...
		_scope_factory->clear();
		_script_function_factory->clear();
		_script_class_factory->clear();

	}


	CZetScript::CZetScript(){

		_scope_factory = new CScopeFactory();
		_native_function_factory = new CNativeFunctionFactory();
		_script_function_factory= new CScriptFunctionFactory();
		_script_class_factory = new CScriptClassFactory();
		_eval = new CEval();

		_virtual_machine = new CVirtualMachine();

		_main_object=NULL;
		_main_function=NULL;

		_eval_int=0;
		_eval_float=0;
		_eval_string="";
		_eval_bool = false;

	}


	int * CZetScript::evalIntValue(const std::string & str_to_eval){


		if(!evalString(str_to_eval)){
			return NULL;
		}

		tStackElement *se=_virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_INTEGER){

				_eval_int=(int)((intptr_t)se->stkValue);
				return &_eval_int;
			}
			else{
				THROW_RUNTIME_ERROR(string::sformat("evalIntValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));
			}
		}

		return NULL;


	}

	bool * CZetScript::evalBoolValue(const std::string & str_to_eval){

		if(!evalString(str_to_eval)){
			return NULL;
		}

		tStackElement *se=_virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_BOOLEAN){
				_eval_bool=(bool)((intptr_t)se->stkValue);
				return &_eval_bool;

			}else{
				THROW_RUNTIME_ERROR(string::sformat("evalBoolValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));
			}
		}

		return NULL;
	}

	float * CZetScript::evalFloatValue(const std::string & str_to_eval){

		if(!evalString(str_to_eval)){
			return NULL;
		}

		tStackElement *se=_virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_NUMBER){
				_eval_float = *((float *)(&se->stkValue));
				return &_eval_float;
			}
			else{
				THROW_RUNTIME_ERROR(string::sformat("evalFloatValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));

			}
		}


		return NULL;
	}

	std::string * CZetScript::evalStringValue(const std::string & str_to_eval){


		if(!evalString(str_to_eval)){
			return NULL;
		}

		tStackElement *se=_virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_STRING){

				_eval_string = ((const char *)se->stkValue);
				return &_eval_string;
			}
			else{
				string::sformat("evalStringValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
			}
		}

		return NULL;
	}


	void CZetScript::execute(){

		bool error=false;

		// the first code to execute is the main function that in fact is a special member function inside our main class
		_virtual_machine->execute(_main_function, NULL,error,NO_PARAMS);

		if(error){
			THROW_SCRIPT_ERROR();
		}
	}

	bool CZetScript::evalString(const std::string & expression, bool exec_vm, const char *filename, bool show_bytecode)  {


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

	bool CZetScript::evalFile(const std::string & filename, bool exec_vm, bool show_bytecode){

		char *buf_tmp=NULL;

		bool status = false;


		if(!CEval::evalFile(filename)){
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


	CZetScript::~CZetScript(){

		_virtual_machine->clearGlobalVars();

		// clear objects...
		delete _scope_factory;
		delete _script_function_factory;
		delete _script_class_factory;
		delete _native_function_factory;
		delete _eval;

		_main_object = NULL;
		_main_function = NULL;


		_virtual_machine=NULL;

	}


}
