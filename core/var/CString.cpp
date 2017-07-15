#include "../../CZetScript.h"

namespace zetscript{

	void CString::setup(){
		m_value = &m_strValue;
		m_strValue = "";


	}

	CString::CString(){
		m_strValue = "";
		init(CScriptClass::getRegisteredClassString(), (void *)this);
	}

	CString::CString(const string & s){


		init(CScriptClass::getRegisteredClassString(), (void *)this);
		m_strValue = s;
	}

}
