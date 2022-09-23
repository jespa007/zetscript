/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_STRING_OBJECT(zs) (StringScriptObject::newStringScriptObject(zs)) //ScriptType::getInstance()->getRegisteredClassByIdx(ScriptType::getInstance()->getIdxClassString()),NULL))


namespace zetscript{

	class  StringScriptObject: public ScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static StringScriptObject * newStringScriptObject(ZetScript *_zs, const zs_string &_str="");
		static StringScriptObject * newStringScriptObjectAddStk(ZetScript *_zs,StackElement *_stk_result_op1,StackElement *_stk_result_op2);
		static StringScriptObject * format(ZetScript *_zs,StackElement *_str, StackElement *_args);

		//
		// Helpers
		//
		//----------------------------------------------

		zs_string		*	str_ptr;	// 8


		StringScriptObject(ZetScript *_zs);

		void set(const zs_string & _s);
		const zs_string & get();
		const char *getConstChar();
		virtual int length();
		virtual zs_string toString();

		StringScriptObject *sub(StringScriptObject *s1);


	protected:


	private:
		zs_string default_str_value;

		void setup();
	};

}
