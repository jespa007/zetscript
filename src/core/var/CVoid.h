#pragma once

#define NEW_VOID_VAR (new CVoid(CScriptClassFactory::getInstance()->getRegisteredClassByIdx(CScriptClassFactory::getInstance()->getIdxClassVoid())))

class CVoid:public CScriptVariable{

public:
	CVoid(){}
	CVoid(tInfoRegisteredClass *info_registered_class);
};
