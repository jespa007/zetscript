#include "CZetScript.h"

namespace zetscript{

	CZetScript * CZetScript::m_instance = NULL;
	char CZetScript::str_error[MAX_BUFFER_STR_ERROR] = { 0 };
	vector<tInfoParsedSource> * CZetScript::m_parsedSource = NULL;

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

			//CASTNode::createSingletons();
			//CScope::createSingletons();
			//CScriptClass::createSingletons();
			//CScriptFunctionObject::createSingletons();

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
	}
	
	void  CZetScript::clearErrorMsg(){
		memset(str_error, 0,sizeof(str_error));
	}

	void  CZetScript::writeErrorMsg(const char *filename, int line, const  char  *string_text, ...) {
		char  aux_text[MAX_BUFFER_AUX_TMP]={0};


		ZETSCRIPT_CAPTURE_VARIABLE_ARGS(aux_text, string_text);

		if (strlen(aux_text) + strlen(str_error) > MAX_BUFFER_STR_ERROR) {
			memset(str_error, 0,sizeof(str_error));
		}

		sprintf(str_error,"[%s:%i] %s", filename, line, aux_text);

#if __DEBUG__
		zs_print_error_cr(str_error);
#endif

	}

	const char *  CZetScript::getErrorMsg() {
		return str_error;
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

							// put only if load is var.
							//if(!(
							//	  (asm_op_statment->operator_type == LOAD)
							 //  && (asm_op_statment->index_op1 == LOAD_TYPE::LOAD_TYPE_CONSTANT)
							  // )
							//){/
								pre="-";
							//}
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
					//case  STORE:
					//	printf("[%02i:%02i]\t%s\t%s,[%02i:%02i]\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str,getStrMovVar((*asm_op_statment)[i]),s,index_op2);
					//	break;
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
					/*case PRE_INC:
					case POST_INC:
					case PRE_DEC:
					case POST_DEC:
						printf("[%02i:%02i]\t%s\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str);
						break;*/
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

				/*if(MAIN_CLASS_NODE->metadata_info.object_info.idxScriptFunctionObject == fs->idxScriptFunctionObject){ // main class (main entry)
					sprintf(symbol_ref,"MAIN_ENTRY (MainClass)");
				}
				else if(MAIN_CLASS_NODE->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[0]!=fs->idxScriptFunctionObject){ // main function (main entry)
					sprintf(symbol_ref,"%s::%s",fs->symbol_info.symbol_name.c_str(),AST_SYMBOL_VALUE_CONST_CHAR(local_irfs->object_info.symbol_info.idxAstNode));
				}*/

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


		idxMainScriptFunctionObject=ZS_UNDEFINED_IDX;
		m_mainObject = NULL;
		__init__ = false;


		vm=NULL;
	}

	int interface_variable;

	bool CZetScript::init(){

		CState::init();
		CASTNode::init();


		 if(!CScriptClass::init()){
				exit(EXIT_FAILURE);
		 }

		 // init custom registers...
		 CJSON::registerScript();

		 // push undefined file by default ...
		tInfoParsedSource ps;
		ps.filename = "undefined";
		m_parsedSource->push_back(ps);


		 // after init, let's save/restore state...
		 CState::saveState();
		 CState::setState(0);



		vm = new CVirtualMachine();

		// ok register CInteger through CScriptVariable...
		//if((m_mainClassInfo = CScriptClass::getScriptClassByName(MAIN_SCRIPT_CLASS_NAME)) == NULL) return false;
		if((CScriptClass::getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_OBJECT_NAME)) == ZS_UNDEFINED_IDX) return false;

		__init__=true;

		return true;

	}

	bool CZetScript::parse_ast(const char   * s, const char *filename){
		int m_line = 1;
		bool error=false;
		PASTNode main_node = MAIN_AST_NODE;
		int idx_parsed_source = getIdxParsedFilenameSource(filename);

		if(idx_parsed_source == -1){
			return false;
		}

		SET_PARSING_FILENAME(idx_parsed_source,filename);


		if(CASTNode::generateAST_Recursive(
				s,
				m_line,
				MAIN_SCOPE_NODE,
				error,
				&main_node,
				false,
				true) != NULL){
			return true;
		}

		RESET_PARSING_FILENAME;

		return false;
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
			return "unknown";
		}

		return m_parsedSource->at(idx).filename.c_str();
	}

	int CZetScript::eval_int(const string & str_to_eval){
		int value = int();
		void *ptr=NULL;
		string typestr = typeid(int).name();


		CZetScript *zetscript= CZetScript::getInstance();


		if(zetscript->eval(str_to_eval)){


				tStackElement *se=CURRENT_VM->getLastStackValue();

				if(se != NULL){

					if((typestr == typeid(int).name())  && (se->properties & STK_PROPERTY_TYPE_INTEGER)){
						ptr = &value;
						*((intptr_t *)ptr) = (intptr_t)se->stkValue;
					}
					else{
						fprintf(stderr,"eval<%s>(...): Error evaluating \"%s\". Property:0x%X",typestr.c_str(),str_to_eval.c_str(),se->properties);
						exit(-1);
					}


				}
		}

		return value;
	}

	bool CZetScript::eval_bool(const string & str_to_eval){
		bool value = bool();
		string typestr = typeid(bool).name();


		CZetScript *zetscript= CZetScript::getInstance();


		if(zetscript->eval(str_to_eval)){


				tStackElement *se=CURRENT_VM->getLastStackValue();

				if(se != NULL){

					if((typestr == typeid(bool).name())  && (se->properties & STK_PROPERTY_TYPE_BOOLEAN)){
												memcpy(&value, &se->stkValue, sizeof(bool));

					}else{
						fprintf(stderr,"eval<%s>(...): Error evaluating \"%s\". Property:0x%X",typestr.c_str(),str_to_eval.c_str(),se->properties);
						exit(-1);
					}
				}
		}

		return value;
	}

	float CZetScript::eval_float(const string & str_to_eval){
		float value = float();
		void *ptr=NULL;
		string typestr = typeid(float).name();


		CZetScript *zetscript= CZetScript::getInstance();


		if(zetscript->eval(str_to_eval)){


				tStackElement *se=CURRENT_VM->getLastStackValue();

				if(se != NULL){

					if((typestr == typeid(float).name())  && (se->properties & STK_PROPERTY_TYPE_NUMBER)){
						ptr = &value;
						memcpy(ptr,(const void *)&se->stkValue,sizeof(float));
					}
					else{
						fprintf(stderr,"eval<%s>(...): Error evaluating \"%s\". Property:0x%X",typestr.c_str(),str_to_eval.c_str(),se->properties);
						exit(-1);
					}


				}
		}

		return value;
	}

	string CZetScript::eval_string(const string & str_to_eval){

		string value;
		void *ptr=NULL;
		string typestr = typeid(string).name();


		CZetScript *zetscript= CZetScript::getInstance();


		if(zetscript->eval(str_to_eval)){

				tStackElement *se=CURRENT_VM->getLastStackValue();

				if(se != NULL){

					if((typestr == typeid(string).name())  && (se->properties & STK_PROPERTY_TYPE_STRING)){
						ptr=&value;
						*((string *)ptr) = ((const char *)se->stkValue);
					}
					else{
						fprintf(stderr,"eval<%s>(...): Error evaluating \"%s\". Property:0x%X",typestr.c_str(),str_to_eval.c_str(),se->properties);
						exit(-1);
					}
				}
		}

		return value;

	}

	bool CZetScript::eval(const string & s, bool execute, const char *filename){

		if(!__init__) return false;


		ZS_CLEAR_ERROR_MSG();

		if(parse_ast(s.c_str(),filename)){

			idxMainScriptFunctionObject = CScriptClass::getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_OBJECT_NAME);

			CLEAR_COMPILE_INFORMATION;

			if(CCompiler::getInstance()->compile(IDX_MAIN_AST_NODE,GET_SCRIPT_FUNCTION_OBJECT(idxMainScriptFunctionObject) )){
				// print generated asm ...

				if(!CScriptClass::updateReferenceSymbols()){
					return false;
				}

				if(m_mainObject == NULL){
					// creates the main entry function with compiled code. On every executing code, within "execute" function
					// virtual machine is un charge of allocating space for all local variables...

					m_mainObject = CScriptClass::instanceScriptVariableByClassName(MAIN_SCRIPT_CLASS_NAME);//new CScriptVariable(&m_structInfoMain);//CScriptClass::instanceScriptVariableByClassName("Main");


				}

	#ifdef __DEBUG__
				printGeneratedCodeAllClasses();//&m_mainFunctionInfo->object_info);
	#endif

				if(execute){
					return vm->execute(GET_SCRIPT_FUNCTION_OBJECT(idxMainScriptFunctionObject), m_mainObject);
				}

				return true;
			}
		}

		// restore to last state...
		CState::restoreLastState();

		return false;
	}

	bool CZetScript::eval_file(const char * filename){

		ZS_CLEAR_ERROR_MSG();

		if(isFilenameAlreadyParsed(filename)){
			zs_print_error_cr("Filename already parsed");
			return false;
		}


		bool status = false;

		char *buf_tmp=NULL;
		int n_bytes;

		if((buf_tmp=CIO_Utils::readFile(filename, n_bytes))!=NULL){

			string file=CIO_Utils::getFileName(filename);
			tInfoParsedSource ps;
			ps.filename = file;
			m_parsedSource->push_back(ps);
			//ps.data = buf_tmp;

			status = eval((char *)buf_tmp, true, file.c_str());

			free(buf_tmp);
		}


		return status;
	}

	std::function<CScriptVariable * (const std::vector<CScriptVariable *> & args)> * CZetScript::bind_function(const string &function_access_expression){

		ZS_CLEAR_ERROR_MSG();

		vector<string> access_var = CStringUtils::split(function_access_expression,'.');
		CScriptFunctionObject * m_mainFunctionInfo = GET_SCRIPT_FUNCTION_OBJECT(idxMainScriptFunctionObject);
		CScriptVariable *calling_obj = NULL;
		tSymbolInfo *is=NULL;
		CScriptFunctionObject *fun_obj=NULL;


		// 1. accessing var scopes...
		if(access_var.size()>1){
			for(unsigned i=0; i < access_var.size()-1; i++){

				string symbol_to_find=access_var[i];
				if(i==0){ // get variable through main_class.main_function (global element)

					for(unsigned j = 0; j < m_mainFunctionInfo->object_info.local_symbols.m_registeredVariable.size() && calling_obj==NULL; j++){
						if(m_mainFunctionInfo->object_info.local_symbols.m_registeredVariable[j].symbol_name==symbol_to_find){
							tStackElement *stk = CURRENT_VM->getStackElement(j); // m_mainFunctionInfo->object_info.local_symbols.m_registeredVariable[j].
							if(stk!=NULL){
								if(stk->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
									calling_obj=(CScriptVariable *)stk->varRef;
								}
							}
							else{
								zs_print_error_cr("cannot access i (%i)",j);
								return NULL;
							}
						}
					}
				}else{ // we have got the calling_obj from last iteration ...
					is = calling_obj->getVariableSymbol(symbol_to_find);

					if(is!=NULL){

						if(is->object.properties & STK_PROPERTY_TYPE_SCRIPTVAR){
							calling_obj=(CScriptVariable *)is->object.varRef;
						}else{
							zs_print_error_cr("error evaluating \"%s\". Variable name \"%s\" not script variable",function_access_expression.c_str(),symbol_to_find.c_str());
							return NULL;
						}
					}
					else{
						zs_print_error_cr("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access_expression.c_str(),symbol_to_find.c_str());
						return NULL;
					}

				}

			}

			is=calling_obj->getFunctionSymbol(access_var[access_var.size()-1]);
			if(is!=NULL){
				if(is->object.properties & STK_PROPERTY_TYPE_FUNCTION){
					fun_obj=(CScriptFunctionObject *)is->object.stkValue;
				}
			}else{

				zs_print_error_cr("error evaluating \"%s\". Cannot find function \"%s\"",function_access_expression.c_str(),access_var[access_var.size()-1].c_str());
				return NULL;
			}


		}else{
			calling_obj = m_mainObject;
			for(unsigned i = 0; i < m_mainFunctionInfo->object_info.local_symbols.vec_idx_registeredFunction.size() && fun_obj==NULL; i++){
				CScriptFunctionObject *aux_fun_obj=GET_SCRIPT_FUNCTION_OBJECT(m_mainFunctionInfo->object_info.local_symbols.vec_idx_registeredFunction[i]);
				if(aux_fun_obj->object_info.symbol_info.symbol_name == access_var[0]){
					fun_obj=aux_fun_obj;
				}
			}
		}

		if(fun_obj==NULL){
			zs_print_error_cr("error evaluating \"%s\". Variable name \"%s\" is not function type",function_access_expression.c_str(),access_var[access_var.size()-1].c_str());
			return NULL;
		}



		return new std::function<CScriptVariable * (const std::vector<CScriptVariable *> & _args)>([&,calling_obj,fun_obj](const std::vector<CScriptVariable *> & _args){
					return vm->execute(
								fun_obj,
								calling_obj,
								_args);
				});
	}

	CVirtualMachine * CZetScript::getVirtualMachine(){
		return vm;
	}

	CScriptVariable * CZetScript::execute(){

		if(!__init__) return NULL;

		// the first code to execute is the main function that in fact is a special member function inside our main class
		return vm->execute(GET_SCRIPT_FUNCTION_OBJECT(idxMainScriptFunctionObject), m_mainObject);//->excute();
	}
	//-------------------------------------------------------------------------------------
	CZetScript::~CZetScript(){
		// unregister operators ...
		if( m_mainObject != NULL){
			delete m_mainObject;
		}
		delete vm;


	}

}
