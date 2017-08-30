#pragma once

#define NEW_STRING_VAR (new CStringScriptVariable()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassString()),NULL))


namespace zetscript{

	class  CStringScriptVariable: public CScriptVariable{

	public:

		CStringScriptVariable();
		CStringScriptVariable(const string & s);

	protected:
		virtual void setup();
	
	};

}
