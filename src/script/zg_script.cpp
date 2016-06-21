
#include "ast.cpp"
#include "CVirtualMachine.cpp"
#include "CCompiler.cpp"
#include "CScope.cpp"
#include "CScriptFunction.cpp"


CZG_Script::tPrimitiveType CZG_Script::primitiveType[MAX_VAR_C_TYPES];
CZG_Script * CZG_Script::m_instance = NULL;


void CZG_Script::registerPrimitiveTypes(){

	primitiveType[VOID_TYPE]={typeid(void).name(),VOID_TYPE};
	primitiveType[INT_TYPE]={typeid(int).name(),INT_TYPE};
	primitiveType[INT_PTR_TYPE]={typeid(int *).name(),INT_PTR_TYPE};
	primitiveType[FLOAT_TYPE]={typeid(float).name(),FLOAT_TYPE};
	primitiveType[FLOAT_PTR_TYPE]={typeid(float *).name(),FLOAT_PTR_TYPE};
	primitiveType[STRING_TYPE]={typeid(string).name(),STRING_TYPE};
	primitiveType[STRING_PTR_TYPE]={typeid(string *).name(),STRING_PTR_TYPE};
	primitiveType[BOOL_TYPE]={typeid(bool).name(),BOOL_TYPE};
	primitiveType[BOOL_PTR_TYPE]={typeid(bool *).name(),BOOL_PTR_TYPE};
}


CZG_Script::tPrimitiveType *CZG_Script::getPrimitiveType(const string & str){

	for(unsigned i=0; i < MAX_VAR_C_TYPES; i++){
		if(primitiveType[i].str == str){
			return &primitiveType[i];
		}
	}

	print_error_cr("type \"%s\" is not registered",str.c_str());

	return NULL;
}

CZG_Script * CZG_Script::getInstance(){
	if(m_instance==NULL){
		m_instance = new CZG_Script();
	}

	return m_instance;
}

void CZG_Script::destroy(){

	if(m_instance!=NULL){
		delete m_instance;
	}

	CFactoryContainer::destroySingletons();

}
/*
CZG_Script::tLocalScope * CZG_Script::createLocalScope(CZG_Script::tLocalScope *m_parent){
	tLocalScope * localScope = new tLocalScope;

	localScope->m_parentScope = m_parent;

	return localScope;
}

CZG_Script::tContext * CZG_Script::createContext(){

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





void print(float * caca){
	print_info_cr("Hola!");
}

CZG_Script::CZG_Script(){

	registerPrimitiveTypes();


	iniFactory<CNumberFactory>("CNumber");
	iniFactory<CIntegerFactory>("CInteger");
	iniFactory<CBooleanFactory>("CBoolean");
	iniFactory<CStringFactory>("CString");
	iniFactory<CUndefinedFactory>("CUndefined");
	iniFactory<CVectorFactory>("Vector");

	CScope::createSingletons();
	CAst::createSingletons();

	//main_context = new CContext();

	m_mainFunction = new CScriptFunction();

	// register c function's
	registerFunction(print);
	//registerFunction(&CCustomObject::member2);

}


void CZG_Script::init(){
	CFactoryContainer::getInstance()->registerScriptFunctions();
	CVirtualMachine::getInstance();
}

bool CZG_Script::eval(const string & s){
	return CCompiler::getInstance()->compile(s, m_mainFunction);
}

bool CZG_Script::execute(){

	//CCompiler::getInstance()->printGeneratedCode(m_mainFunction);

	return CVirtualMachine::getInstance()->execute(m_mainFunction);//->excute();
}


//-------------------------------------------------------------------------------------
CZG_Script::~CZG_Script(){
	// unregister operators ...

	delete m_mainFunction;


	CCompiler::destroySingletons();
	CVirtualMachine::destroySingletons();
	CAst::destroySingletons();
	CScope::destroySingletons();

}
