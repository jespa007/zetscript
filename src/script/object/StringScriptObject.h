/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_STRING(zs) (StringScriptObject::newStringScriptObject(zs)) //ScriptType::getInstance()->getRegisteredClassByIdx(ScriptType::getInstance()->getIdxClassString()),NULL))


namespace zetscript{

	class  StringScriptObject: public ScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static StringScriptObject * newStringScriptObject(ZetScript *zs, const zs_string &_str="");
		static StringScriptObject * newShareableStringScriptObject(ZetScript	*_zs);

		static StringScriptObject * newStringScriptObjectAddStk(ZetScript *zs,StackElement *stk_result_op1,StackElement *stk_result_op2);
		static StringScriptObject * format(ZetScript *zs,StackElement *str, StackElement *args);

		//
		// Helpers
		//
		//----------------------------------------------

		void 		*	value;	// 8


		StringScriptObject();

		void set(const zs_string & _s);
		virtual int length();
		virtual zs_string toString();

		StringScriptObject *sub(StringScriptObject *s1);


	protected:


	private:
		zs_string default_str_value;

		void setup();
	};

}
