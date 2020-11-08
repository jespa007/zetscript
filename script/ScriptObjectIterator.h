/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	typedef enum {
		IDX_ITERATOR_ELEMENT_OBJECT = 0,
		IDX_ITERATOR_ELEMENT_KEY,
		IDX_ITERATOR_ELEMENT_VALUE,
		IDX_ITERATOR_ELEMENT_MAX,
	}IdxIteratorElement;

	class  ScriptObjectIterator: public ScriptObject{

	public:
		std::string str_value;

		ScriptObjectIterator(ZetScript *_zs);

		static void    			pushSf(ScriptObjectIterator *si,StackElement  * stk);
		static void				nextSf(ScriptObjectIterator *si);
		static bool				endSf(ScriptObjectIterator *si);

	protected:

		void push(StackElement  * _stk);
		void next();
		bool end();

		StackElement *stk_it_elements[IDX_ITERATOR_ELEMENT_MAX]; // 1st vector, 2nd key, 3rd value (optional)
		int n_it_elements;
		ScriptObject *object;

	};

}
