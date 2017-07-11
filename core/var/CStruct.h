
#define NEW_STRUCT_VAR (new CStruct()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassStruct())))

class  CStruct:public CScriptVariable{

public:
	CStruct();

	virtual bool unrefSharedPtr();
	virtual bool initSharedPtr();


};
