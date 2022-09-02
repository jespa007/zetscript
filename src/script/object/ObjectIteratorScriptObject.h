/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_ITERATOR_OBJECT(so) (ObjectIteratorScriptObject::newObjectIteratorScriptObject(so))

namespace zetscript{

	class  ObjectIteratorScriptObject:public RefObjectScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ObjectIteratorScriptObject * newObjectIteratorScriptObject(ObjectScriptObject *so);

		//
		// Helpers
		//
		//----------------------------------------------

		ObjectIteratorScriptObject();
		ObjectIteratorScriptObject(ObjectScriptObject *so);
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
