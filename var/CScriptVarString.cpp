/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../ZetScript.h"

namespace ZetScript{

	void CScriptVarString::setup(){
		m_value = &m_strValue;
		m_strValue = "";


	}

	CScriptVarString::CScriptVarString(ZetScript *_zs):CScriptVar(_zs){
		m_strValue = "";
		init(SCRIPT_CLASS_STRING, (void *)this);
	}

	CScriptVarString::CScriptVarString(ZetScript *_zs, const std::string & s):CScriptVar(_zs){


		init(SCRIPT_CLASS_STRING, (void *)this);
		m_strValue = s;
	}

}
