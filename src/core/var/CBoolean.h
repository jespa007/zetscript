#pragma once


#define NEW_BOOLEAN_VAR (new CBoolean()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassBoolean()),NULL))

class CBoolean: public CScriptVariable{
	
public:
	bool m_boolValue;

	//static CBoolean * Parse(const string & );
	static bool * Parse(const string & );

	CBoolean();
	CBoolean(bool _value);
	//CBoolean(tInfoScriptClass *info_registered_class, bool *_value);

protected:

	virtual void setup();
	

};
