#pragma once


#define NEW_NUMBER_VAR (new CNumber()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassNumber()),NULL))

class CNumber: public CScriptVariable{




public:
	float m_floatValue;

	//static CNumber * Parse(const string & );
	static float * Parse(const string & );

	CNumber();


protected:

	virtual void setup();
	

};
