#include "zg_script.h"


CContext::CContext(){
	m_localScope = new CScope(this);
	m_compiler = new CCompiler();
}



bool CContext::eval(const string & s){
	return m_localScope->eval(s);
}



CContext::~CContext(){
	delete m_localScope;
	delete m_compiler;
}
