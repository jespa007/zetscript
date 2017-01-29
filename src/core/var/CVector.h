#pragma once

#define NEW_VECTOR_VAR (new CVector()) //CScriptClassFactory::getInstance()->getRegisteredClassByIdx(CScriptClassFactory::getInstance()->getIdxClassVector())))//,(void *)(new CVector())))

class CVector: public CScriptVariable{

public:
	int _i_size;

	vector<CScriptVariable *> m_objVector;

	CVector();
	//CVector(tInfoRegisteredClass *info_registered_class);
	virtual bool unrefSharedPtr();
	virtual bool refSharedPtr();
	void add(CScriptVariable *v);
	int *size();

	~CVector();


};
