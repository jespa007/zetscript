#include "core/zg_core.h"
/*
CBoolean * CBoolean::Parse(const string & s){



		if(CStringUtils::toLower(s)=="true"){
			CBoolean *b=new CBoolean();
			b->m_value=true;
			return b;
			
		}else if(CStringUtils::toLower(s)=="false"){
			CBoolean *b=new CBoolean();
			b->m_value=false;
			return b;
		}

		// TODO: develop exception handler.
		return NULL;
}*/

bool * CBoolean::Parse(const string & s){

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


CBoolean::CBoolean(tInfoRegisteredClass *info_registered_class):CScriptVariable(info_registered_class){

    //m_classStr=typeid(CBoolean).name();
    //m_pointerClassStr=typeid(CBoolean *).name();

	//m_varType = CVariable::VAR_TYPE::BOOLEAN;
	m_value = false;
	//m_ptr=&m_value;
}



