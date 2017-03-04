#include "../../CZetScript.h"


float *  CNumber::Parse(const string & s){
	char *p;bool ok=true;
	float *n=NULL;

	float numberValue=0;
	if(s!="0") {// trivial case
	  numberValue=strtof((char *)s.c_str(),&p);
	  ok = *p == '\0';
	}

    if(ok){
    	n=new float;
    	*n = numberValue;

	}

	// TODO: develop exception handler.
	return n;
}

void CNumber::setup(){

	CScriptVariable::setup();

	m_floatValue = 0;
	m_value = &m_floatValue;
}



CNumber::CNumber(){
	m_floatValue = 0;
	init(CScriptClass::getRegisteredClassNumber(), (void *)this);
}


