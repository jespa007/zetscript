#pragma once


#define NEW_BOOLEAN_VAR (new CBoolean(CScriptClassFactory::getInstance()->getRegisteredClassByIdx(CScriptClassFactory::getInstance()->getIdxClassBoolean())))

class CBoolean: public CScriptVariable{
	

public:

	bool m_value;


	//static CBoolean * Parse(const string & );
	static bool * Parse(const string & );

	CBoolean(tInfoRegisteredClass *info_registered_class);


	

};
