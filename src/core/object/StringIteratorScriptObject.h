/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_STRING_ITERATOR_SCRIPT_OBJECT(zs,so) (StringIteratorScriptObject::newStringIteratorScriptObject(zs,so))

namespace zetscript{

	class  StringIteratorScriptObject:public RefScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static StringIteratorScriptObject * newStringIteratorScriptObject(
				ScriptEngine *_zs
				,StringScriptObject *_so_ref
		);

		//
		// Helpers
		//
		//----------------------------------------------

		//StringIteratorScriptObject();
		StringIteratorScriptObject(
				ScriptEngine *_zs
				,StringScriptObject *_so_ref
		);
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
