#pragma once

#define NEW_STRING_VAR (new CString(CScriptClassFactory::getInstance()->getRegisteredClassByIdx(CScriptClassFactory::getInstance()->getIdxClassString())))

class CString: public CScriptVariable{
	
public:

	string m_value;

	CString(){}
	CString(tInfoRegisteredClass *info_registered_class);

	

};
