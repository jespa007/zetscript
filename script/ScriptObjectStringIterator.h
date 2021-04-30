/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_STRING_ITERATOR(so) (ScriptObjectStringIterator::newScriptObjectStringIterator(so))

namespace zetscript{

	class  ScriptObjectStringIterator:public ScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectStringIterator * newScriptObjectStringIterator(ScriptObjectString *so);

		//
		// Helpers
		//
		//----------------------------------------------

		ScriptObjectStringIterator();
		ScriptObjectStringIterator(ScriptObjectString *so);
		void get();
		void next();
		bool end();

	private:

		ScriptObjectString *so;
		int idx;
		StackElement stk_key;
		StackElement stk_value;

		void setup();
	};

}
