/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_DICTIONARY_ITERATOR_SCRIPT_OBJECT(zs,so) (DictionaryIteratorScriptObject::newDictionaryIteratorScriptObject(zs,so))

namespace zetscript{

	class  DictionaryIteratorScriptObject:public RefDictionaryScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static DictionaryIteratorScriptObject * newDictionaryIteratorScriptObject(
				ZetScript *_zs
				, DictionaryScriptObject *_so_ref
		);

		//
		// Helpers
		//
		//----------------------------------------------

		DictionaryIteratorScriptObject(
				ZetScript *_zs
				,DictionaryScriptObject *_so_ref
		);
		void get();
		void next();
		bool end();

		~DictionaryIteratorScriptObject();

	private:

		StackElement stk_key;
		MapStringIterator it;
		void setup();

	};

}
