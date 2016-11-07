#pragma once

#define NEW_NULL_VAR (new CNull(CScriptClassFactory::getInstance()->getRegisteredClassByIdx(CScriptClassFactory::getInstance()->getIdxClassNull())))

class CNull:public CScriptVariable{

public:
	CNull(){}
	CNull(tInfoRegisteredClass *info_registered_class);
};
