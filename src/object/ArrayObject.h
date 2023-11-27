/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_ARRAY_OBJECT(zs) (ArrayObject::newArrayObject(zs)) //Type::getInstance()->getRegisteredClassByIdx(Type::getInstance()->getIdxClassArray())))//,(void *)(new ArrayObject())))

namespace zetscript{

	class ZetScript;
	class  ArrayObject: public ContainerObject{
	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ArrayObject * 			newArrayObject(ZetScript *zs);
		static ArrayObject * 			concat(ZetScript *zs,ArrayObject *v1,ArrayObject *v2);



		//
		// Helpers
		//
		//----------------------------------------------

		ArrayObject(ZetScript *_zs);

		template<typename _T>
		void 								set(int _pos, _T _element);

		template<typename _T>
		_T 									get(int _pos);

		template<typename _T>
		bool 								elementInstanceOf(int _pos);

		virtual	StackElement * 				setStackElementByIndex(int _idx, StackElement *_element);
		virtual StackElement * 				getStackElementByIndex(int idx);

		bool 								eraseElementAt(int idx);
		void								eraseAllElements();

		virtual int 						length();
		virtual Vector<StackElement *> * getStkListElements();

		bool 								exists(StackElement  * stk);

		void 								concat(ArrayObject *_v);

		/**
		 * creates and insert new StackElement at the end of the vector
		 * @Return: New StackElement created
		 */
		StackElement *						newSlot();

		void 								pushStackElement(const StackElement  * stk);
		void 								pushStackElement(StackElement  stk);
		template<typename _T>
		void 								push(_T _element);
		void 								pop();
		virtual 							String toString();
		virtual 							~ArrayObject();
	protected:
		Vector<StackElement *> 			stk_elements;




	};
}
