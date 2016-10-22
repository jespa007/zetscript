#pragma once


#define NEW_NUMBER_VAR (new CNumber(CScriptClassFactory::getInstance()->getRegisteredClassByIdx(CScriptClassFactory::getInstance()->getIdxClassNumber()),NULL))

class CNumber: public CScriptVariable{

public:
	float m_floatValue;

	//static CNumber * Parse(const string & );
	static float * Parse(const string & );

	CNumber();
	CNumber(tInfoRegisteredClass *info_registered_class,void *_value);

protected:

	virtual void setup();


	

};
