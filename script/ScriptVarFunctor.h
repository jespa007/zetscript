/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	#define NEW_FUNCTOR_VAR (new ScriptVarFunctor()) //setFunctionSymbol(ScriptFunction *_irfs)fun_struct)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassFunctor()),fun_struct))

	class  ScriptVarFunctor:public ScriptVar{

	public:
		//ScriptFunction *m_functorValue;

		ScriptVarFunctor(){}
		ScriptVarFunctor(ZetScript *_zs);
		void setFunctionSymbol(ScriptFunction *_irfs);


	};

}
