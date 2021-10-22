/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_ITERATOR_VECTOR(so) (ScriptObjectIteratorVector::newScriptObjectIteratorVector(so))

namespace zetscript{

	class  ScriptObjectIteratorVector:public ScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectIteratorVector * newScriptObjectIteratorVector(ScriptObjectVector *vo);

		//
		// Helpers
		//
		//----------------------------------------------

		ScriptObjectIteratorVector();
		ScriptObjectIteratorVector(ScriptObjectVector *vo);
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
