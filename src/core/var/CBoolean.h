#pragma once


#define NEW_BOOLEAN_VAR (new CBoolean(CScriptClassFactory::getInstance()->getRegisteredClassByIdx(CScriptClassFactory::getInstance()->getIdxClassBoolean()),NULL))

class CBoolean: public CScriptVariable{
	


public:

	bool m_boolValue;


	//static CBoolean * Parse(const string & );
	static bool * Parse(const string & );

	CBoolean();
	CBoolean(tInfoRegisteredClass *info_registered_class, bool *_value);

protected:

	virtual void setup();
	

};
