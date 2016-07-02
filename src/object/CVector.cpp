#include "script/zg_script.h"

CVector::CVector(){
	m_varType = CVariable::VAR_TYPE::VECTOR;

	m_ptr = &m_value;
}
