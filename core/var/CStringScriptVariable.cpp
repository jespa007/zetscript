#include "../../CZetScript.h"

namespace zetscript{

	void CStringScriptVariable::setup(){
		m_value = &m_strValue;
		m_strValue = "";


	}

	CStringScriptVariable::CStringScriptVariable(){
		m_strValue = "";
		init(CScriptClass::getRegisteredClassString(), (void *)this);
	}

	CStringScriptVariable::CStringScriptVariable(const string & s){


		init(CScriptClass::getRegisteredClassString(), (void *)this);
		m_strValue = s;
	}

}
