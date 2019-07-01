#include "zetscript.h"

namespace zetscript{

	// define prototype ...
	int error_line;
	char error_filename[512];
	char error_description[1024];
	const char *error_type="NA";



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

	CZetScript * CZetScript::m_instance = NULL;

	CZetScript * CZetScript::getInstance(){
		if(m_instance==NULL){
			m_instance = new CZetScript();

			if(!m_instance->init()){
				CZetScriptUtils::sformat("Cannot instance ZetScript");
			}
		}
		return m_instance;
	}

	void CZetScript::clear(){

		CURRENT_VM->clearGlobalVars();

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

	void CZetScript::destroy(){

		CURRENT_VM->clearGlobalVars();

		// clear globals...
		if(m_instance!=NULL){
			delete m_instance;
		}
		
		CScopeFactory::destroySingleton();
		CScriptFunctionFactory::destroySingleton();
		CScriptClassFactory::destroySingleton();
		CNativeFunction::destroySingleton();
		CEval::destroySingleton();
	}


	CZetScript::CZetScript(){
		m_mainObject = NULL;
		m_mainFunction = NULL;
		show_filename_on_error=true;
		__init__ = false;
		vm=NULL;
	}

	bool CZetScript::init(){

		CScopeFactory::getInstance();
		CNativeFunction::getInstance();
		CScriptFunctionFactory::getInstance();
		CScriptClassFactory::getInstance();
		CEval::getInstance();

		vm = new CVirtualMachine();

		__init__=true;

		return true;
	}


	int CZetScript::eval_int(const std::string & str_to_eval){
		CZetScript *zetscript= CZetScript::getInstance();

		try{
			zetscript->evalString(str_to_eval);
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
				THROW_RUNTIME_ERROR(CZetScriptUtils::sformat("eval_int(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));
			}
		}

		return 0;


	}

	bool CZetScript::eval_bool(const std::string & str_to_eval){

		CZetScript *zetscript= CZetScript::getInstance();

		try{
			zetscript->evalString(str_to_eval);
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
				THROW_RUNTIME_ERROR(CZetScriptUtils::sformat("eval_bool(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));
			}
		}

		return false;
	}

	float CZetScript::eval_float(const std::string & str_to_eval){

		CZetScript *zetscript= CZetScript::getInstance();

		try{
			zetscript->evalString(str_to_eval);
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
				CZetScriptUtils::sformat("eval_float(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);

			}
		}


		return 0.0f;
	}

	std::string CZetScript::eval_string(const std::string & str_to_eval){

		std::string value="---";

		CZetScript *zetscript= CZetScript::getInstance();

		try{
			zetscript->evalString(str_to_eval);
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
				CZetScriptUtils::sformat("eval_string(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
			}
		}

		return value;
	}


	void CZetScript::execute(){

		if(!__init__) {THROW_RUNTIME_ERROR ("ZetScript not initialized"); return;}
		//ZS_CLEAR_ERROR_MSG();

		bool error=false;

		// the first code to execute is the main function that in fact is a special member function inside our main class
		vm->execute(MAIN_FUNCTION, NULL,error,NO_PARAMS);

		if(error){
			THROW_SCRIPT_ERROR();
		}
	}

	bool CZetScript::evalString(const std::string & expression, bool exec_vm, const char *filename, bool show_bytecode)  {


		if(!__init__) {CZetScriptUtils::sformat("zetscript not initialized");return false;}

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

		if(!__init__) {CZetScriptUtils::sformat("zetscript not initialized");return false;}

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

	//CScriptFunction *getScriptObjectFromScriptFunctionAccessName(const std::string &function_access_expression)
	bool CZetScript::getScriptObjectFromFunctionAccess(const std::string &function_access,CScriptVariable **calling_obj,CScriptFunction **fun_obj ){

		//ZS_CLEAR_ERROR_MSG();

		std::vector<std::string> access_var = CZetScriptUtils::split(function_access,'.');
		CScriptFunction * m_mainFunctionInfo = MAIN_FUNCTION;

		if(m_mainFunctionInfo == NULL){
			CZetScriptUtils::sformat("m_mainFunctionInfo is not initialized");
			return false;
		}

		*calling_obj = NULL;
		tFunctionSymbol *is=NULL;
		tStackElement *se=NULL;
		*fun_obj=NULL;

		// 1. some variable in main function ...
		if(access_var.size()>1){
			for(unsigned i=0; i < access_var.size()-1; i++){

				std::string symbol_to_find=access_var[i];
				if(i==0){ // get variable through main_class.main_function (global element)
					//symbol_to_find= CEval::makeSymbolRef(symbol_to_find,IDX_GLOBAL_SCOPE);
					for(unsigned j = 0; j < m_mainFunctionInfo->m_variable.size() && *calling_obj==NULL; j++){
						if(m_mainFunctionInfo->m_variable[j].symbol->name==symbol_to_find
						&& m_mainFunctionInfo->m_variable[j].symbol->idxScope == IDX_GLOBAL_SCOPE){
							tStackElement *stk = CURRENT_VM->getStackElement(j); // m_mainFunctionInfo->object_info.local_symbols.variable[j].
							if(stk!=NULL){
								if(stk->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
									*calling_obj=(CScriptVariable *)stk->varRef;
								}
							}
							else{
								CZetScriptUtils::sformat("cannot access i (%i)",j);
								return false;
							}
						}
					}

					if((*calling_obj) == NULL){
						CZetScriptUtils::sformat("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
						return false;
					}

				}else{ // we have got the calling_obj from last iteration ...
					se = (*calling_obj)->getVariableSymbol(symbol_to_find);

					if(se!=NULL){

						if(se->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
							*calling_obj=(CScriptVariable *)se->varRef;
						}else{
							CZetScriptUtils::sformat("error evaluating \"%s\". Variable name \"%s\" not script variable",function_access.c_str(),symbol_to_find.c_str());
							return false;
						}
					}
					else{
						CZetScriptUtils::sformat("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
						return false;
					}
				}
			}

			is=(*calling_obj)->getFunctionSymbol(access_var[access_var.size()-1]);
			if(is!=NULL){
				if(is->object.properties & STK_PROPERTY_TYPE_FUNCTION){
					*fun_obj=(CScriptFunction *)is->object.stkValue;
				}
			}else{

				CZetScriptUtils::sformat("error evaluating \"%s\". Cannot find function \"%s\"",function_access.c_str(),access_var[access_var.size()-1].c_str());
				return false;
			}

		}else{ // some function in main function
			//*calling_obj = m_mainObject;
			std::string symbol_to_find=access_var[0];
			for(unsigned i = 0; i < m_mainFunctionInfo->m_function.size() && *fun_obj==NULL; i++){
				CScriptFunction *aux_fun_obj=m_mainFunctionInfo->m_function[i];
				if(		aux_fun_obj->symbol_info.symbol->name  == symbol_to_find
				  && aux_fun_obj->symbol_info.symbol->idxScope == IDX_GLOBAL_SCOPE){
					*fun_obj=aux_fun_obj;
				}
			}
		}

		if(*fun_obj==NULL){
			THROW_RUNTIME_ERROR(CZetScriptUtils::sformat("error evaluating \"%s\". Variable name \"%s\" is not function type",function_access.c_str(),access_var[access_var.size()-1].c_str()));
			return false;
		}

		return true;
	}

	CVirtualMachine * CZetScript::getVirtualMachine(){
		return vm;
	}

	//-------------------------------------------------------------------------------------
	CZetScript::~CZetScript(){

		delete vm;
	}
}
