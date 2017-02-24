#pragma once

#define NEW_NULL_VAR (new CNull()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassNull())))

class CNull:public CScriptVariable{


public:
	CNull();


};
