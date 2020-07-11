/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptVarFunctor::ScriptVarFunctor(ZetScript *_zs):ScriptVar(_zs){

	  //  m_classStr=typeid(ScriptVarFunctor).name();
	  //  m_pointerClassStr=typeid(ScriptVarFunctor *).name();


		//m_varType =VAR_TYPE::FUNCTION;
		//value=_irfs;
		this->Init(SCRIPT_CLASS_FUNCTOR(this), (void *)this);
	}

	void ScriptVarFunctor::setFunctionSymbol(ScriptFunction *_irfs){
		value=_irfs;
	}

}

