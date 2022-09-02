/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_ITERATOR_STRING(so) (StringIteratorScriptObject::newStringIteratorScriptObject(so))

namespace zetscript{

	class  StringIteratorScriptObject:public RefObjectScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static StringIteratorScriptObject * newStringIteratorScriptObject(StringScriptObject *so);

		//
		// Helpers
		//
		//----------------------------------------------

		StringIteratorScriptObject();
		StringIteratorScriptObject(StringScriptObject *so);
		void get();
		void next();
		bool end();
		~StringIteratorScriptObject();

	private:

		zs_int idx;
		StackElement stk_key;
		StackElement stk_value;

		void setup();
	};

}
