/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_ITERATOR_ASSIGNRING(zs,so) (StringIteratorObject::newStringIteratorObject(zs,so))

namespace zetscript{

	class  StringIteratorObject:public RefObjectObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static StringIteratorObject * newStringIteratorObject(
				ZetScript *_zs
				,StringObject *_so_ref
		);

		//
		// Helpers
		//
		//----------------------------------------------

		//StringIteratorObject();
		StringIteratorObject(
				ZetScript *_zs
				,StringObject *_so_ref
		);
		void get();
		void next();
		bool end();
		~StringIteratorObject();

	private:

		zs_int idx;
		StackElement stk_key;
		StackElement stk_value;

		void setup();
	};

}
