
#define NEW_STRUCT (new CStruct()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassStruct())))

class CStruct:public CScriptVariable{

public:
	CStruct();

	virtual bool unrefSharedPtr();
	virtual bool refSharedPtr();


};
