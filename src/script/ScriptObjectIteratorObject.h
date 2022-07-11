/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_ITERATOR_OBJECT(so) (ScriptObjectIteratorObject::newScriptObjectIteratorObject(so))

namespace zetscript{

	class  ScriptObjectIteratorObject:public ScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectIteratorObject * newScriptObjectIteratorObject(ScriptObjectObject *so);

		//
		// Helpers
		//
		//----------------------------------------------

		ScriptObjectIteratorObject();
		ScriptObjectIteratorObject(ScriptObjectObject *so);
		void get();
		void next();
		bool end();

		//~ScriptObjectIteratorObject();

	private:

		ScriptObjectObject *oo;
		StackElement stk_key;
		zs_map_iterator it;
		void setup();

	};

}
