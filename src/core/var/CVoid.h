#pragma once

#define NEW_VOID_VAR (new CVoid(CScriptClassFactory::getInstance()->getRegisteredClassByIdx(CScriptClassFactory::getInstance()->getIdxClassVoid())))

class CVoid:public CScriptVariable{

public:
	CVoid();


};
