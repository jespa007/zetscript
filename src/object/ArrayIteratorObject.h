/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_ITERATOR_ARRAY(zs,so) (ArrayIteratorObject::newArrayIteratorObject(zs,so))

namespace zetscript{

	class  ArrayIteratorObject:public RefObjectObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ArrayIteratorObject * newArrayIteratorObject(ZetScript *_zs, ArrayObject *_so_ref);

		//
		// Helpers
		//
		//----------------------------------------------

		ArrayIteratorObject(ZetScript *_zs,ArrayObject *_so_ref);
		void get();
		void next();
		bool end();
		~ArrayIteratorObject();
	private:

		zs_int idx;
		StackElement stk_key;
		StackElement stk_value;

	};

}
