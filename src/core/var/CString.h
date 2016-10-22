#pragma once

#define NEW_STRING_VAR (new CString(CScriptClassFactory::getInstance()->getRegisteredClassByIdx(CScriptClassFactory::getInstance()->getIdxClassString()),NULL))

class CString: public CScriptVariable{
	
public:

	string m_stringValue;

	CString();
	CString(tInfoRegisteredClass *info_registered_class, string *_value);

protected:

	virtual void setup();
	

};
