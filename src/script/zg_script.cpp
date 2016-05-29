
#include "ast.cpp"
#include "CVirtualMachine.cpp"
#include "CCompiler.cpp"
#include "CScope.cpp"
#include "CScriptFunction.cpp"



CZG_Script * CZG_Script::m_instance = NULL;


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
CZG_Script::CZG_Script(){



	iniFactory<CNumberFactory>("CNumber");
	iniFactory<CIntegerFactory>("CInteger");
	iniFactory<CBooleanFactory>("CBoolean");
	iniFactory<CStringFactory>("CString");

	CScope::createSingletons();
	CAst::createSingletons();

	//main_context = new CContext();

	m_mainFunction = new CScriptFunction();

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
