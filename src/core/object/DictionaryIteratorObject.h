/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_ITERATOR_OBJECT(zs,so) (ObjectIteratorObject::newObjectIteratorObject(zs,so))

namespace zetscript{

	class  ObjectIteratorObject:public RefObjectObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ObjectIteratorObject * newObjectIteratorObject(
				ZetScript *_zs
				, DictionaryObject *_so_ref
		);

		//
		// Helpers
		//
		//----------------------------------------------

		ObjectIteratorObject(
				ZetScript *_zs
				,DictionaryObject *_so_ref
		);
		void get();
		void next();
		bool end();

		~ObjectIteratorObject();

	private:

		StackElement stk_key;
		MapStringIterator it;
		void setup();

	};

}
