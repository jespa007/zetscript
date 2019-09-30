/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	#define NEW_FUNCTOR_VAR (new CScriptVarFunctor()) //setFunctionSymbol(CScriptFunction *_irfs)fun_struct)) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassFunctor()),fun_struct))

	class  CScriptVarFunctor:public CScriptVar{

	public:
		//CScriptFunction *m_functorValue;

		CScriptVarFunctor(){}
		CScriptVarFunctor(CZetScript *_zs);
		void setFunctionSymbol(CScriptFunction *_irfs);


	};

}
