/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_VECTOR_OBJECT(zs) (VectorScriptObject::newVectorScriptObject(zs)) //ScriptType::getInstance()->getRegisteredClassByIdx(ScriptType::getInstance()->getIdxClassVector())))//,(void *)(new VectorScriptObject())))

namespace zetscript{

	class CZetgine;
	class  VectorScriptObject: public ContainerScriptObject{
	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static VectorScriptObject * newVectorScriptObject(ZetScript *zs);
		static VectorScriptObject * newVectorScriptObjectAdd(ZetScript *zs,VectorScriptObject *v1,VectorScriptObject *v2);



		//
		// Helpers
		//
		//----------------------------------------------

		VectorScriptObject(ZetScript *_zs);

		virtual StackElement * 	getUserElementAt(int idx);
		bool 				eraseUserElementAt(int idx);
		void				eraseAllUserElements();

		virtual int length();
		virtual zs_vector<StackElement *> * getStkUserListElements();

		bool exists(StackElement  * stk);

		StackElement *			pushNewUserSlot();
		void 					push(StackElement  * stk);
		void 					pop();
		virtual zs_string toString();
		virtual ~VectorScriptObject();
	protected:
		zs_vector<StackElement *> 			stk_user_elements;




	};
}
