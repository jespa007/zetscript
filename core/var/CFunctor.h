#pragma once

#define NEW_FUNCTOR_VAR (new CFunctor()) //setFunctionSymbol(CScriptFunctionObject *_irfs)fun_struct)) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassFunctor()),fun_struct))

class CFunctor:public CScriptVariable{

public:
	//CScriptFunctionObject *m_functorValue;

	//CFunctor(){}
	CFunctor();
	void setFunctionSymbol(CScriptFunctionObject *_irfs);


};
