/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../CZetScript.h"

namespace zetscript{

	CFunctorScriptVariable::CFunctorScriptVariable(){

	  //  m_classStr=typeid(CFunctorScriptVariable).name();
	  //  m_pointerClassStr=typeid(CFunctorScriptVariable *).name();


		//m_varType =VAR_TYPE::FUNCTION;
		//m_value=_irfs;
		this->init(SCRIPT_CLASS_FUNCTOR, (void *)this);
	}

	void CFunctorScriptVariable::setFunctionSymbol(CScriptFunction *_irfs){
		m_value=_irfs;
	}

}

