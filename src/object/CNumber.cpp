#include "script/zg_script.h"

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
}

float *  CNumber::ParsePrimitive(const string & s){
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


CNumber::CNumber(){ m_value=0;}
CNumber::CNumber(float v){m_value=v;}

string CNumber::toString(){string s; return s;}
