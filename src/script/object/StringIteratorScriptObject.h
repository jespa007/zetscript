/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_ITERATOR_STRING(zs,so) (StringIteratorScriptObject::newStringIteratorScriptObject(zs,so))

namespace zetscript{

	class  StringIteratorScriptObject:public RefObjectScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static StringIteratorScriptObject * newStringIteratorScriptObject(
				ZetScript *_zs
				,StringScriptObject *_so_ref
		);

		//
		// Helpers
		//
		//----------------------------------------------

		//StringIteratorScriptObject();
		StringIteratorScriptObject(
				ZetScript *_zs
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
