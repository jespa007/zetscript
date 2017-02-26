#include "../../CZetScript.h"
/*
CNumber * CNumber::Parse(const string & s){
	char *p;bool ok=true;
	CNumber *n=NULL;
	
	double numberValue=0;
	if(s!="0") {// trivial case
		//print_info_cr("not %i %i! %s",s[0],s[1]);
	  numberValue=strtod((char *)s.c_str(),&p);
	  ok = *p == '\0';
	}

    if(ok){
		n=new CNumber();
		n->m_value = numberValue;
	}

	// TODO: develop exception handler.
	return n;
}*/

float *  CNumber::Parse(const string & s){
	char *p;bool ok=true;
	float *n=NULL;

	float numberValue=0;
	if(s!="0") {// trivial case
		//print_info_cr("not %i %i! %s",s[0],s[1]);
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


