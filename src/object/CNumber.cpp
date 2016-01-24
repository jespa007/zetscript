#include "script/zg_script.h"

CNumber * CNumber::Parse(const string & s){
	char *p;
	CNumber *n=NULL;
	double numberValue=strtod((char *)s.c_str(),&p);
	
	if(*p == '\0'){
		n=NEW_NUMBER();
		n->m_value = numberValue;
	}
	// TODO: develop exception handler.
	return n;
}
CNumber::CNumber(){ m_value=0;}
CNumber::CNumber(double v){m_value=v;}

string CNumber::toString(){string s; return s;}
