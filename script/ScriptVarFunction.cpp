/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptVarFunction::ScriptVarFunction(ZetScript *_zs):ScriptVar(_zs){

	  //  m_classStr=typeid(ScriptVarFunction).name();
	  //  m_pointerClassStr=typeid(ScriptVarFunction *).name();


		//m_varType =VAR_TYPE::FUNCTION;
		//value=_irfs;
		this->init(SCRIPT_CLASS_FUNCTOR(this), (void *)this);
	}

	void ScriptVarFunction::setFunctionSymbol(ScriptFunction *_irfs){
		value=_irfs;
	}

}

