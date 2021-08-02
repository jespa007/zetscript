/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_VECTOR_ITERATOR(so) (ScriptObjectVectorIterator::newScriptObjectVectorIterator(so))

namespace zetscript{

	class  ScriptObjectVectorIterator:public ScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectVectorIterator * newScriptObjectVectorIterator(ScriptObjectVector *vo);

		//
		// Helpers
		//
		//----------------------------------------------

		ScriptObjectVectorIterator();
		ScriptObjectVectorIterator(ScriptObjectVector *vo);
		void get();
		void next();
		bool end();

	private:

		ScriptObjectVector *vo;
		zs_int idx;
		StackElement stk_key;
		StackElement stk_value;

		void setup();
	};

}
