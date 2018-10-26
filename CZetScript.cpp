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
	vector<tInfoParsedSource> * CZetScript::m_parsedSource = NULL;


	int CZetScript::getIdxParsedFilenameSource(const char *file){

		if(file == NULL) return 0;

		for(unsigned i =0; i < m_parsedSource->size(); i++){
			if(m_parsedSource->at(i).filename == file){
				return i;
			}
		}

		THROW_RUNTIME_ERROR("Fatal error! Cannot find idx for \"%s\"",file);

		return -1;
	}

	void CZetScript::setVectorInfoParsedFiles(vector<tInfoParsedSource> * parsedFiles){
		m_parsedSource = parsedFiles;
	}

	CZetScript * CZetScript::getInstance(){
		if(m_instance==NULL){
			m_instance = new CZetScript();

			if(!m_instance->init()){
				THROW_RUNTIME_ERROR("Cannot instance ZetScript");
			}
		}
		return m_instance;
	}

	void CZetScript::destroy(){

		CURRENT_VM->clearGlobals();

		// clear globals...
		if(m_instance!=NULL){
			delete m_instance;
		}

		CCompiler::destroySingletons();
		CState::destroySingletons();
		CNativeFunction::destroySingletons();
		CScriptClass::destroySingletons();
		CASTNode::destroySingletons();
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	 // PRINT ASM INFO

	 const char * CZetScript::getStrMovVar(tInfoAsmOp * iao){

		if(iao->operator_type != STORE){
			return "ERROR";
		}

		string symbol_value="Unknown";

		if(iao->idxAstNode != -1){
			symbol_value = AST_SYMBOL_VALUE(iao->idxAstNode);
		}

		sprintf(print_aux_load_value,"VAR(%s)",symbol_value.c_str());

		return print_aux_load_value;
	 }

	 const char * CZetScript::getStrTypeLoadValue(PtrAsmOp m_listStatements, int current_instruction){

		tInfoAsmOp * iao =&m_listStatements[current_instruction];

		if(iao->operator_type != LOAD){
			return "ERROR";
		}

		string symbol_value="Unknown";

		if(iao->idxAstNode != -1){
			symbol_value = AST_SYMBOL_VALUE(iao->idxAstNode);
		}

		char object_access[512] = "";

		sprintf(print_aux_load_value,"UNDEFINED");

		if(iao->instruction_properties & INS_PROPERTY_ACCESS_SCOPE){

			sprintf(object_access,
					"[%03i]."

					,(int)iao->index_op2);
		}
		else if(iao->instruction_properties & INS_PROPERTY_THIS_SCOPE){
			sprintf(object_access,"this.");
		}

		switch(iao->index_op1){

			case LOAD_TYPE::LOAD_TYPE_CONSTANT:

				sprintf(print_aux_load_value,"CONST(%s)",symbol_value.c_str());
				break;

			case LOAD_TYPE::LOAD_TYPE_VARIABLE:
				sprintf(print_aux_load_value,"%sVAR(%s)",object_access,symbol_value.c_str());
				break;
			case LOAD_TYPE::LOAD_TYPE_FUNCTION:

				sprintf(print_aux_load_value,"%sFUN(%s)",object_access,symbol_value.c_str());
				break;

			case LOAD_TYPE::LOAD_TYPE_ARGUMENT:
				sprintf(print_aux_load_value,"ARG(%s)",symbol_value.c_str());
				break;
			default:

				break;
		}
		return print_aux_load_value;
	 }

	 void CZetScript::printGeneratedCode(CScriptFunctionObject *sfo){

		// PRE: it should printed after compile and updateReferences.
		string pre="";
		string post="";

		if(sfo->asm_op != NULL){

			unsigned idx_instruction=0;
			for(tInfoAsmOp * asm_op=sfo->asm_op; asm_op->operator_type!= END_FUNCTION; asm_op++,idx_instruction++){

				int n_ops=0;
				int index_op1 = asm_op->index_op1;
				int index_op2 = asm_op->index_op2;

				if(index_op1 != -1)
					n_ops++;

				 if(index_op2 != -1)
					 n_ops++;

				 pre="";
				 post="";

					switch(GET_INS_PROPERTY_PRE_POST_OP(asm_op->instruction_properties)){
					case INS_PROPERTY_PRE_NEG:
						pre="-";
						break;
					case INS_PROPERTY_PRE_INC:
						pre="++";
						break;
					case INS_PROPERTY_PRE_DEC:
						pre="--";
						break;
					case INS_PROPERTY_POST_INC:
						post="++";
						break;
					case INS_PROPERTY_POST_DEC:
						post="--";
						break;
					default:
						// check whether is constant and numeric
						if(asm_op->operator_type==ASM_OPERATOR::LOAD && asm_op->index_op1==LOAD_TYPE_CONSTANT){
							tInfoConstantValue *icv = (((tInfoConstantValue *)asm_op->index_op2));
							float n;

							// change the sign
							switch(icv->properties){
							default:
								break;
							case STK_PROPERTY_TYPE_INTEGER:
								if(((intptr_t)icv->stkValue)<0){
									pre="-";
								}
								break;
							case STK_PROPERTY_TYPE_NUMBER:
								memcpy(&n,&icv->stkValue,sizeof(float));
								if(n<0){
									pre="-";
								}
								break;
							}
						}
						break;

					}
				switch(asm_op->operator_type){

				case  NEW:
					printf("[%03i]\t%s\t%s\n",idx_instruction,CCompiler::def_operator[asm_op->operator_type].op_str,asm_op->index_op1!=ZS_INVALID_CLASS?CScriptClass::getNameRegisteredClassByIdx(asm_op->index_op1):"???");
					break;
				case  LOAD:
					printf("[%03i]\t%s\t%s%s%s\n"
							,idx_instruction,
							CCompiler::def_operator[asm_op->operator_type].op_str,
							pre.c_str(),
							getStrTypeLoadValue(sfo->asm_op,idx_instruction),
							post.c_str());
					break;
				case JNT:
				case JT:
				case JMP:
					printf("[%03i]\t%s\t%03i\n"
							,idx_instruction
							,CCompiler::def_operator[asm_op->operator_type].op_str
							,(int)asm_op->index_op2);
					break;
				case PUSH_SCOPE:
					printf("[%03i]\t%s%c%s%s%s%c\n"
							,idx_instruction
							,CCompiler::def_operator[asm_op->operator_type].op_str
							,asm_op->index_op1!=0?'(':' '
							,asm_op->index_op1&SCOPE_PROPERTY::BREAK?"BREAK":""
							,asm_op->index_op1&SCOPE_PROPERTY::CONTINUE?" CONTINUE":""
							,asm_op->index_op1&SCOPE_PROPERTY::FOR_IN?" FOR_IN":""
							,asm_op->index_op1!=0?')':' '
							);
					break;
				case POP_SCOPE:
					printf("[%03i]\t%s%c%s%s%s%c\n"
							,idx_instruction
							,CCompiler::def_operator[asm_op->operator_type].op_str
							,asm_op->index_op1!=0?'(':' '
							,asm_op->index_op1&SCOPE_PROPERTY::BREAK?"BREAK":""
							,asm_op->index_op1&SCOPE_PROPERTY::CONTINUE?" CONTINUE":""
							,asm_op->index_op1&SCOPE_PROPERTY::FOR_IN?" FOR_IN":""
							,asm_op->index_op1!=0?')':' '
							);
					break;
				default:

					if(n_ops==0){
						printf("[%03i]\t%s\n",idx_instruction,CCompiler::def_operator[asm_op->operator_type].op_str);
					}else if(n_ops==1){
						printf("[%03i]\t%s%s\n"
								,idx_instruction
								,CCompiler::def_operator[asm_op->operator_type].op_str
								,(asm_op->instruction_properties & STK_PROPERTY_READ_TWO_POP_ONE)?"_CS":""
								);
					}else{
						printf("[%03i]\t%s\n"
								,idx_instruction
								,CCompiler::def_operator[asm_op->operator_type].op_str
								);
					}
					break;
				}
			}
		}

		// and then print its functions ...
		vector<int> * m_vf = &sfo->scope_info.local_symbols.vec_idx_registeredFunction;

		for(unsigned j =0; j < m_vf->size(); j++){

			CScriptFunctionObject *local_irfs = GET_SCRIPT_FUNCTION_OBJECT((*m_vf)[j]);

			if(( local_irfs->symbol_info.properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF){
				char symbol_ref[1024*8]={0};

				strcpy(symbol_ref,AST_SYMBOL_VALUE_CONST_CHAR(local_irfs->symbol_info.idxAstNode));

				if(local_irfs->symbol_info.idxScriptClass!=ZS_INVALID_CLASS){
					CScriptClass *sc = CScriptClass::getScriptClassByIdx(local_irfs->symbol_info.idxScriptClass);
					if(sc->symbol_info.idxScriptClass == IDX_CLASS_MAIN){
						sprintf(symbol_ref,"Main");
					}else{
						sprintf(symbol_ref,"%s::%s",sfo->symbol_info.symbol_ref.c_str(),AST_SYMBOL_VALUE_CONST_CHAR(local_irfs->symbol_info.idxAstNode));
					}
				}

				printf("-------------------------------------------------------\n");
				printf("\nCode for function \"%s\"\n\n",symbol_ref);
				printGeneratedCode(GET_SCRIPT_FUNCTION_OBJECT(m_vf->at(j)));
			}
		}
	 }


	 void CZetScript::printGeneratedCodeAllClasses(){

		 vector<CScriptClass *> * registeredClass = CScriptClass::getVectorScriptClassNode();

		 // for all classes print code...
		 for(unsigned i = 0; i < registeredClass->size(); i++){
			 CScriptClass *rc=registeredClass->at(i);
			 for(unsigned f = 0; f < rc->scope_info.local_symbols.vec_idx_registeredFunction.size(); f++){
				 printGeneratedCode(GET_SCRIPT_FUNCTION_OBJECT(rc->scope_info.local_symbols.vec_idx_registeredFunction[f]));
			 }
		 }
	 }
	 // PRINT ASM INFO
	 //----------------------------------------------------------------------------------------------------------------------------------------------------------------

	CZetScript::CZetScript(){
		//idxMainScriptFunctionObject=ZS_UNDEFINED_IDX;
		m_mainObject = NULL;
		show_filename_on_error=true;
		__init__ = false;

		vm=NULL;
	}



	bool CZetScript::init(){

		CState::init();

		vm = new CVirtualMachine();

		// ok register CInteger through CScriptVariable...
		//if((m_mainClassInfo = CScriptClass::getScriptClassByName(MAIN_SCRIPT_CLASS_NAME)) == NULL) return false;
		if((CScriptClass::getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_OBJECT_NAME)) == ZS_UNDEFINED_IDX) return false;

		__init__=true;
		// create compiler instance ...
		CCompiler::getInstance();

		return true;
	}

	void CZetScript::parse_ast(const char   * s, int idx_filename){
		int m_line = 1;
		char *end;
		bool error=false;
		PASTNode main_node = MAIN_AST_NODE;
		const char *filename=NULL;

		if(idx_filename>=0){
			filename=m_parsedSource->at(idx_filename).filename.c_str();
		}

		SET_PARSING_FILENAME(idx_filename,filename);


		/*if((end=CASTNode::generateAST_Recursive(
				s,
				m_line,
				MAIN_SCOPE_NODE,
				error,
				main_node
				)) == NULL){
			return;
		}*/

		CEval::eval(s);


		if(*end != 0){
			writeErrorMsg(filename,m_line,"Unexpected \'%c\' ",*end);
			return THROW_SCRIPT_ERROR();

		}

	}

	bool CZetScript::isFilenameAlreadyParsed(const char * filename){
		for(unsigned i = 0; i < m_parsedSource->size(); i++){
			if(m_parsedSource->at(i).filename==filename){
				return true;
			}
		}
		return false;
	}
	
	const char * CZetScript::getParsedFilenameFromIdx(unsigned idx){

		if(idx >= m_parsedSource->size()){
			THROW_RUNTIME_ERROR("out of bounds");
			return DEFAULT_NO_FILENAME;
		}

		return m_parsedSource->at(idx).filename.c_str();
	}

	int CZetScript::eval_int(const string & str_to_eval){
		CZetScript *zetscript= CZetScript::getInstance();

		try{
			zetscript->eval(str_to_eval);
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
			zetscript->eval(str_to_eval);
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
			zetscript->eval(str_to_eval);
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
			zetscript->eval(str_to_eval);
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

	void CZetScript::destroyMainFunction() {

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

	}

	void CZetScript::execute(){

		if(!__init__) {THROW_RUNTIME_ERROR ("ZetScript not initialized"); return;}
		//ZS_CLEAR_ERROR_MSG();

		bool error=false;

		// the first code to execute is the main function that in fact is a special member function inside our main class
		vm->execute(MAIN_SCRIPT_FUNCTION_OBJECT, m_mainObject,error,NO_PARAMS);

		if(error){
			THROW_SCRIPT_ERROR();
		}


	}

	void CZetScript::eval(const string & s, bool exec_vm, const char *filename_ref, bool show_bytecode)  {


		parse(s,filename_ref);
		//compile(show_bytecode);
		//if(exec_vm){
		//	execute();
		//}
	}

	void CZetScript::eval_file(const char * filename, bool execute, bool show_bytecode){

		//ZS_CLEAR_ERROR_MSG();

		bool status = false;

		char *buf_tmp=NULL;
		int n_bytes;

		if((buf_tmp=CZetScriptUtils::readFile(filename, n_bytes))!=NULL){

			try{
				eval((char *)buf_tmp, execute, filename,show_bytecode);
			}catch(script_error & error){
				free(buf_tmp);
				THROW_EXCEPTION(error);
			}

			free(buf_tmp);
		}

	}

	//CScriptFunctionObject *getScriptObjectFromScriptFunctionAccessName(const string &function_access_expression)
	bool CZetScript::getScriptObjectFromFunctionAccess(const string &function_access,CScriptVariable **calling_obj,CScriptFunctionObject **fun_obj ){

		//ZS_CLEAR_ERROR_MSG();

		vector<string> access_var = CZetScriptUtils::split(function_access,'.');
		CScriptFunctionObject * m_mainFunctionInfo = MAIN_SCRIPT_FUNCTION_OBJECT;

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
					symbol_to_find=CCompiler::makeSymbolRef(symbol_to_find,IDX_GLOBAL_SCOPE);
					for(unsigned j = 0; j < m_mainFunctionInfo->scope_info.local_symbols.m_registeredVariable.size() && *calling_obj==NULL; j++){
						if(m_mainFunctionInfo->scope_info.local_symbols.m_registeredVariable[j].symbol_ref==symbol_to_find){
							tStackElement *stk = CURRENT_VM->getStackElement(j); // m_mainFunctionInfo->object_info.local_symbols.m_registeredVariable[j].
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
					*fun_obj=(CScriptFunctionObject *)is->object.stkValue;
				}
			}else{

				THROW_RUNTIME_ERROR("error evaluating \"%s\". Cannot find function \"%s\"",function_access.c_str(),access_var[access_var.size()-1].c_str());
				return false;
			}

		}else{ // function
			*calling_obj = m_mainObject;
			string symbol_to_find=CCompiler::makeSymbolRef(access_var[0],IDX_GLOBAL_SCOPE);
			for(unsigned i = 0; i < m_mainFunctionInfo->scope_info.local_symbols.vec_idx_registeredFunction.size() && *fun_obj==NULL; i++){
				CScriptFunctionObject *aux_fun_obj=GET_SCRIPT_FUNCTION_OBJECT(m_mainFunctionInfo->scope_info.local_symbols.vec_idx_registeredFunction[i]);
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

	CVirtualMachine * CZetScript::getVirtualMachine(){
		return vm;
	}

	//-------------------------------------------------------------------------------------
	CZetScript::~CZetScript(){
		// unregister operators ...
		destroyMainFunction();
		delete vm;
	}
}
