#pragma once


#define NEW_INTEGER_VAR (new CInteger(CScriptClassFactory::getInstance()->getRegisteredClassByIdx(CScriptClassFactory::getInstance()->getIdxClassInteger())))

class CInteger: public CScriptVariable{

public:
	int m_value;


	//static CInteger * Parse(const string & );
	static int * Parse(const string & );

	CInteger(){}
	CInteger(tInfoRegisteredClass *info_registered_class);


	

};
