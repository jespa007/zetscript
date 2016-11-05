#include "CSharedPointerManagement.cpp"
#include "var/zg_variable.cpp"
#include "ast/CScopeInfo.cpp"
#include "ast/ast.cpp"
#include "vm/CALE.cpp"
#include "vm/CVirtualMachine.cpp"
#include "CCompiler.cpp"
#include "CScriptClassFactory.cpp"


//#include "CScriptFunction.cpp.old"



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
				//return CScriptVariable::UndefinedSymbol;
			}

			converted_param[i] = paramConv(argv->at(i));
		}
	}



	switch(argv->size()){
	default:
		print_error_cr("cannot call !");
		return CScriptVariable::UndefinedSymbol;
	case 0:
		result=(*((std::function<int ()> *)fun_ptr))();
		break;
	case 1:
		result=(*((std::function<int (int)> *)fun_ptr))(converted_param[0]);
		break;
	}

	var_result = CScriptClassFactory::getInstance()->newClassByIdx(irfs->idx_return_type,(void *)result);

	return var_result;

}
/*
CZG_ScriptCore::tLocalScope * CZG_ScriptCore::createLocalScope(CZG_ScriptCore::tLocalScope *m_parent){
	tLocalScope * localScope = new tLocalScope;

	localScope->m_parentScope = m_parent;

	return localScope;
}

CZG_ScriptCore::tContext * CZG_ScriptCore::createContext(){

	tContext *aux= new tContext();

	aux->base_scope = createLocalScope(NULL);

	return aux;
}
*/

class CCustomObject{
	int x;
public:
	CCustomObject(){
		printf("CCustomObject constructing point @%p\n", this);
	}
	void member(){

		printf("hola %p!\n",this);
	}

	int member2(int i1, int i2){ return 0;}

	   virtual ~CCustomObject()
	   {
	       printf("CCustomObject @%p destroyed\n", this);
	   }
};

/*
bool CZG_ScriptCore::object2float(CVariable *obj, float & v){

	// only float ...
	CNumber *n=dynamic_cast<CNumber *>(obj);
	if(n!=NULL){
		v=n->m_value;
		return true;
	}

	return false;
}

bool CZG_ScriptCore::object2int(CVariable *obj, int & v){
	// only float ...
	CInteger *n=dynamic_cast<CInteger *>(obj);
	if(n!=NULL){
		v=n->m_value;
		return true;
	}

	return false;
}

bool CZG_ScriptCore::object2bool(CVariable *obj, bool & v){
	// only float ...
	CBoolean *n=dynamic_cast<CBoolean *>(obj);
	if(n!=NULL){
		v=n->m_value;
		return true;
	}

	return false;
}

bool CZG_ScriptCore::object2string(CVariable *obj, string & v){
	// only float ...
	CString *n=dynamic_cast<CString *>(obj);
	if(n!=NULL){
		v=n->m_value;
		return true;
	}

	return false;
}


CVariable * CZG_ScriptCore::createObjectFromPrimitiveType(CZG_ScriptCore::tPrimitiveType *pt){

	if(pt != NULL){
		switch(pt->id){
		case C_TYPE_VALID_PRIMITIVE_VAR::VOID_TYPE:
			return CScriptVariable::VoidSymbol;
			break;
		case C_TYPE_VALID_PRIMITIVE_VAR::STRING_TYPE:
			return (CVariable *)NEW_STRING();
			break;
		case C_TYPE_VALID_PRIMITIVE_VAR::FLOAT_TYPE:
			return (CVariable *)NEW_NUMBER();
			break;
		case C_TYPE_VALID_PRIMITIVE_VAR::INT_TYPE:
			return (CVariable *)NEW_INTEGER();
			break;
		case C_TYPE_VALID_PRIMITIVE_VAR::BOOL_TYPE:
			return (CVariable *)NEW_BOOLEAN();
			break;
		default:
			print_error_cr("Not found. Must add object type into factory");
			break;

		}
	}
	return NULL;
}
*/



CZG_ScriptCore::CZG_ScriptCore(){

	//registerFunction(&CCustomObject::member2);
	// call_function("print");
	__init__ = false;
	m_mainClassInfo=NULL;
	m_mainClass=NULL;
	m_mainFunctionInfo=NULL;

	vm=NULL;
}

int interface_variable;



bool CZG_ScriptCore::init(){

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
		// then you have all information -> compile into asm!
		//generateAsmCode(root);
	}

	return false;

	//return CCompiler::getInstance()->compile(s, &m_structInfoMain);
}


bool CZG_ScriptCore::execute(){

	if(!__init__) return false;

	if(m_mainClass == NULL){
		// creates the main entry function with compiled code. On every executing code, within "execute" function
		// virtual machine is un charge of allocating space for all local variables...

		m_mainClass = CScriptClassFactory::getInstance()->newClass(MAIN_SCRIPT_CLASS_NAME);//new CScriptVariable(&m_structInfoMain);//CScriptClassFactory::newClass("Main");

		//m_mainFunction = new CScriptFunction(m_mainClass,&m_structInfoMain);
	}

	//CCompiler::getInstance()->printGeneratedCode(m_mainClass);

	// the first code to execute is the main function that in fact is a special member function inside our main class
	return vm->execute(m_mainFunctionInfo,  m_mainClass, NULL,0);//->excute();
}


//-------------------------------------------------------------------------------------
CZG_ScriptCore::~CZG_ScriptCore(){
	// unregister operators ...

	//delete m_mainClass;
	delete m_mainClass;
	delete vm;
	//delete m_mainFunction;

	CCompiler::destroySingletons();
	//CVirtualMachine::destroySingletons();
	CAst::destroySingletons();
	CScriptVariable::destroySingletons();

}
