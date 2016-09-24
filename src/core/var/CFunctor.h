#pragma once

#define NEW_FUNCTOR_VAR(fun_struct) (new CFunctor(CScriptClassFactory::getInstance()->getRegisteredClassByIdx(CScriptClassFactory::getInstance()->getIdxClassFunctor()),fun_struct))

class CFunctor:public CScriptVariable{

public:
	tInfoRegisteredFunctionSymbol *m_value;
	CFunctor(tInfoRegisteredClass *info_registered_class,tInfoRegisteredFunctionSymbol *_irfs);

};
