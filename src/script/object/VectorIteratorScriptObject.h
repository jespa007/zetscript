/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_ITERATOR_VECTOR(so) (VectorIteratorScriptObject::newVectorIteratorScriptObject(so))

namespace zetscript{

	class  VectorIteratorScriptObject:public RefObjectScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static VectorIteratorScriptObject * newVectorIteratorScriptObject(VectorScriptObject *vo);

		//
		// Helpers
		//
		//----------------------------------------------

		VectorIteratorScriptObject();
		VectorIteratorScriptObject(VectorScriptObject *vo);
		void get();
		void next();
		bool end();
		~VectorIteratorScriptObject();
	private:

		zs_int idx;
		StackElement stk_key;
		StackElement stk_value;

		void setup();
	};

}
