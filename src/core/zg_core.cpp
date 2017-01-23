#include "CSharedPointerManager.cpp"
#include "var/zg_variable.cpp"
#include "ast/CScopeInfo.cpp"
#include "ast/ast.cpp"
#include "vm/CVirtualMachine.cpp"
#include "CCompiler.cpp"
#include "CScriptClassFactory.cpp"

CZG_ScriptCore * CZG_ScriptCore::m_instance = NULL;
CAst *CZG_ScriptCore::m_ast = NULL;


CZG_ScriptCore * CZG_ScriptCore::getInstance(){
	if(m_instance==NULL){
		m_instance = new CZG_ScriptCore();
		if(!m_instance->init()){
			exit(-1);
		}
	}
	return m_instance;
}

void CZG_ScriptCore::destroy(){


	if(m_instance!=NULL){
		delete m_instance;
	}

	CScriptVariable::destroySingletons();
	CScriptClassFactory::destroySingletons();
	CCompiler::destroySingletons();
	CAst::destroySingletons();
	CSharedPointerManager::destroySingletons();


}

CScriptVariable * CZG_ScriptCore::call_C_function(void *fun_ptr, tInfoRegisteredFunctionSymbol *irfs, vector<CScriptVariable *> * argv){

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
		if((argv->at(i))->getPointer_C_ClassName()!=irfs->m_arg[i]){
			fntConversionType paramConv=CScriptClassFactory::getInstance()->getConversionType((argv->at(i))->getPointer_C_ClassName(),irfs->m_arg[i]);

			if(paramConv == NULL){
				return NULL;
			}
			converted_param[i] = paramConv(argv->at(i));
		}
	}

	print_info_cr("pre_call %i",argv->size());

	if(irfs->idx_return_type != CScriptClassFactory::getInstance()->getIdxClassVoid()){

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

		var_result = CScriptClassFactory::getInstance()->newClassByIdx(irfs->idx_return_type,(void *)result);

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

CZG_ScriptCore::CZG_ScriptCore(){

	__init__ = false;
	m_mainClassInfo=NULL;
	m_mainClass=NULL;
	m_mainFunctionInfo=NULL;

	vm=NULL;
}

int interface_variable;

bool CZG_ScriptCore::init(){

	CScriptClassFactory::getInstance();

	 if(!CScriptClassFactory::getInstance()->registerBase()){
			exit(EXIT_FAILURE);
	 }


	vm = new CVirtualMachine();
	CScriptClassFactory::registerPrimitiveTypes();
	CScriptVariable::createSingletons();

	m_ast = CAst::getInstance();

	// ok register CInteger through CScriptVariable...
	if((m_mainClassInfo = CScriptClassFactory::getInstance()->getRegisteredClass(MAIN_SCRIPT_CLASS_NAME)) == NULL) return false;
	if((CScriptClassFactory::getInstance()->getRegisteredFunctionSymbol(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_NAME)) == NULL) return false;

	__init__=true;

	return true;

}

bool CZG_ScriptCore::eval(const string & s){

	if(!__init__) return false;

	// generate whole AST

	if(m_ast->parse(s.c_str())){

		m_mainFunctionInfo = CScriptClassFactory::getInstance()->getRegisteredFunctionSymbol(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_NAME);

		if(CCompiler::getInstance()->compile(m_ast->getMainAstNode(),m_mainFunctionInfo )){
			// print generated asm ...

			if(!CScriptClassFactory::getInstance()->updateReferenceSymbols()){
				return false;
			}

			CScriptClassFactory::getInstance()->printGeneratedCodeAllClasses();//&m_mainFunctionInfo->object_info);
			return true;
		}
	}

	return false;
}

std::function<CScriptVariable * (std::vector<CScriptVariable *> args)> * CZG_ScriptCore::script_call(const string &script_function_name){

	//tInfoRegisteredFunctionSymbol *irfs = CScriptClassFactory::getInstance()->getRegisteredFunctionSymbol(MAIN_SCRIPT_CLASS_NAME,function);

	//if(irfs != NULL){
		for(unsigned i = 0; i < m_mainFunctionInfo->object_info.local_symbols.m_registeredFunction.size(); i++){
			if(m_mainFunctionInfo->object_info.local_symbols.m_registeredFunction[i].object_info.symbol_info.symbol_name == script_function_name){
				return new std::function<CScriptVariable * (std::vector<CScriptVariable *> args)>([&,i](std::vector<CScriptVariable *> args){
						return vm->execute(&m_mainFunctionInfo->object_info.local_symbols.m_registeredFunction[i],  m_mainClass, &args,0);//->excute();
				});
			}
		}
	//}

	print_error_cr("function %s don't exist",script_function_name.c_str());


	return NULL;//[](std::vector<CScriptVariable *> args){};//CScriptVariable::UndefinedSymbol;
}

CScriptVariable * CZG_ScriptCore::execute(){

	if(!__init__) return NULL;

	if(m_mainClass == NULL){
		// creates the main entry function with compiled code. On every executing code, within "execute" function
		// virtual machine is un charge of allocating space for all local variables...

		m_mainClass = CScriptClassFactory::getInstance()->newClass(MAIN_SCRIPT_CLASS_NAME);//new CScriptVariable(&m_structInfoMain);//CScriptClassFactory::newClass("Main");
	}
	// the first code to execute is the main function that in fact is a special member function inside our main class
	return vm->execute(m_mainFunctionInfo,  m_mainClass, NULL,0);//->excute();
}
//-------------------------------------------------------------------------------------
CZG_ScriptCore::~CZG_ScriptCore(){
	// unregister operators ...
	delete m_mainClass;
	delete vm;
}
