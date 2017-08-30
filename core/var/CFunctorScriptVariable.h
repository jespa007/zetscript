#pragma once

namespace zetscript{

	#define NEW_FUNCTOR_VAR (new CFunctorScriptVariable()) //setFunctionSymbol(CScriptFunctionObject *_irfs)fun_struct)) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassFunctor()),fun_struct))

	class  CFunctorScriptVariable:public CScriptVariable{

	public:
		//CScriptFunctionObject *m_functorValue;

		//CFunctorScriptVariable(){}
		CFunctorScriptVariable();
		void setFunctionSymbol(CScriptFunctionObject *_irfs);


	};

}
