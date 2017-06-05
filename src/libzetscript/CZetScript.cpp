#include "CZetScript.h"

CZetScript * CZetScript::m_instance = NULL;

vector<tInfoParsedSource> * CZetScript::m_parsedSource = NULL;



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


	if(m_instance!=NULL){
		delete m_instance;
	}


	CNull::destroySingletons();
	CUndefined::destroySingletons();

	CCompiler::destroySingletons();
	CState::destroySingletons();
	CNativeFunction::destroySingletons();
	CScriptClass::destroySingletons();
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

		sprintf(object_access,"[%02i:"
#ifdef __x86_64__
				"%02lu"
#else
				"%02i"
#endif
				"].",current_statment,iao->index_op2);
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
						break;

					}
				switch(asm_op_statment->operator_type){
				case  PUSH_ATTR:

					printf("[%02i:%02i]\t%s\tSTRUCT[%02i:%02i],[%02i:%02i],[%02i:"
#ifdef __x86_64__
							"%02lu]"
#else
							"%02i]"
#endif
							"\n"
							,
							idx_statment,idx_instruction,
							CCompiler::def_operator[asm_op_statment->operator_type].op_str,

							idx_statment,asm_op_statment->index_op1,
							idx_statment,idx_instruction-1, // lat operand must be a string constant ...
							idx_statment,asm_op_statment->index_op2);
					break;

				case  NEW:
					printf("[%02i:%02i]\t%s\t%s\n",idx_statment,idx_instruction,CCompiler::def_operator[asm_op_statment->operator_type].op_str,CScriptClass::getNameRegisteredClassByIdx(asm_op_statment->index_op1));
					break;
				case  LOAD:
					printf("[%02i:%02i]\t%s\t%s%s%s --- %i\n",idx_statment,idx_instruction,
							CCompiler::def_operator[asm_op_statment->operator_type].op_str,
							pre.c_str(),
							getStrTypeLoadValue(fs->statment_op,idx_statment,idx_instruction),
							post.c_str(),
							asm_op_statment->index_op2);
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
#ifdef __x86_64__
							"%04lu"
#else
							"%04i"
#endif
							"]\n",idx_statment,idx_instruction,CCompiler::def_operator[asm_op_statment->operator_type].op_str,asm_op_statment->index_op1,asm_op_statment->index_op2);
					break;
				/*case PRE_INC:
				case POST_INC:
				case PRE_DEC:
				case POST_DEC:
					printf("[%02i:%02i]\t%s\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str);
					break;*/
				case VGET:
				case VPUSH:
					printf("[%02i:%02i]\t%s\t%sVEC[%02i:%02i]%s,[%02i:%02i]\n",idx_statment,idx_instruction,CCompiler::def_operator[asm_op_statment->operator_type].op_str,pre.c_str(),idx_statment,index_op1,post.c_str(),idx_statment,index_op2);
					break;
				default:

					if(n_ops==0){
						printf("[%02i:%02i]\t%s\n",idx_statment,idx_instruction,CCompiler::def_operator[asm_op_statment->operator_type].op_str);
					}else if(n_ops==1){
						printf("[%02i:%02i]\t%s\t[%02i:%02i]\n",idx_statment,idx_instruction,CCompiler::def_operator[asm_op_statment->operator_type].op_str,idx_statment,index_op1);
					}else{
						printf("[%02i:%02i]\t%s\t[%02i:%02i],[%02i:%02i]\n",idx_statment,idx_instruction,CCompiler::def_operator[asm_op_statment->operator_type].op_str,idx_statment,index_op1,idx_statment,index_op2);
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

			if(MAIN_CLASS_NODE->metadata_info.object_info.idxScriptFunctionObject == fs->idxScriptFunctionObject){ // main class (main entry)
				sprintf(symbol_ref,"MAIN_ENTRY (MainClass)");
			}
			else if(MAIN_CLASS_NODE->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[0]!=fs->idxScriptFunctionObject){ // main function (main entry)
				sprintf(symbol_ref,"%s::%s",fs->symbol_info.symbol_name.c_str(),AST_SYMBOL_VALUE_CONST_CHAR(local_irfs->object_info.symbol_info.idxAstNode));
			}

			printf("-------------------------------------------------------\n");
			printf("Code for function \"%s\"\n",symbol_ref);
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


	vm = new CVirtualMachine();

	// ok register CInteger through CScriptVariable...
	//if((m_mainClassInfo = CScriptClass::getScriptClassByName(MAIN_SCRIPT_CLASS_NAME)) == NULL) return false;
	if((CScriptClass::getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_OBJECT_NAME)) == ZS_UNDEFINED_IDX) return false;

	__init__=true;

	return true;

}

bool CZetScript::parse_ast(const char   * s){
	int m_line = 1;
	bool error=false;
	PASTNode main_node = MAIN_AST_NODE;

	if(CASTNode::generateAST_Recursive(
			s,
			m_line,
			MAIN_SCOPE_ROOT,
			error,
			&main_node,
			false,
			true) != NULL){
		return true;
	}
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



bool CZetScript::eval_file(const char * filename){
	if(isFilenameAlreadyParsed(filename)){
		print_error_cr("Filename already parsed");
		return false;
	}
	ByteBuffer *buffer = CIO_Utils::readFile(filename);
	bool status= false;

	if(buffer){

		tInfoParsedSource ps;
		ps.filename = filename;
		ps.data = buffer->data_buffer;

		status =  eval((char *)buffer->data_buffer);
	}
	return status;
}


bool CZetScript::eval(const string & s){

	if(!__init__) return false;

	// generate whole AST

	if(parse_ast(s.c_str())){

		idxMainScriptFunctionObject = CScriptClass::getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_OBJECT_NAME);

		if(CCompiler::getInstance()->compile(IDX_MAIN_AST_NODE,GET_SCRIPT_FUNCTION_OBJECT(idxMainScriptFunctionObject) )){
			// print generated asm ...

			if(!CScriptClass::updateReferenceSymbols()){
				return false;
			}

			printGeneratedCodeAllClasses();//&m_mainFunctionInfo->object_info);
			return true;
		}
	}

	return false;
}

std::function<CScriptVariable * ( std::vector<CScriptVariable *> args)> * CZetScript::script_call(const string &script_function_name){

	//CScriptFunctionObject *irfs = CScriptClass::getInstance()->getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,function);
	CScriptFunctionObject * m_mainFunctionInfo = GET_SCRIPT_FUNCTION_OBJECT(idxMainScriptFunctionObject);

	if(m_mainFunctionInfo != NULL){

	//if(irfs != NULL){
		for(unsigned i = 0; i < m_mainFunctionInfo->object_info.local_symbols.vec_idx_registeredFunction.size(); i++){
			if(GET_SCRIPT_FUNCTION_OBJECT(m_mainFunctionInfo->object_info.local_symbols.vec_idx_registeredFunction[i])->object_info.symbol_info.symbol_name == script_function_name){
				return new std::function<CScriptVariable * (std::vector<CScriptVariable *> args)>([&,i]( std::vector<CScriptVariable *> _args){
					return vm->execute(
								GET_SCRIPT_FUNCTION_OBJECT(m_mainFunctionInfo->object_info.local_symbols.vec_idx_registeredFunction[i]),
								m_mainObject,
								&_args);//->excute();
				});
			}
		}
	//}
		print_error_cr("function %s don't exist",script_function_name.c_str());
	}




	return NULL;//[](std::vector<CScriptVariable *> args){};//CScriptVariable::UndefinedSymbol;
}


CVirtualMachine * CZetScript::getVirtualMachine(){
	return vm;
}

CScriptVariable * CZetScript::execute(){

	if(!__init__) return NULL;

	if(m_mainObject == NULL){
		// creates the main entry function with compiled code. On every executing code, within "execute" function
		// virtual machine is un charge of allocating space for all local variables...

		m_mainObject = CScriptClass::instanceScriptVariableByClassName(MAIN_SCRIPT_CLASS_NAME);//new CScriptVariable(&m_structInfoMain);//CScriptClass::instanceScriptVariableByClassName("Main");


	}
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
