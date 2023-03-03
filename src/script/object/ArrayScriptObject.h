/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_ARRAY_OBJECT(zs) (ArrayScriptObject::newArrayScriptObject(zs)) //ScriptType::getInstance()->getRegisteredClassByIdx(ScriptType::getInstance()->getIdxClassArray())))//,(void *)(new ArrayScriptObject())))

namespace zetscript{

	class CZetgine;
	class  ArrayScriptObject: public ContainerScriptObject{
	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ArrayScriptObject * newArrayScriptObject(ZetScript *zs);
		static ArrayScriptObject * concat(ZetScript *zs,ArrayScriptObject *v1,ArrayScriptObject *v2);



		//
		// Helpers
		//
		//----------------------------------------------

		ArrayScriptObject(ZetScript *_zs);

		virtual StackElement * 				getUserElementAt(int idx);
		bool 								eraseUserElementAt(int idx);
		void								eraseAllUserElements();

		virtual int length();
		virtual zs_vector<StackElement *> * getStkUserListElements();

		bool 								exists(StackElement  * stk);

		void 								concat(ArrayScriptObject *_v);

		/**
		 * creates and insert new StackElement at the end of the vector
		 * @Return: New StackElement created
		 */
		StackElement *						newSlot();

		void 								push(const StackElement  * stk);
		void 								pushInteger(zs_int _value);
		void 								pushFloat(zs_float _value);
		void 								pushBoolean(bool _value);
		void 								pushString(const zs_string & _value);
		void 								pop();
		virtual 							zs_string toString();
		virtual 							~ArrayScriptObject();
	protected:
		zs_vector<StackElement *> 			stk_user_elements;




	};
}
