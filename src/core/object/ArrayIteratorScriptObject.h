/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_ARRAY_ITERATOR_SCRIPT_OBJECT(zs,so) (ArrayIteratorScriptObject::newArrayIteratorObject(zs,so))

namespace zetscript{

	class  ArrayIteratorScriptObject:public RefObjectScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ArrayIteratorScriptObject * newArrayIteratorObject(ZetScript *_zs, ArrayScriptObject *_so_ref);

		//
		// Helpers
		//
		//----------------------------------------------

		ArrayIteratorScriptObject(ZetScript *_zs,ArrayScriptObject *_so_ref);
		void get();
		void next();
		bool end();
		~ArrayIteratorScriptObject();
	private:

		zs_int idx;
		StackElement stk_key;
		StackElement stk_value;

	};

}
