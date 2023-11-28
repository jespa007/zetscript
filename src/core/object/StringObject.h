/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_STRING_OBJECT StringObject::newStringObject


namespace zetscript{

	class  StringObject: public Object{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static StringObject * newStringObject(
			ZetScript *_zs
			, const String &_str=""
		);

		static StringObject * newStringObjectAddStk(
			ZetScript *_zs
			,StackElement *_stk_result_op1
			,StackElement *_stk_result_op2
		);

		static StringObject * format(
			ZetScript *_zs,StackElement *_stk
			, StackElement *_args
		);

		//
		// Helpers
		//
		//----------------------------------------------

		String		*	str_ptr;	// 8


		StringObject(ZetScript *_zs, const String & _str="");

		void set(const String & _s);
		const String & get();
		const char *getConstChar();
		virtual int length();
		virtual String toString();

		StringObject *sub(StringObject *s1);


	protected:


	private:
		String default_str_value;

		void setup();
	};

}
