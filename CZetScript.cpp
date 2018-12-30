#include "CZetScript.h"

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
	//char CZetScript::str_error[MAX_BUFFER_STR_ERROR] = { 0 };


	CZetScript * CZetScript::getInstance(){
		if(m_instance==NULL){
			m_instance = new CZetScript();

			if(!m_instance->init()){
				THROW_RUNTIME_ERROR("Cannot instance ZetScript");
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


		//int i = vec_script_function_node->size()-1;

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

		//CCompiler::destroySingletons();
		//CState::destroySingletons();
		
		CScopeFactory::destroySingleton();
		CScriptFunctionFactory::destroySingleton();
		CScriptClassFactory::destroySingleton();
		CNativeFunction::destroySingleton();
		CEval::destroySingleton();
		//CASTNode::destroySingletons();
	}


	CZetScript::CZetScript(){
		//idxMainScriptFunctionObject=ZS_UNDEFINED_IDX;
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

		//m_mainObject = CScriptClass::instanceScriptVariableByClassName(MAIN_SCRIPT_CLASS_NAME);//new CScriptVariable(&m_structInfoMain);//CScriptClass::instanceScriptVariableByClassName("Main");
		//m_mainFunction = MAIN_FUNCTION;

		vm = new CVirtualMachine();

		// ok register CInteger through CScriptVariable...
		//if((m_mainClassInfo = CScriptClass::getScriptClassByName(MAIN_SCRIPT_CLASS_NAME)) == NULL) return false;
		//if(GET_SCRIPT_FUNCTION() (CScriptClass::ge getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_OBJECT_NAME)) == ZS_UNDEFINED_IDX) return false;

		__init__=true;
		// create compiler instance ...
		//CCompiler::getInstance();

		return true;
	}

	/*void CZetScript::parse_ast(const char   * s, int idx_filename){
		int m_line = 1;
		char *end;
		bool error=false;
		//PASTNode main_node = MAIN_AST_NODE;
		const char *filename=NULL;

		if(idx_filename>=0){
			filename=m_parsedSource->at(idx_filename).filename.c_str();
		}

		SET_PARSING_FILENAME(idx_filename,filename);


		if((end=CASTNode::generateAST_Recursive(
				s,
				m_line,
				MAIN_SCOPE_NODE,
				error,
				main_node
				)) == NULL){
			return;
		}




		if(*end != 0){
			writeErrorMsg(filename,m_line,"Unexpected \'%c\' ",*end);
			return THROW_SCRIPT_ERROR();

		}

	}*/



	int CZetScript::eval_int(const string & str_to_eval){
		CZetScript *zetscript= CZetScript::getInstance();

		try{
			zetscript->evalString(str_to_eval);
		}
		catch(script_error & error){
			THROW_EXCEPTION(error);
		}

		tStackElement *se=CURRENT_VM->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_INTEGER){

				return (int)((intptr_t)se->stkValue);
			}
			else{
				THROW_RUNTIME_ERROR("eval_int(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
			}
		}

		return 0;


	}

	bool CZetScript::eval_bool(const string & str_to_eval){

		CZetScript *zetscript= CZetScript::getInstance();

		try{
			zetscript->evalString(str_to_eval);
		}
		catch(script_error & error){
			THROW_EXCEPTION(error);
			return false;
		}

		tStackElement *se=CURRENT_VM->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_BOOLEAN){
				return (bool)((intptr_t)se->stkValue);

			}else{
				THROW_RUNTIME_ERROR("eval_bool(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
			}
		}

		return false;
	}

	float CZetScript::eval_float(const string & str_to_eval){

		CZetScript *zetscript= CZetScript::getInstance();

		try{
			zetscript->evalString(str_to_eval);
		}
		catch(script_error & error){
			THROW_EXCEPTION(error);
		}

		tStackElement *se=CURRENT_VM->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_NUMBER){
				float *f = ((float *)(&se->stkValue));
				return *f;
			}
			else{
				THROW_RUNTIME_ERROR("eval_float(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);

			}
		}


		return 0.0f;
	}

	string CZetScript::eval_string(const string & str_to_eval){

		string value="---";

		CZetScript *zetscript= CZetScript::getInstance();

		try{
			zetscript->evalString(str_to_eval);
		}
		catch(script_error & error){
			THROW_EXCEPTION(error);
			return "";
		}

		tStackElement *se=CURRENT_VM->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_STRING){

				value = ((const char *)se->stkValue);
			}
			else{
				THROW_RUNTIME_ERROR("eval_string(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
			}
		}

		return value;
	}

	/*void CZetScript::destroyMainFunction() {

		if (m_mainObject != NULL) {
			delete m_mainObject;
		}
		m_mainObject = NULL;
	}

	
	ZETSCRIPT_MODULE_EXPORT void CZetScript::parse(const string & str_script,const char *filename_ref){
		if(!__init__) {THROW_RUNTIME_ERROR("zetscript not initialized");return;}

		int idx_file=-1;

		if(filename_ref != NULL){
			if(!isFilenameAlreadyParsed(filename_ref)){
				tInfoParsedSource ps;
				ps.filename = filename_ref;
				m_parsedSource->push_back(ps);
				idx_file=m_parsedSource->size()-1;
			}else{
				// already parsed
				return;
			}
		}

		//ZS_CLEAR_ERROR_MSG();
		parse_ast(str_script.c_str(),idx_file);
	}

	void CZetScript::parse_file(const char * filename){

		//ZS_CLEAR_ERROR_MSG();

		bool status = false;
		char *buf_tmp=NULL;
		int n_bytes;

		if((buf_tmp=CZetScriptUtils::readFile(filename, n_bytes))!=NULL){


			try{
				parse((char *)buf_tmp, filename);
			}
			catch(script_error & error){
				free(buf_tmp);
				THROW_EXCEPTION(error);
			}

			free(buf_tmp);
		}

	}

	ZETSCRIPT_MODULE_EXPORT void CZetScript::compile(bool show_bytecode){
		if(!__init__) {THROW_RUNTIME_ERROR("ZetScript not initialized"); return;}
		//ZS_CLEAR_ERROR_MSG();

		if(CCompiler::getInstance()->compile()){

			// print generated asm ...
			if(show_bytecode){
				printGeneratedCodeAllClasses();
			}

			if(m_mainObject == NULL){
				// creates the main entry function with compiled code. On every executing code, within "execute" function
				// virtual machine is un charge of allocating space for all local variables...
				m_mainObject = CScriptClass::instanceScriptVariableByClassName(MAIN_SCRIPT_CLASS_NAME);//new CScriptVariable(&m_structInfoMain);//CScriptClass::instanceScriptVariableByClassName("Main");
			}

		}
		else THROW_SCRIPT_ERROR();

	}*/

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

	bool CZetScript::evalString(const string & expression, bool exec_vm, const char *filename, bool show_bytecode)  {


		if(!__init__) {THROW_RUNTIME_ERROR("zetscript not initialized");return false;}



		if(!CEval::evalString(expression)){
			return false;
		}


		if(show_bytecode){
			SCRIPT_CLASS_FACTORY->printGeneratedCode();
		}

		if(exec_vm){
			execute();
		}

		return true;
	}

	bool CZetScript::evalFile(const string & filename, bool exec_vm, bool show_bytecode){

		if(!__init__) {THROW_RUNTIME_ERROR("zetscript not initialized");return false;}

		char *buf_tmp=NULL;
//		int n_bytes;


		bool status = false;



		try{
			if(!CEval::evalFile(filename)){
				return false;
			}
		}catch(script_error & e){
			THROW_EXCEPTION(e);
			return false;
		}

		if(show_bytecode){
			SCRIPT_CLASS_FACTORY->printGeneratedCode();
		}

		if(exec_vm){
			execute();
		}

		return true;

	}

	//CScriptFunction *getScriptObjectFromScriptFunctionAccessName(const string &function_access_expression)
	bool CZetScript::getScriptObjectFromFunctionAccess(const string &function_access,CScriptVariable **calling_obj,CScriptFunction **fun_obj ){

		//ZS_CLEAR_ERROR_MSG();

		vector<string> access_var = CZetScriptUtils::split(function_access,'.');
		CScriptFunction * m_mainFunctionInfo = MAIN_FUNCTION;

		if(m_mainFunctionInfo == NULL){
			THROW_RUNTIME_ERROR("m_mainFunctionInfo is not initialized");
			return false;
		}

		*calling_obj = NULL;
		tFunctionSymbol *is=NULL;
		tStackElement *se=NULL;
		*fun_obj=NULL;

		// 1. accessing var scopes...
		if(access_var.size()>1){
			for(unsigned i=0; i < access_var.size()-1; i++){

				string symbol_to_find=access_var[i];
				if(i==0){ // get variable through main_class.main_function (global element)
					symbol_to_find=CEval::makeSymbolRef(symbol_to_find,IDX_GLOBAL_SCOPE);
					for(unsigned j = 0; j < m_mainFunctionInfo->m_variable.size() && *calling_obj==NULL; j++){
						if(m_mainFunctionInfo->m_variable[j].symbol_ref==symbol_to_find){
							tStackElement *stk = CURRENT_VM->getStackElement(j); // m_mainFunctionInfo->object_info.local_symbols.variable[j].
							if(stk!=NULL){
								if(stk->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
									*calling_obj=(CScriptVariable *)stk->varRef;
								}
							}
							else{
								THROW_RUNTIME_ERROR("cannot access i (%i)",j);
								return false;
							}
						}
					}

					if((*calling_obj) == NULL){
						THROW_RUNTIME_ERROR("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
						return false;
					}

				}else{ // we have got the calling_obj from last iteration ...
					se = (*calling_obj)->getVariableSymbol(symbol_to_find,true);

					if(se!=NULL){

						if(se->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
							*calling_obj=(CScriptVariable *)se->varRef;
						}else{
							THROW_RUNTIME_ERROR("error evaluating \"%s\". Variable name \"%s\" not script variable",function_access.c_str(),symbol_to_find.c_str());
							return false;
						}
					}
					else{
						THROW_RUNTIME_ERROR("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
						return false;
					}
				}
			}

			is=(*calling_obj)->getFunctionSymbol(access_var[access_var.size()-1],true);
			if(is!=NULL){
				if(is->object.properties & STK_PROPERTY_TYPE_FUNCTION){
					*fun_obj=(CScriptFunction *)is->object.stkValue;
				}
			}else{

				THROW_RUNTIME_ERROR("error evaluating \"%s\". Cannot find function \"%s\"",function_access.c_str(),access_var[access_var.size()-1].c_str());
				return false;
			}

		}else{ // function
			//*calling_obj = m_mainObject;
			string symbol_to_find=CEval::makeSymbolRef(access_var[0],IDX_GLOBAL_SCOPE);
			for(unsigned i = 0; i < m_mainFunctionInfo->m_function.size() && *fun_obj==NULL; i++){
				CScriptFunction *aux_fun_obj=m_mainFunctionInfo->m_function[i];
				if(aux_fun_obj->symbol_info.symbol_ref == symbol_to_find){
					*fun_obj=aux_fun_obj;
				}
			}
		}

		if(*fun_obj==NULL){
			THROW_RUNTIME_ERROR("error evaluating \"%s\". Variable name \"%s\" is not function type",function_access.c_str(),access_var[access_var.size()-1].c_str());
			return false;
		}

		return true;
	}

/*
	int CZetScript::newGlobalFunction( const string & function_name, vector<tArgumentInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short properties=0){

	}

	int CZetScript::getGlobalFunction(const string & function_ref,char n_args=0){

	}

	int CZetScript::newGlobalVariable(const string & variable,const string & variable_ref, const string & c_type="", intptr_t ref_ptr=0, unsigned short properties=0){

	}

	int CZetScript::getGlobalVariable(const string & variable_ref){

	}
*/
	CVirtualMachine * CZetScript::getVirtualMachine(){
		return vm;
	}

	//-------------------------------------------------------------------------------------
	CZetScript::~CZetScript(){
		// unregister operators ...
	/*	if (m_mainObject != NULL) {
			delete m_mainObject;
		}
		m_mainObject = NULL;*/

		delete vm;
	}
}
