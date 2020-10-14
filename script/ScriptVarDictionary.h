/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_DICTIONARY_VAR (new ScriptVarDictionary(this->zs)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassStruct())))

namespace zetscript{

	class 	ZetScript;
	class  	ScriptVarDictionary:public ScriptVar{

	public:
		ScriptVarDictionary(){}
		ScriptVarDictionary(ZetScript *_zs);

		void 			addAttr(const char *attr_name, StackElement  * stk);
		static void     addAttrSf(ScriptVarDictionary *dic, const char *attr_name, StackElement  * stk){ dic->addAttr(attr_name,stk);}


		void removeAttr(const char *attr_name);
		static void removeAttrSf(ScriptVarDictionary *dic,const char *attr_name){ dic->removeAttr(attr_name);}

	};

}
