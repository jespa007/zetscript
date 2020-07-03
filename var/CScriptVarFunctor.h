/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace ZetScript{

	#define NEW_FUNCTOR_VAR (new CScriptVarFunctor()) //setFunctionSymbol(ScriptFunction *_irfs)fun_struct)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassFunctor()),fun_struct))

	class  CScriptVarFunctor:public CScriptVar{

	public:
		//ScriptFunction *m_functorValue;

		CScriptVarFunctor(){}
		CScriptVarFunctor(ZetScript *_zs);
		void setFunctionSymbol(ScriptFunction *_irfs);


	};

}
