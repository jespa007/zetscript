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



		//
		// Helpers
		//
		//----------------------------------------------

		ScriptObjectVector();

		virtual StackElement * 	getUserElementAt(size_t idx);
		bool 				eraseUserElementAt(size_t idx);
		void				eraseAllUserElements();

		virtual size_t length();
		virtual std::vector<StackElement *> * getStkUserListElements();

		bool exists(StackElement  * stk);

		StackElement *			pushNewUserSlot();
		void 					push(StackElement  * stk);
		void 					pop();
		virtual std::string toString();
		virtual ~ScriptObjectVector();
	protected:
		std::vector<StackElement *> 			stk_user_elements;




	};
}
