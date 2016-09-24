#include "core/zg_core.h"


CFunctor::CFunctor(tInfoRegisteredClass *info_registered_class,tInfoRegisteredFunctionSymbol *_irfs):CScriptVariable(info_registered_class){

  //  m_classStr=typeid(CFunctor).name();
  //  m_pointerClassStr=typeid(CFunctor *).name();


	//m_varType =VAR_TYPE::FUNCTION;
	m_value=_irfs;
}
