#include "CZetScript.h"

CZetScript * CZetScript::m_instance = NULL;
CAst *CZetScript::m_ast = NULL;


CZetScript * CZetScript::getInstance(){
	if(m_instance==NULL){
		m_instance = new CZetScript();

		CASTNodeFactory::createSingletons();
		CScopeFactory::createSingletons();
		CScriptClassFactory::createSingletons();
		CScriptFunctionObjectFactory::createSingletons();

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

	CASTNodeFactory::destroySingletons();
	CScopeFactory::destroySingletons();
	CScriptClassFactory::destroySingletons();
	CScriptFunctionObjectFactory::destroySingletons();

	CScriptVariable::destroySingletons();
	//CScriptClass::destroySingletons();
	CCompiler::destroySingletons();
	//CAst::destroySingletons();
	CSharedPointerManager::destroySingletons();
	C_VariableFunctionFactory::destroySingletons();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
 // PRINT ASM INFO

 const char * CZetScript::getStrMovVar(tInfoAsmOp * iao){

 	if(iao->operator_type != MOV){
 		return "ERROR";
 	}

 	string symbol_value="Unknown";

 	if(iao->idxAstNode != -1){
 		symbol_value = AST_SYMBOL_VALUE(iao->idxAstNode);
 	}

 	sprintf(print_aux_load_value,"VAR(%s)",symbol_value.c_str());

 	return print_aux_load_value;
 }

 const char * CZetScript::getStrTypeLoadValue(vector<tInfoStatementOp> * m_listStatements,int current_statment, int current_instruction){


 	tInfoAsmOp * iao =(*m_listStatements)[current_statment].asm_op[current_instruction];


 	if(iao->operator_type != LOAD){
 		return "ERROR";
 	}


 	string symbol_value="Unknown";

 	if(iao->idxAstNode != -1){
 		symbol_value = AST_SYMBOL_VALUE(iao->idxAstNode);
 	}

 	char object_access[512] = "";

 	sprintf(print_aux_load_value,"UNDEFINED");

	if(iao->scope_type == SCOPE_TYPE::ACCESS_SCOPE){
		sprintf(object_access,"[%02i:%02i].",current_statment,iao->index_op2);
	}
	else if(iao->scope_type == SCOPE_TYPE::THIS_SCOPE){
		sprintf(object_access,"this.");
	}


 	/*if(iao->scope_type == SCOPE_TYPE::ACCESS_SCOPE) {
 		sprintf(print_aux_load_value,"[%02i:%02i].%s (solved at run-time)",current_statment,current_instruction-1,symbol_value.c_str());
 	}
 	else*/
 	{

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
 	}

 	return print_aux_load_value;
 }

 void CZetScript::printGeneratedCode_Recursive(tFunctionInfo *fs){

 	vector<tInfoStatementOp> * m_listStatements = &fs->statment_op;
 	string pre="";
 	string post="";

 	for(unsigned s = 0; s < (*m_listStatements).size();s++){
 		vector<tInfoAsmOp *> * asm_op_statment = &(*m_listStatements)[s].asm_op;

 		//printf("\n[%s]\n\n","file.zs");

 		for(unsigned i = 0; i  <  asm_op_statment->size(); i++){

 			int n_ops=0;
 			int index_op1 = (*asm_op_statment)[i]->index_op1;
 			int index_op2 = (*asm_op_statment)[i]->index_op2;

 			if(index_op1 != -1)
 				n_ops++;

 			 if(index_op2 != -1)
 				 n_ops++;

 			 pre="";
 			 post="";

 				switch((*asm_op_statment)[i]->pre_post_operator_type){
 				case ASM_PRE_POST_OPERATORS::PRE_NEG:
 					pre="-";
 					break;
 				case ASM_PRE_POST_OPERATORS::PRE_INC:
 					pre="++";
 					break;
 				case ASM_PRE_POST_OPERATORS::PRE_DEC:
 					pre="--";
 					break;
 				case ASM_PRE_POST_OPERATORS::POST_INC:
 					post="++";
 					break;
 				case ASM_PRE_POST_OPERATORS::POST_DEC:
 					post="--";
 					break;
 				default:
 					break;

 				}
 			switch((*asm_op_statment)[i]->operator_type){
 			case  PUSH_ATTR:
 				printf("[%02i:%02i]\t%s\tSTRUCT[%02i:%02i],[%02i:%02i],[%02i:%02i]\n",
 						s,i,
						CCompiler::def_operator[(*asm_op_statment)[i]->operator_type].op_str,

						s,(*asm_op_statment)[i]->index_op1,
						s,i-1, // lat operand must be a string constant ...
						s,(*asm_op_statment)[i]->index_op2);
 				break;

 			case  NEW:
 				printf("[%02i:%02i]\t%s\t%s\n",s,i,CCompiler::def_operator[(*asm_op_statment)[i]->operator_type].op_str,CScriptClass::getInstance()->getNameRegisteredClassByIdx((*asm_op_statment)[i]->index_op1));
 				break;
 			case  LOAD:
 				printf("[%02i:%02i]\t%s\t%s%s%s\n",s,i,
 						CCompiler::def_operator[(*asm_op_statment)[i]->operator_type].op_str,
 						pre.c_str(),
 						getStrTypeLoadValue(m_listStatements,s,i),
 						post.c_str());
 				break;
 			//case  MOV:
 			//	printf("[%02i:%02i]\t%s\t%s,[%02i:%02i]\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str,getStrMovVar((*asm_op_statment)[i]),s,index_op2);
 			//	break;
 			case POP_SCOPE:
 				printf("[%02i:%02i]\t%s(%i)\n",s,i,CCompiler::def_operator[(*asm_op_statment)[i]->operator_type].op_str,(*asm_op_statment)[i]->index_op1);
 				break;
 			case JNT:
 			case JT:
 			case JMP:
 				printf("[%02i:%02i]\t%s\t[%04i:%04i]\n",s,i,CCompiler::def_operator[(*asm_op_statment)[i]->operator_type].op_str,(*asm_op_statment)[i]->index_op1,(*asm_op_statment)[i]->index_op2);
 				break;
 			/*case PRE_INC:
 			case POST_INC:
 			case PRE_DEC:
 			case POST_DEC:
 				printf("[%02i:%02i]\t%s\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str);
 				break;*/
 			case VGET:
 			case VPUSH:
 				printf("[%02i:%02i]\t%s\t%sVEC[%02i:%02i]%s,[%02i:%02i]\n",s,i,CCompiler::def_operator[(*asm_op_statment)[i]->operator_type].op_str,pre.c_str(),s,index_op1,post.c_str(),s,index_op2);
 				break;
 			default:

 				if(n_ops==0){
 					printf("[%02i:%02i]\t%s\n",s,i,CCompiler::def_operator[(*asm_op_statment)[i]->operator_type].op_str);
 				}else if(n_ops==1){
 					printf("[%02i:%02i]\t%s\t[%02i:%02i]\n",s,i,CCompiler::def_operator[(*asm_op_statment)[i]->operator_type].op_str,s,index_op1);
 				}else{
 					printf("[%02i:%02i]\t%s\t[%02i:%02i],[%02i:%02i]\n",s,i,CCompiler::def_operator[(*asm_op_statment)[i]->operator_type].op_str,s,index_op1,s,index_op2);
 				}
 				break;
 			}
 		}
 	}
 	// and then print its functions ...
 	vector<int> * m_vf = &fs->local_symbols.vec_idx_registeredFunction;



 	for(unsigned j =0; j < m_vf->size(); j++){

 		tScriptFunctionObject *local_irfs = REGISTERED_FUNCTION_SYMBOL_NODE((*m_vf)[j]);

 		if(( local_irfs->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF){
 			char symbol_ref[1024*8]={0};

 			strcpy(symbol_ref,AST_SYMBOL_VALUE_CONST_CHAR(local_irfs->object_info.symbol_info.idxAstNode));

 			if(MAIN_CLASS_NODE->metadata_info.object_info.idxFunctionSymbol == fs->idxFunctionSymbol){ // main class (main entry)
 				sprintf(symbol_ref,"MAIN_ENTRY (MainClass)");
 			}
 			else if(MAIN_CLASS_NODE->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[0]!=fs->idxFunctionSymbol){ // main function (main entry)
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

	 vector<tInfoRegisteredClass *> * registeredClass = CState::getCurrentVecRegisteredClassNode();

	 // for all classes print code...
	 for(unsigned i = 0; i < registeredClass->size(); i++){
		 printGeneratedCode(GET_FUNCTION_INFO(registeredClass->at(i)->metadata_info.object_info.idxFunctionSymbol));
	 }

 }

 // PRINT ASM INFO
 //----------------------------------------------------------------------------------------------------------------------------------------------------------------

CScriptVariable * CZetScript::call_C_function(void *fun_ptr, tScriptFunctionObject *irfs, vector<CScriptVariable *> * argv){

	int converted_param[MAX_PARAM_C_FUNCTION];
	CScriptVariable *var_result;
	int result;

	if((irfs->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) {
		print_error_cr("Function is not registered as C");
		return CScriptVariable::UndefinedSymbol;
	}


	if(fun_ptr==0){
		print_error_cr("Null function");
		return CScriptVariable::UndefinedSymbol;
	}

	if(irfs->m_arg.size() != argv->size()){
		print_error_cr("C argument VS scrip argument doestn't match sizes");
		return CScriptVariable::UndefinedSymbol;
	}

	if(irfs->m_arg.size() >= MAX_PARAM_C_FUNCTION){
		print_error_cr("Reached max param for C function (Current: %i Max Allowed: %i)",irfs->m_arg.size(),MAX_PARAM_C_FUNCTION);
		return CScriptVariable::UndefinedSymbol;
	}

	// convert parameters script to c...
	for(unsigned int i = 0; i < argv->size();i++){

		converted_param[i]= (int)(argv->at(i));

		if(!(argv->at(i)->getPointer_C_ClassName()==TYPE_SCRIPT_VARIABLE && irfs->m_arg[i]==typeid(CScriptVariable *).name())){ //not script, then it can pass through ...

			if((argv->at(i))->getPointer_C_ClassName()!=irfs->m_arg[i]){
				fntConversionType paramConv=CScriptClass::getInstance()->getConversionType((argv->at(i))->getPointer_C_ClassName(),irfs->m_arg[i]);

				if(paramConv == NULL){
					return NULL;
				}
				converted_param[i] = paramConv(argv->at(i));
			}
		}
	}

	print_info_cr("pre_call %i",argv->size());

	if(irfs->idx_return_type != CScriptClass::getInstance()->getIdxClassVoid()){

		switch(argv->size()){
		default:
			print_error_cr("Max run-time args! (Max:%i Provided:%i)",6,argv->size());
			return CScriptVariable::UndefinedSymbol;
		case 0:
			result=(*((std::function<int ()> *)fun_ptr))();
			break;
		case 1:
			result=(*((std::function<int (int)> *)fun_ptr))(converted_param[0]);
			break;
		case 2:
			result=(*((std::function<int (int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1]
									);
			break;
		case 3:
			result=(*((std::function<int (int,int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1],
					converted_param[2]
									);
			break;
		case 4:
			result=(*((std::function<int (int,int,int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1],
					converted_param[2],
					converted_param[3]
									);
			break;
		case 5:
			result=(*((std::function<int (int,int,int,int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1],
					converted_param[2],
					converted_param[3],
					converted_param[4]
   				);
			break;
		case 6:
			result=(*((std::function<int (int,int,int,int,int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1],
					converted_param[2],
					converted_param[3],
					converted_param[4],
					converted_param[5]
									);
			break;

		}

		var_result = CScriptClass::getInstance()->newClassByIdx(irfs->idx_return_type,(void *)result);

	}else{
		switch(argv->size()){
		default:
			print_error_cr("Max run-time args! (Max:%i Provided:%i)",6,argv->size());
			return CScriptVariable::UndefinedSymbol;
		case 0:
			(*((std::function<void ()> *)fun_ptr))();
			break;
		case 1:
			(*((std::function<void (int)> *)fun_ptr))(converted_param[0]);
			break;
		case 2:
			(*((std::function<void (int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1]
									);
			break;
		case 3:
			(*((std::function<void (int,int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1],
					converted_param[2]
									);
			break;
		case 4:
			(*((std::function<void (int,int,int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1],
					converted_param[2],
					converted_param[3]
									);
			break;
		case 5:
			(*((std::function<void (int,int,int,int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1],
					converted_param[2],
					converted_param[3],
					converted_param[4]
   				);
			break;
		case 6:
			(*((std::function<void (int,int,int,int,int,int)> *)fun_ptr))(
					converted_param[0],
					converted_param[1],
					converted_param[2],
					converted_param[3],
					converted_param[4],
					converted_param[5]
									);
			break;

		}

		var_result = CScriptVariable::UndefinedSymbol;
	}



	return var_result;

}

CZetScript::CZetScript(){

	__init__ = false;
	m_mainClassInfo=NULL;
	m_mainClass=NULL;
	m_mainFunctionInfo=NULL;

	vm=NULL;
}

int interface_variable;

bool CZetScript::init(){


	CScriptClass::getInstance();

	 if(!CScriptClass::getInstance()->registerBase()){
			exit(EXIT_FAILURE);
	 }


	vm = new CVirtualMachine();
	CScriptClass::registerPrimitiveTypes();
	CScriptVariable::createSingletons();

	m_ast = CAst::getInstance();

	// ok register CInteger through CScriptVariable...
	if((m_mainClassInfo = CScriptClass::getInstance()->getRegisteredClass(MAIN_SCRIPT_CLASS_NAME)) == NULL) return false;
	if((CScriptClass::getInstance()->getFunctionSymbol(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_NAME)) == NULL) return false;

	__init__=true;

	return true;

}

bool CZetScript::eval(const string & s){

	if(!__init__) return false;

	// generate whole AST

	if(m_ast->parse(s.c_str())){

		m_mainFunctionInfo = CScriptClass::getInstance()->getFunctionSymbol(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_NAME);

		if(CCompiler::getInstance()->compile(m_ast->getMainAstNode(),m_mainFunctionInfo )){
			// print generated asm ...

			if(!CScriptClass::getInstance()->updateReferenceSymbols()){
				return false;
			}

			CScriptClass::getInstance()->printGeneratedCodeAllClasses();//&m_mainFunctionInfo->object_info);
			return true;
		}
	}

	return false;
}

std::function<CScriptVariable * (std::vector<CScriptVariable *> args)> * CZetScript::script_call(const string &script_function_name){

	//tScriptFunctionObject *irfs = CScriptClass::getInstance()->getFunctionSymbol(MAIN_SCRIPT_CLASS_NAME,function);

	//if(irfs != NULL){
		for(unsigned i = 0; i < m_mainFunctionInfo->object_info.local_symbols.vec_idx_registeredFunction.size(); i++){
			if(GET_SCRIPT_FUNCTION_OBJECT(m_mainFunctionInfo->object_info.local_symbols.vec_idx_registeredFunction[i])->object_info.symbol_info.symbol_name == script_function_name){
				return new std::function<CScriptVariable * (std::vector<CScriptVariable *> args)>([&,i](std::vector<CScriptVariable *> args){
						return vm->execute(&m_mainFunctionInfo->object_info.local_symbols.vec_idx_registeredFunction[i],  m_mainClass, &args,0);//->excute();
				});
			}
		}
	//}

	print_error_cr("function %s don't exist",script_function_name.c_str());


	return NULL;//[](std::vector<CScriptVariable *> args){};//CScriptVariable::UndefinedSymbol;
}

CScriptVariable * CZetScript::execute(){

	if(!__init__) return NULL;

	if(m_mainClass == NULL){
		// creates the main entry function with compiled code. On every executing code, within "execute" function
		// virtual machine is un charge of allocating space for all local variables...

		m_mainClass = CScriptClass::getInstance()->newClass(MAIN_SCRIPT_CLASS_NAME);//new CScriptVariable(&m_structInfoMain);//CScriptClass::newClass("Main");
	}
	// the first code to execute is the main function that in fact is a special member function inside our main class
	return vm->execute(m_mainFunctionInfo,  m_mainClass, NULL,0);//->excute();
}
//-------------------------------------------------------------------------------------
CZetScript::~CZetScript(){
	// unregister operators ...
	delete m_mainClass;
	delete vm;
}
