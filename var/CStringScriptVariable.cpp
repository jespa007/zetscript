/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../zetscript.h"

namespace zetscript{

	void CStringScriptVariable::setup(){
		m_value = &m_strValue;
		m_strValue = "";


	}

	CStringScriptVariable::CStringScriptVariable(){
		m_strValue = "";
		init(SCRIPT_CLASS_STRING, (void *)this);
	}

	CStringScriptVariable::CStringScriptVariable(const std::string & s){


		init(SCRIPT_CLASS_STRING, (void *)this);
		m_strValue = s;
	}

}
