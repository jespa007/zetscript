#pragma once

#define NEW_UNDEFINED_VAR (new CUndefined()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassUndefined())))

class CUndefined:public CScriptVariable{

public:
	CUndefined();
	//CUndefined(tInfoScriptClass *info_registered_class);

};
