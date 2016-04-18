#include "zg_script.h"

CScriptFunction::CScriptFunction(CScope * _globalscope){

	m_scope = new CScope(this,_globalscope);

}

CScope *CScriptFunction::getScope(){
	return m_scope;
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
