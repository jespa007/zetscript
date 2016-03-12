#include "zg_script.h"


CContext::CContext(){
	m_localScope = new CLocalScope();
}

bool CContext::eval(const string & s){
	return m_localScope->eval(s);
}

bool CContext::execute(){
	return CLocalScope::execute(m_localScope);
}


CContext::~CContext(){
	delete m_localScope;
}
