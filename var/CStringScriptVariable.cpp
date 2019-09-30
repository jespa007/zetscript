/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../zetscript.h"

namespace zetscript{

	void CScriptVarString::setup(){
		m_value = &m_strValue;
		m_strValue = "";


	}

	CScriptVarString::CScriptVarString(CZetScript *_zs):CScriptVar(_zs){
		m_strValue = "";
		init(SCRIPT_CLASS_STRING, (void *)this);
	}

	CScriptVarString::CScriptVarString(CZetScript *_zs, const std::string & s):CScriptVar(_zs){


		init(SCRIPT_CLASS_STRING, (void *)this);
		m_strValue = s;
	}

}
