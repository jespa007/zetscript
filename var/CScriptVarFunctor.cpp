/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../zetscript.h"

namespace zetscript{

	CScriptVarFunctor::CScriptVarFunctor(CZetScript *_zs):CScriptVar(_zs){

	  //  m_classStr=typeid(CScriptVarFunctor).name();
	  //  m_pointerClassStr=typeid(CScriptVarFunctor *).name();


		//m_varType =VAR_TYPE::FUNCTION;
		//m_value=_irfs;
		this->init(SCRIPT_CLASS_FUNCTOR, (void *)this);
	}

	void CScriptVarFunctor::setFunctionSymbol(CScriptFunction *_irfs){
		m_value=_irfs;
	}

}

