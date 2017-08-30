#pragma once

namespace zetscript{

	#define NEW_NULL_VAR (new CNullScriptVariable()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassNull())))
	#define NULL_SYMBOL (CNullScriptVariable::getInstance())

	class  CNullScriptVariable:public CScriptVariable{

		static CNullScriptVariable *null_symbol;
		CNullScriptVariable();
	public:
		static CNullScriptVariable *getInstance();
		static void destroySingletons();


	};

}
