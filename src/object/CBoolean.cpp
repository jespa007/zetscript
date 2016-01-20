#include "script/zg_script.h"

CBoolean * CBoolean::Parse(const string & s){
		if(CStringUtils::toLower(s)=="true"){
			return new CBoolean(true);
			
		}else if(CStringUtils::toLower(s)=="false"){
			return new CBoolean(false);
		}

		// TODO: develop exception handler.
		return NULL;
}

CBoolean::CBoolean(){
	m_value = false;
}
CBoolean::CBoolean(bool b){
	m_value = b;
}

string CBoolean::toString(){string s; return s;}
