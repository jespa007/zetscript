#pragma once

#define NEW_VECTOR_VAR (new CVector(CScriptClassFactory::getInstance()->getRegisteredClassByIdx(CScriptClassFactory::getInstance()->getIdxClassVector())))

class CVector: public CScriptVariable{

public:
	int _i_size;

	vector<CScriptVariable *> m_objVector;

	CVector(){}
	CVector(tInfoRegisteredClass *info_registered_class);

	int *size();

	~CVector();
};
