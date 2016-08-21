#include "core/zg_core.h"


CFunctor::CFunctor(){

    m_classStr=typeid(CFunctor).name();
    m_pointerClassStr=typeid(CFunctor *).name();


	m_varType =VAR_TYPE::FUNCTION;
	m_ptr=NULL;
}
