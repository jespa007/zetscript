#pragma once


#define NEW_INTEGER_VAR (new CInteger()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassInteger()),NULL))

class CInteger: public CScriptVariable{

public:
	int m_intValue;


	//static CInteger * Parse(const string & );
	static int * Parse(const string & );

	CInteger();

	CInteger(int i);

protected:

	virtual void setup();
	virtual string * toString();//{ return &this->m_strValue;}

};
