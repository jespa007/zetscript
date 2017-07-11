#pragma once

#define NEW_NULL_VAR (new CNull()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassNull())))
#define NULL_SYMBOL (CNull::getInstance())

class  CNull:public CScriptVariable{

	static CNull *null_symbol;
	CNull();
public:
	static CNull *getInstance();
	static void destroySingletons();


};
