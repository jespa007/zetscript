#pragma once

#define NEW_VECTOR_VAR (new CVector()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassVector())))//,(void *)(new CVector())))

class CVector: public CScriptVariable{

public:
	int _i_size;

	vector<CScriptVariable *> m_objVector;

	CVector();
	//CVector(tInfoScriptClass *info_registered_class);
	virtual bool unrefSharedPtr();
	virtual bool refSharedPtr();
	void add(CScriptVariable *v);
	int *size();

	~CVector();


};
