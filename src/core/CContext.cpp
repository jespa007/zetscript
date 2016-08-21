#include "zg_core.h"


CContext::CContext(){
	m_localScope = new CScopeInfo(this);
	m_compiler = new CCompiler();
}



bool CContext::eval(const string & s){
	return m_localScope->eval(s);
}



CContext::~CContext(){
	delete m_localScope;
	delete m_compiler;
}
