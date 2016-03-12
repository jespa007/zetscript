
#include "ast.cpp"
#include "CLocalScope.cpp"
#include "CContext.cpp"



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

	CLocalScope::createSingletons();

	main_context = new CContext();

}


void CZG_Script::init(){
	CFactoryContainer::getInstance()->registerScriptFunctions();
}

bool CZG_Script::eval(const string & s){
	return main_context->eval(s);
}

bool CZG_Script::execute(){
	return main_context->execute();
}

//-------------------------------------------------------------------------------------
CZG_Script::~CZG_Script(){
	// unregister operators ...


	//unregisterOperators();
	delete main_context;
	CLocalScope::destroySingletons();

}
