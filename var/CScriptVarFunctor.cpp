/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../ZetScript.h"

namespace ZetScript{

	CScriptVarFunctor::CScriptVarFunctor(ZetScript *_zs):CScriptVar(_zs){

	  //  m_classStr=typeid(CScriptVarFunctor).name();
	  //  m_pointerClassStr=typeid(CScriptVarFunctor *).name();


		//m_varType =VAR_TYPE::FUNCTION;
		//m_value=_irfs;
		this->init(SCRIPT_CLASS_FUNCTOR, (void *)this);
	}

	void CScriptVarFunctor::setFunctionSymbol(ScriptFunction *_irfs){
		m_value=_irfs;
	}

}

