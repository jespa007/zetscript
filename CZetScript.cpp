#include "CZetScript.h"

namespace zetscript{

	// define prototype ...
	int error_line;
	char error_filename[512];
	char error_description[512];
	const char *error_type="NA";
	bool assigned_error=false;


	void  writeErrorMsg(const char *filename, int line, const  char  *string_text, ...);
	int getErrorLine();
	const char * getErrorDescription();
	const char * getErrorFilename();

	CZetScript * CZetScript::m_instance = NULL;
	//char CZetScript::str_error[MAX_BUFFER_STR_ERROR] = { 0 };
	vector<tInfoParsedSource> * CZetScript::m_parsedSource = NULL;
	tPrintFunctionCallback print_error_callback=NULL;

	int CZetScript::getIdxParsedFilenameSource(const char *file){

		if(file == NULL) return 0;

		for(unsigned i =0; i < m_parsedSource->size(); i++){
			if(m_parsedSource->at(i).filename == file){
				return i;
			}
		}

		zs_print_error_cr("Fatal error! Cannot find idx for \"%s\"",file);

		return -1;
	}

	void CZetScript::setVectorInfoParsedFiles(vector<tInfoParsedSource> * parsedFiles){
		m_parsedSource = parsedFiles;
	}

	CZetScript * CZetScript::getInstance(){
		if(m_instance==NULL){
			m_instance = new CZetScript();

			if(!m_instance->init()){
				exit(-1);
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

	/*void  CZetScript::clearErrorMsg(){
		memset(str_error, 0,sizeof(str_error));
	}*/

	void	CZetScript::setUserCallbackOnError(tPrintFunctionCallback _fun){
		print_error_callback=_fun;
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

	 const char * CZetScript::getStrTypeLoadValue(PtrStatment m_listStatements,int current_statment, int current_instruction){

		tInfoAsmOp * iao =&m_listStatements[current_statment][current_instruction];

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
					"[%02i:"
					 "%02i"
					"]."
					,current_statment
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

	 void CZetScript::printGeneratedCode_Recursive(tFunctionInfo *fs){

		// PRE: it should printed after compile and updateReferences.

		//tInfoStatementOp * m_listStatements = fs->statment_op;
		string pre="";
		string post="";
		unsigned idx_statment=0;

		if(fs->statment_op != NULL){


			for(PtrStatment stat = fs->statment_op; *stat != NULL; stat++,idx_statment++){


				//printf("\n[%s]\n\n","file.zs");
				unsigned idx_instruction=0;
				for(tInfoAsmOp * asm_op_statment=*stat; asm_op_statment->operator_type!= END_STATMENT; asm_op_statment++,idx_instruction++){

					int n_ops=0;
					int index_op1 = asm_op_statment->index_op1;
					int index_op2 = asm_op_statment->index_op2;

					if(index_op1 != -1)
						n_ops++;

					 if(index_op2 != -1)
						 n_ops++;

					 pre="";
					 post="";

						switch(GET_INS_PROPERTY_PRE_POST_OP(asm_op_statment->instruction_properties)){
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
							if(asm_op_statment->operator_type==ASM_OPERATOR::LOAD && asm_op_statment->index_op1==LOAD_TYPE_CONSTANT){
								CCompiler::tInfoConstantValue *icv = (((CCompiler::tInfoConstantValue *)asm_op_statment->index_op2));
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
					switch(asm_op_statment->operator_type){
					case  PUSH_ATTR:

						printf("[%02i:%02i]\t%s\tSTRUCT[%02i:%02i],[%02i:%02i],[%02i:"
								"%02i]"
								"\n"
								,
								idx_statment,idx_instruction,
								CCompiler::def_operator[asm_op_statment->operator_type].op_str,

								idx_statment,asm_op_statment->index_op1,
								idx_statment,idx_instruction-1, // lat operand must be a string constant ...
								(int)idx_statment
								,(int)asm_op_statment->index_op2);
						break;

					case  NEW:
						printf("[%02i:%02i]\t%s\t%s\n",idx_statment,idx_instruction,CCompiler::def_operator[asm_op_statment->operator_type].op_str,CScriptClass::getNameRegisteredClassByIdx(asm_op_statment->index_op1));
						break;
					case  LOAD:
						printf("[%02i:%02i]\t%s\t%s%s%s --- %i\n"
								,idx_statment
								,idx_instruction,
								CCompiler::def_operator[asm_op_statment->operator_type].op_str,
								pre.c_str(),
								getStrTypeLoadValue(fs->statment_op,idx_statment,idx_instruction),
								post.c_str(),
								(int)asm_op_statment->index_op2);
						break;
					case POP_SCOPE:
						printf("[%02i:%02i]\t%s(%i)\n",idx_statment,idx_instruction,CCompiler::def_operator[asm_op_statment->operator_type].op_str,asm_op_statment->index_op1);
						break;
					case JNT:
					case JT:
					case JMP:
						printf("[%02i:%02i]\t%s\t[%04i:"
								"%04i"
								"]\n"
								,idx_statment
								,idx_instruction
								,CCompiler::def_operator[asm_op_statment->operator_type].op_str,asm_op_statment->index_op1
								,(int)asm_op_statment->index_op2);
						break;
					case VGET:
					case VPUSH:
						printf("[%02i:%02i]\t%s\t%sVEC[%02i:%02i]%s,[%02i:%02i]\n"
								,idx_statment
								,idx_instruction
								,CCompiler::def_operator[asm_op_statment->operator_type].op_str,pre.c_str()
								,idx_statment
								,index_op1
								,post.c_str()
								,idx_statment
								,index_op2);
						break;
					default:

						if(n_ops==0){
							printf("[%02i:%02i]\t%s\n",idx_statment,idx_instruction,CCompiler::def_operator[asm_op_statment->operator_type].op_str);
						}else if(n_ops==1){
							printf("[%02i:%02i]\t%s%s\t[%02i:%02i]\n"
									,idx_statment
									,idx_instruction
									,CCompiler::def_operator[asm_op_statment->operator_type].op_str
									,(asm_op_statment->instruction_properties & STK_PROPERTY_READ_TWO_POP_ONE)?"_CS":""
									,idx_statment
									,index_op1);
						}else{
							printf("[%02i:%02i]\t%s\t[%02i:%02i],[%02i:%02i]\n"
									,idx_statment
									,idx_instruction
									,CCompiler::def_operator[asm_op_statment->operator_type].op_str
									,idx_statment
									,index_op1
									,idx_statment
									,index_op2);
						}
						break;
					}
				}
			}
		}

		// and then print its functions ...
		vector<int> * m_vf = &fs->local_symbols.vec_idx_registeredFunction;

		for(unsigned j =0; j < m_vf->size(); j++){

			CScriptFunctionObject *local_irfs = GET_SCRIPT_FUNCTION_OBJECT((*m_vf)[j]);

			if(( local_irfs->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF){
				char symbol_ref[1024*8]={0};

				strcpy(symbol_ref,AST_SYMBOL_VALUE_CONST_CHAR(local_irfs->object_info.symbol_info.idxAstNode));

				if(local_irfs->object_info.symbol_info.idxScriptClass!=ZS_UNDEFINED_IDX){
					CScriptClass *sc = CScriptClass::getScriptClassByIdx(local_irfs->object_info.symbol_info.idxScriptClass);
					if(sc->metadata_info.object_info.symbol_info.idxScriptClass != IDX_CLASS_MAIN){
						sprintf(symbol_ref,"%s::%s",fs->symbol_info.symbol_name.c_str(),AST_SYMBOL_VALUE_CONST_CHAR(local_irfs->object_info.symbol_info.idxAstNode));
					}
				}

				printf("-------------------------------------------------------\n");
				printf("\nCode for function \"%s\"\n\n",symbol_ref);
				printGeneratedCode_Recursive(GET_FUNCTION_INFO(m_vf->at(j)));
			}
		}
	 }

	 void CZetScript::printGeneratedCode(tFunctionInfo *fs){
		printGeneratedCode_Recursive(fs);
	 }

	 void CZetScript::printGeneratedCodeAllClasses(){

		 vector<CScriptClass *> * registeredClass = CScriptClass::getVectorScriptClassNode();

		 // for all classes print code...
		 for(unsigned i = 0; i < registeredClass->size(); i++){
			 printGeneratedCode(&GET_SCRIPT_CLASS_INFO(registeredClass->at(i)->metadata_info.object_info.symbol_info.idxScriptClass)->metadata_info.object_info);
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

		return true;
	}

	void CZetScript::parse_ast(const char   * s, int idx_filename){
		int m_line = 1;
		bool error=false;
		PASTNode main_node = MAIN_AST_NODE;
		const char *filename=NULL;

		if(idx_filename>=0){
			filename=m_parsedSource->at(idx_filename).filename.c_str();
		}

		SET_PARSING_FILENAME(idx_filename,filename);

		if(CASTNode::generateAST_Recursive(
				s,
				m_line,
				MAIN_SCOPE_NODE,
				error,
				&main_node,
				false) == NULL){
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
			zs_print_error_cr("out of bounds");
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
			throw error;
		}

		tStackElement *se=CURRENT_VM->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_INTEGER){

				return (int)((intptr_t)se->stkValue);
			}
			else{
				fprintf(stderr,"eval_int(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
				exit(-1);
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
			throw error;
		}

		tStackElement *se=CURRENT_VM->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_BOOLEAN){
				return (bool)((intptr_t)se->stkValue);

			}else{
				fprintf(stderr,"eval_bool(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
				exit(-1);
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
			throw error;
		}

		tStackElement *se=CURRENT_VM->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_NUMBER){
				float *f = ((float *)(&se->stkValue));
				return *f;
			}
			else{
				fprintf(stderr,"eval_float(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
				exit(-1);
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
			throw error;
		}

		tStackElement *se=CURRENT_VM->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_STRING){

				value = ((const char *)se->stkValue);
			}
			else{
				fprintf(stderr,"eval_string(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
				exit(-1);
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
		if(!__init__) throw("zetscript not initialized");

		int idx_file=-1;

		if(filename_ref != NULL){
			if(isFilenameAlreadyParsed(filename_ref)){
				throw("Filename \""+string(filename_ref)+"\" already parsed");
			}else{
				tInfoParsedSource ps;
				ps.filename = filename_ref;
				m_parsedSource->push_back(ps);
				idx_file=m_parsedSource->size()-1;
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
				throw error;
			}

			free(buf_tmp);
		}

	}

	ZETSCRIPT_MODULE_EXPORT void CZetScript::compile(){
		if(!__init__) return throw("ZetScript not initialized!");
		//ZS_CLEAR_ERROR_MSG();

		if(CCompiler::getInstance()->compile()){

			// print generated asm ...
			//printGeneratedCodeAllClasses();

			if(m_mainObject == NULL){
				// creates the main entry function with compiled code. On every executing code, within "execute" function
				// virtual machine is un charge of allocating space for all local variables...
				m_mainObject = CScriptClass::instanceScriptVariableByClassName(MAIN_SCRIPT_CLASS_NAME);//new CScriptVariable(&m_structInfoMain);//CScriptClass::instanceScriptVariableByClassName("Main");
			}

		}
		else THROW_SCRIPT_ERROR();

	}

	void CZetScript::execute(){

		if(!__init__) throw ("ZetScript not initialized");
		//ZS_CLEAR_ERROR_MSG();

		bool error=false;

		// the first code to execute is the main function that in fact is a special member function inside our main class
		vm->execute(MAIN_SCRIPT_FUNCTION_OBJECT, m_mainObject,error,NO_PARAMS);

		if(error){
			THROW_SCRIPT_ERROR();
		}


	}

	void CZetScript::eval(const string & s, bool exec_vm, const char *filename_ref)  {

		parse(s,filename_ref);
		compile();
		if(exec_vm){
			execute();
		}
	}

	void CZetScript::eval_file(const char * filename, bool execute){

		//ZS_CLEAR_ERROR_MSG();

		bool status = false;

		char *buf_tmp=NULL;
		int n_bytes;

		if((buf_tmp=CZetScriptUtils::readFile(filename, n_bytes))!=NULL){

			try{
				eval((char *)buf_tmp, execute, filename);
			}catch(script_error & error){
				free(buf_tmp);
				throw error;
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
			zs_print_error_cr("m_mainFunctionInfo is not initialized");
			return false;
		}

		*calling_obj = NULL;
		tSymbolInfo *is=NULL;
		*fun_obj=NULL;

		// 1. accessing var scopes...
		if(access_var.size()>1){
			for(unsigned i=0; i < access_var.size()-1; i++){

				string symbol_to_find=access_var[i];
				if(i==0){ // get variable through main_class.main_function (global element)

					for(unsigned j = 0; j < m_mainFunctionInfo->object_info.local_symbols.m_registeredVariable.size() && *calling_obj==NULL; j++){
						if(m_mainFunctionInfo->object_info.local_symbols.m_registeredVariable[j].symbol_name==symbol_to_find){
							tStackElement *stk = CURRENT_VM->getStackElement(j); // m_mainFunctionInfo->object_info.local_symbols.m_registeredVariable[j].
							if(stk!=NULL){
								if(stk->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
									*calling_obj=(CScriptVariable *)stk->varRef;
								}
							}
							else{
								zs_print_error_cr("cannot access i (%i)",j);
								return false;
							}
						}
					}

					if((*calling_obj) == NULL){
						zs_print_error_cr("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
						return false;
					}

				}else{ // we have got the calling_obj from last iteration ...
					is = (*calling_obj)->getVariableSymbol(symbol_to_find);

					if(is!=NULL){

						if(is->object.properties & STK_PROPERTY_TYPE_SCRIPTVAR){
							*calling_obj=(CScriptVariable *)is->object.varRef;
						}else{
							zs_print_error_cr("error evaluating \"%s\". Variable name \"%s\" not script variable",function_access.c_str(),symbol_to_find.c_str());
							return false;
						}
					}
					else{
						zs_print_error_cr("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
						return false;
					}
				}
			}

			is=(*calling_obj)->getFunctionSymbol(access_var[access_var.size()-1]);
			if(is!=NULL){
				if(is->object.properties & STK_PROPERTY_TYPE_FUNCTION){
					*fun_obj=(CScriptFunctionObject *)is->object.stkValue;
				}
			}else{

				zs_print_error_cr("error evaluating \"%s\". Cannot find function \"%s\"",function_access.c_str(),access_var[access_var.size()-1].c_str());
				return false;
			}

		}else{
			*calling_obj = m_mainObject;
			for(unsigned i = 0; i < m_mainFunctionInfo->object_info.local_symbols.vec_idx_registeredFunction.size() && *fun_obj==NULL; i++){
				CScriptFunctionObject *aux_fun_obj=GET_SCRIPT_FUNCTION_OBJECT(m_mainFunctionInfo->object_info.local_symbols.vec_idx_registeredFunction[i]);
				if(aux_fun_obj->object_info.symbol_info.symbol_name == access_var[0]){
					*fun_obj=aux_fun_obj;
				}
			}
		}

		if(*fun_obj==NULL){
			zs_print_error_cr("error evaluating \"%s\". Variable name \"%s\" is not function type",function_access.c_str(),access_var[access_var.size()-1].c_str());
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
