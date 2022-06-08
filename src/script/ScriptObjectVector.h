/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_VECTOR(zs) (ScriptObjectVector::newScriptObjectVector(zs)) //ScriptType::getInstance()->getRegisteredClassByIdx(ScriptType::getInstance()->getIdxClassVector())))//,(void *)(new ScriptObjectVector())))

namespace zetscript{

	class CZetgine;
	class  ScriptObjectVector: public ScriptObject{
	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectVector * newScriptObjectVector(ZetScript *zs);
		static ScriptObjectVector * newShareableScriptObjectVector(ZetScript *zs);
		static ScriptObjectVector * newScriptObjectVectorAdd(ZetScript *zs,ScriptObjectVector *v1,ScriptObjectVector *v2);

		static zs_vector 	toVector(ScriptObjectVector *sv);

		//
		// Helpers
		//
		//----------------------------------------------

		ScriptObjectVector();

		virtual StackElement * 	getUserElementAt(int idx);
		bool 				eraseUserElementAt(int idx);
		void				eraseAllUserElements();

		virtual size_t length();
		virtual zs_vector * getStkUserListElements();

		bool exists(StackElement  * stk);

		StackElement *			pushNewUserSlot();
		void 					push(StackElement  * stk);
		void 					pop();
		virtual zs_string toString();
		virtual ~ScriptObjectVector();
	protected:
		zs_vector 			stk_user_elements;




	};
}
