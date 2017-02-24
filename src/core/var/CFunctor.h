#pragma once

#define NEW_FUNCTOR_VAR (new CFunctor()) //setFunctionSymbol(tScriptFunctionObject *_irfs)fun_struct)) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassFunctor()),fun_struct))

class CFunctor:public CScriptVariable{

public:
	//tScriptFunctionObject *m_functorValue;

	//CFunctor(){}
	CFunctor();
	void setFunctionSymbol(tScriptFunctionObject *_irfs);


};
