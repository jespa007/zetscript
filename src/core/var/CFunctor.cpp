#include "core/zg_core.h"


CFunctor::CFunctor(tInfoRegisteredFunctionSymbol *_irfs){

    m_classStr=typeid(CFunctor).name();
    m_pointerClassStr=typeid(CFunctor *).name();


	m_varType =VAR_TYPE::FUNCTION;
	m_value=_irfs;
}
