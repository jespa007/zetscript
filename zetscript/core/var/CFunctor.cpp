#include "../../CZetScript.h"



CFunctor::CFunctor(){

  //  m_classStr=typeid(CFunctor).name();
  //  m_pointerClassStr=typeid(CFunctor *).name();


	//m_varType =VAR_TYPE::FUNCTION;
	//m_value=_irfs;
	this->init(CScriptClassFactory::getInstance()->getRegisteredClassFunctor(), (void *)this);
}

void CFunctor::setRegisteredFunctionSymbol(tInfoRegisteredFunctionSymbol *_irfs){
	m_value=_irfs;
}

