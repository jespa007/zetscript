#pragma once

#define NEW_FUNCTOR_VAR (new CFunctor()) //setRegisteredFunctionSymbol(tInfoRegisteredFunctionSymbol *_irfs)fun_struct)) //CScriptClassFactory::getInstance()->getRegisteredClassByIdx(CScriptClassFactory::getInstance()->getIdxClassFunctor()),fun_struct))

class CFunctor:public CScriptVariable{

public:
	//tInfoRegisteredFunctionSymbol *m_functorValue;

	//CFunctor(){}
	CFunctor();
	void setRegisteredFunctionSymbol(tInfoRegisteredFunctionSymbol *_irfs);


};
