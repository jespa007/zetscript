#pragma once

#define NEW_UNDEFINED_VAR (new CUndefined()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassUndefined())))
#define UNDEFINED_SYMBOL (CUndefined::getInstance())


class ZETSCRIPT_API CUndefined:public CScriptVariable{
	static CUndefined *undefined_symbol;
	CUndefined();
public:
	static CUndefined *getInstance();
	static void destroySingletons();
	//CUndefined(CScriptClass *info_registered_class);

};
