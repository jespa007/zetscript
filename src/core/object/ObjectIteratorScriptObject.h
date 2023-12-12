/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_ITERATOR_SCRIPT_OBJECT(zs,so) (ObjectIteratorScriptObject::newObjectIteratorScriptObject(zs,so))

namespace zetscript{

	class  ObjectIteratorScriptObject:public RefScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ObjectIteratorScriptObject * newObjectIteratorScriptObject(
				ScriptEngine *_script_engine
				, ObjectScriptObject *_so_ref
		);

		//
		// Helpers
		//
		//----------------------------------------------

		ObjectIteratorScriptObject(
				ScriptEngine *_script_engine
				,ObjectScriptObject *_so_ref
		);
		void get();
		void next();
		bool end();

		~ObjectIteratorScriptObject();

	private:

		StackElement stk_key;
		MapStringIterator it;
		void setup();

	};

}
