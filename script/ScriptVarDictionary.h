/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_STRUCT_VAR (new ScriptVarDictionary(this->zs)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassStruct())))

namespace zetscript{

	class 	ZetScript;
	class  	ScriptVarDictionary:public ScriptVar{

	public:
		ScriptVarDictionary(){}
		ScriptVarDictionary(ZetScript *_zs);

		virtual bool unrefSharedPtr();


		int size();
		void addAttr(const char *attr_name, StackElement  * v);
		void removeAttr(const char *attr_name);


		virtual void destroy();

		virtual ~ScriptVarDictionary();
	};

}
