#include "zg_script.h"

CScriptFunction::CScriptFunction(CScriptFunction * _globalscope){

	m_globalscope = _globalscope;
	if(_globalscope == NULL){
		m_globalscope = this;
	}

	m_scope = new CScope(m_globalscope);
}

CScriptFunction * CScriptFunction::getGlobalScope(){
	return m_globalscope;
}

bool CScriptFunction::eval(const string & _str){

	bool ok = m_scope->eval(_str);


	return ok;
}

vector<CVirtualMachine::tInfoStatementOp> * CScriptFunction::getCompiledCode(){
	return &m_listStatements;
}

CScriptFunction::~CScriptFunction(){

	for(unsigned s = 0; s  <m_listStatements.size(); s++){
		for(unsigned i = 0; i  <m_listStatements[s].asm_op.size(); i++){

			delete m_listStatements[s].asm_op[i];
		}
	}

	//destroy registered functions ..
	for(unsigned i = 0; i < m_function.size(); i++){
		delete m_function[i];
	}
	delete m_scope;
}
