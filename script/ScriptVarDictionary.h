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

		virtual void unrefSharedPtr();


		int size();
		static int sizeSf(ScriptVarDictionary *dic){ return dic->size();}
		void 			addAttr(const char *attr_name, StackElement  * stk);
		static void     addAttrSf(ScriptVarDictionary *dic, const char *attr_name, StackElement  * stk){ dic->addAttr(attr_name,stk);}


		void removeAttr(const char *attr_name);
		static void removeAttrSf(ScriptVarDictionary *dic,const char *attr_name){ dic->removeAttr(attr_name);}

		virtual void destroy();

		virtual ~ScriptVarDictionary();
	};

}
