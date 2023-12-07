/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_STRING_SCRIPT_OBJECT StringScriptObject::newStringScriptObject


namespace zetscript{
	class  ArrayScriptObject;
	class  StringScriptObject: public ScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static StringScriptObject * newStringScriptObject(
			ZetScript *_zs
			, const String &_str=""
		);

		static StringScriptObject * newStringScriptObjectAddStk(
			ZetScript *_zs
			,StackElement *_stk_result_op1
			,StackElement *_stk_result_op2
		);

		static StringScriptObject * format(
			ZetScript *_zs,StackElement *_stk
			, StackElement *_args
		);

		//
		// Helpers
		//
		//----------------------------------------------

		String		*	str_ptr;	// 8


		StringScriptObject(ZetScript *_zs, const String & _str="");

		void set(const String & _s);
		const String & get();
		String * getPtr();
		const char *getConstChar();
		virtual int length();
		virtual String toString();

		StringScriptObject *sub(StringScriptObject *s1);


	protected:


	private:
		String default_str_value;

		void setup();
	};

}
