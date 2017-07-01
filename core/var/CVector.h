#pragma once

#define NEW_VECTOR_VAR (new CVector()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassVector())))//,(void *)(new CVector())))

class CVector: public CScriptVariable{

public:
	int _i_size;

	vector<tStackElement> m_objVector;

	CVector();
	//CVector(CScriptClass *info_registered_class);
	virtual bool unrefSharedPtr();
	virtual bool initSharedPtr();
	void add(const tStackElement  & v);
	void add(int * v);
	void add(bool * v);
	void add( string * v);
	void add(float * v);
	tStackElement *push();
	void add(CScriptVariable * v);
	int *size();




};
