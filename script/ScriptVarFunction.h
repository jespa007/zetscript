/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	#define NEW_FUNCTOR_VAR (new ScriptVarFunction()) //setFunctionSymbol(ScriptFunction *_irfs)fun_struct)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassFunctor()),fun_struct))

	class  ScriptVarFunction:public ScriptVar{

	public:

		ScriptVarFunction(){}
		ScriptVarFunction(ZetScript *_zs);
		void setFunctionSymbol(ScriptFunction *_irfs);



	};

}
