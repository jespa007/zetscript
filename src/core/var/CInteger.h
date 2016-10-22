#pragma once


#define NEW_INTEGER_VAR (new CInteger(CScriptClassFactory::getInstance()->getRegisteredClassByIdx(CScriptClassFactory::getInstance()->getIdxClassInteger()),NULL))

class CInteger: public CScriptVariable{

public:
	int m_intValue;
	int i;

	//static CInteger * Parse(const string & );
	static int * Parse(const string & );

	CInteger();


	// assign from external pointer ...
	CInteger(tInfoRegisteredClass *info_registered_class,void *_value);

protected:

	virtual void setup();

	

};
