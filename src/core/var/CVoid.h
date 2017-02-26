#pragma once

#define NEW_VOID_VAR (new CVoid(CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassVoid())))

class CVoid:public CScriptVariable{

	static CVoid *void_symbol;
	CVoid();
public:
	static CVoid *getInstance();
	static void destroySingletons();


};
