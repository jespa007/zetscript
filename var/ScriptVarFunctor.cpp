/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../ZetScript.h"

namespace zetscript{

	ScriptVarFunctor::ScriptVarFunctor(ZetScript *_zs):ScriptVar(_zs){

	  //  m_classStr=typeid(ScriptVarFunctor).name();
	  //  m_pointerClassStr=typeid(ScriptVarFunctor *).name();


		//m_varType =VAR_TYPE::FUNCTION;
		//m_value=_irfs;
		this->init(SCRIPT_CLASS_FUNCTOR, (void *)this);
	}

	void ScriptVarFunctor::setFunctionSymbol(ScriptFunction *_irfs){
		m_value=_irfs;
	}

}

