/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_OBJECT_ITERATOR(so) (ScriptObjectObjectIterator::newScriptObjectObjectIterator(so))

namespace zetscript{

	class  ScriptObjectObjectIterator:public ScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectObjectIterator * newScriptObjectObjectIterator(ScriptObjectObject *so);

		//
		// Helpers
		//
		//----------------------------------------------

		ScriptObjectObjectIterator();
		ScriptObjectObjectIterator(ScriptObjectObject *so);
		void get();
		void next();
		bool end();

	private:

		ScriptObjectObject *oo;
		zs_map_iterator it;
		StackElement stk_key;


		void setup();

	};

}
