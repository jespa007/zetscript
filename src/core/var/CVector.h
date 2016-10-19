#pragma once

#define NEW_VECTOR_VAR (new CVector(CScriptClassFactory::getInstance()->getRegisteredClassByIdx(CScriptClassFactory::getInstance()->getIdxClassVector())))

class CVector: public CScriptVariable{

public:

	vector<CScriptVariable *> m_value;

	CVector(){}
	CVector(tInfoRegisteredClass *info_registered_class);

	~CVector();
};
