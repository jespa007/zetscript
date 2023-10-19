/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_ARRAY_OBJECT(zs) (ArrayScriptObject::newArrayScriptObject(zs)) //ScriptType::getInstance()->getRegisteredClassByIdx(ScriptType::getInstance()->getIdxClassArray())))//,(void *)(new ArrayScriptObject())))

namespace zetscript{

	class ZetScript;
	class  ArrayScriptObject: public ContainerScriptObject{
	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ArrayScriptObject * 			newArrayScriptObject(ZetScript *zs);
		static ArrayScriptObject * 			concat(ZetScript *zs,ArrayScriptObject *v1,ArrayScriptObject *v2);



		//
		// Helpers
		//
		//----------------------------------------------

		ArrayScriptObject(ZetScript *_zs);

		template<typename _T>
		void 								set(int _pos, _T _element);

		template<typename _T>
		_T 									get(int _pos);

		template<typename _T>
		bool 								elementExtendsFrom(int _pos);

		virtual	StackElement * 				setStackElementByIndex(int _idx, StackElement *_element);
		virtual StackElement * 				getStackElementByIndex(int idx);

		bool 								eraseElementAt(int idx);
		void								eraseAllElements();

		virtual int 						length();
		virtual zs_vector<StackElement *> * getStkListElements();

		bool 								exists(StackElement  * stk);

		void 								concat(ArrayScriptObject *_v);

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
		virtual 							zs_string toString();
		virtual 							~ArrayScriptObject();
	protected:
		zs_vector<StackElement *> 			stk_elements;




	};
}
