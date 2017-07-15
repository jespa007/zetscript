#pragma once

#define NEW_STRUCT_VAR (new CStruct()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassStruct())))

namespace zetscript{

	class  CStruct:public CScriptVariable{

	public:
		CStruct();

		virtual bool unrefSharedPtr();
		virtual bool initSharedPtr();


	};

}
