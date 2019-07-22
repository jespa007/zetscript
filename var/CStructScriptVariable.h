/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_STRUCT_VAR (new CStructScriptVariable()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassStruct())))

namespace zetscript{

	class  CStructScriptVariable:public CScriptVariable{



	public:
		CStructScriptVariable();

		virtual bool unrefSharedPtr();



		int size();
		void add_attr(const char *attr_name, StackElement  * v);
		void remove_attr(const char *attr_name);


		virtual void destroy();

		virtual ~CStructScriptVariable();
	};

}
