#pragma once

#define NEW_UNDEFINED_VAR (new CUndefinedScriptVariable()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassUndefined())))
#define UNDEFINED_SYMBOL (CUndefinedScriptVariable::getInstance())

namespace zetscript{

	class  CUndefinedScriptVariable:public CScriptVariable{
		static CUndefinedScriptVariable *undefined_symbol;
		CUndefinedScriptVariable();
	public:
		static CUndefinedScriptVariable *getInstance();
		static void destroySingletons();
		//CUndefinedScriptVariable(CScriptClass *info_registered_class);

	};

}
