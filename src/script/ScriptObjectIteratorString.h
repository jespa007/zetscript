/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_ITERATOR_STRING(so) (ScriptObjectIteratorString::newScriptObjectIteratorString(so))

namespace zetscript{

	class  ScriptObjectIteratorString:public ScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectIteratorString * newScriptObjectIteratorString(ScriptObjectString *so);

		//
		// Helpers
		//
		//----------------------------------------------

		ScriptObjectIteratorString();
		ScriptObjectIteratorString(ScriptObjectString *so);
		void get();
		void next();
		bool end();
		//~ScriptObjectIteratorString();

	private:

		ScriptObjectString *so;
		zs_int idx;
		StackElement stk_key;
		StackElement stk_value;

		void setup();
	};

}
