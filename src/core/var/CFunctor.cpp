#include "../../CZetScript.h"



CFunctor::CFunctor(){

  //  m_classStr=typeid(CFunctor).name();
  //  m_pointerClassStr=typeid(CFunctor *).name();


	//m_varType =VAR_TYPE::FUNCTION;
	//m_value=_irfs;
	this->init(CScriptClass::getInstance()->getRegisteredClassFunctor(), (void *)this);
}

void CFunctor::setFunctionSymbol(tScriptFunctionObject *_irfs){
	m_value=_irfs;
}

