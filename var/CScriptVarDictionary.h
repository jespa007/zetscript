/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_STRUCT_VAR (new CScriptVarDictionary(this->zs)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassStruct())))

namespace ZetScript{

	class  CScriptVarDictionary:public CScriptVar{



	public:
		CScriptVarDictionary(){}
		CScriptVarDictionary(ZetScript *_zs);

		virtual bool unrefSharedPtr();



		int size();
		void add_attr(const char *attr_name, StackElement  * v);
		void remove_attr(const char *attr_name);


		virtual void destroy();

		virtual ~CScriptVarDictionary();
	};

}
