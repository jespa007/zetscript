#include "script/zg_script.h"

CNumber * CNumber::Parse(const string & s){
	char *p;
	double numberValue=strtod((char *)s.c_str(),&p);
	
	if(*p == '\0'){
		return new CNumber(numberValue);
	}
	// TODO: develop exception handler.
	return NULL;
}
CNumber::CNumber(){ m_value=0;}
CNumber::CNumber(double v){m_value=v;}

string CNumber::toString(){string s; return s;}
