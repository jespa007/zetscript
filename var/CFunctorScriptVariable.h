/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zs{

	#define NEW_FUNCTOR_VAR (new CFunctorScriptVariable()) //setFunctionSymbol(CScriptFunction *_irfs)fun_struct)) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassFunctor()),fun_struct))

	class  CFunctorScriptVariable:public CScriptVariable{

	public:
		//CScriptFunction *m_functorValue;

		//CFunctorScriptVariable(){}
		CFunctorScriptVariable();
		void setFunctionSymbol(CScriptFunction *_irfs);


	};

}
