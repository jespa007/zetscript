#pragma once

template<typename _T>
class CVariable:public CObject{
public:

	_T m_value;
	_T *m_ptr;


	CVariable(){
		m_ptr = &m_value;
	}
};
