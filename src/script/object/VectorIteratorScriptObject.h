/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_ITERATOR_VECTOR(zs,so) (VectorIteratorScriptObject::newVectorIteratorScriptObject(zs,so))

namespace zetscript{

	class  VectorIteratorScriptObject:public RefObjectScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static VectorIteratorScriptObject * newVectorIteratorScriptObject(ZetScript *_zs, VectorScriptObject *_so_ref);

		//
		// Helpers
		//
		//----------------------------------------------

		VectorIteratorScriptObject(ZetScript *_zs,VectorScriptObject *_so_ref);
		void get();
		void next();
		bool end();
		~VectorIteratorScriptObject();
	private:

		zs_int idx;
		StackElement stk_key;
		StackElement stk_value;

	};

}
