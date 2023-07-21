/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_ITERATOR_OBJECT(zs,so) (ObjectIteratorScriptObject::newObjectIteratorScriptObject(zs,so))

namespace zetscript{

	class  ObjectIteratorScriptObject:public RefObjectScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ObjectIteratorScriptObject * newObjectIteratorScriptObject(
				ZetScript *_zs
				, ObjectScriptObject *_so_ref
		);

		//
		// Helpers
		//
		//----------------------------------------------

		ObjectIteratorScriptObject(
				ZetScript *_zs
				,ObjectScriptObject *_so_ref
		);
		void get();
		void next();
		bool end();

		~ObjectIteratorScriptObject();

	private:

		StackElement stk_key;
		zs_map_iterator it;
		void setup();

	};

}
