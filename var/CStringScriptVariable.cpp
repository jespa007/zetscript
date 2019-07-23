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

	CStringScriptVariable::CStringScriptVariable(CZetScript *_zs):CScriptVariable(_zs){
		m_strValue = "";
		init(SCRIPT_CLASS_STRING, (void *)this);
	}

	CStringScriptVariable::CStringScriptVariable(CZetScript *_zs, const std::string & s):CScriptVariable(_zs){


		init(SCRIPT_CLASS_STRING, (void *)this);
		m_strValue = s;
	}

}
