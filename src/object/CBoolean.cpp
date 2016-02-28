#include "script/zg_script.h"

CBoolean * CBoolean::Parse(const string & s){



		if(CStringUtils::toLower(s)=="true"){
			CBoolean *b=NEW_BOOLEAN();
			b->m_value=true;
			return b;
			
		}else if(CStringUtils::toLower(s)=="false"){
			CBoolean *b=NEW_BOOLEAN();
			b->m_value=false;
			return b;
		}

		// TODO: develop exception handler.
		return NULL;
}

bool * CBoolean::ParsePrimitive(const string & s){

	if(CStringUtils::toLower(s)=="true"){
		bool *b=new bool;
		*b=true;
		return b;

	}else if(CStringUtils::toLower(s)=="false"){
		bool *b=new bool;
		*b=false;
		return b;
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
