#pragma once

#define NEW_STRING_VAR (new CString()) //CScriptClassFactory::getInstance()->getRegisteredClassByIdx(CScriptClassFactory::getInstance()->getIdxClassString()),NULL))

class CString: public CScriptVariable{
	
public:



	CString();
	CString(const string & s);

protected:
	virtual void setup();

};
