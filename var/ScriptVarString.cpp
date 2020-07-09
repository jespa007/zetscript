/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../ZetScript.h"

namespace zetscript{

	void ScriptVarString::Setup(){
		m_value = &m_strValue;
		m_strValue = "";


	}

	ScriptVarString::ScriptVarString(ZetScript *_zs):ScriptVar(_zs){
		m_strValue = "";
		init(SCRIPT_CLASS_STRING, (void *)this);
	}

	ScriptVarString::ScriptVarString(ZetScript *_zs, const std::string & s):ScriptVar(_zs){


		init(SCRIPT_CLASS_STRING, (void *)this);
		m_strValue = s;
	}

}
