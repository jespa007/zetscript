/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_STRING_ITERATOR_SCRIPT_OBJECT(script_engine,so) (StringIteratorScriptObject::newStringIteratorScriptObject(script_engine,so))

namespace zetscript{

	class  StringIteratorScriptObject:public RefScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static StringIteratorScriptObject * newStringIteratorScriptObject(
				zetscript::ScriptEngine *_script_engine
				,StringScriptObject *_so_ref
		);

		//
		// Helpers
		//
		//----------------------------------------------

		//StringIteratorScriptObject();
		StringIteratorScriptObject(
				zetscript::ScriptEngine *_script_engine
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
