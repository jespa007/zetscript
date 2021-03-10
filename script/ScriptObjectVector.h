/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_VECTOR(zs) (ScriptObjectVector::newScriptObjectVector(zs)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassVector())))//,(void *)(new ScriptObjectVector())))

namespace zetscript{

	class CZetgine;
	class  ScriptObjectVector: public ScriptObject{
	public:
		StackElement return_callc;

		template<typename T>
		static std::vector<T> 	to_std_vector(ScriptObjectVector *v_in);
		static void    			push(ScriptObjectVector *sv,StackElement  * stk);
		static StackElement *  	pop(ScriptObjectVector *sv);

		static ScriptObjectVector * newScriptObjectVector(ZetScript *zs);
		static ScriptObjectVector * newScriptObjectVectorAdd(ZetScript *zs,ScriptObjectVector *v1,ScriptObjectVector *v2);

		ScriptObjectVector();

		virtual StackElement * 	getUserElementAt(short idx);

		virtual size_t length();
		virtual zs_vector * getAllUserElements();

		StackElement *			newUserSlot();
		void 					push(StackElement  * stk);
		StackElement *			pop();

		virtual ~ScriptObjectVector();
	protected:
		zs_vector 			stk_user_elements;


		bool 				eraseUserElementAt(short idx);

	};
}

#include "ScriptObjectVector.tcc"
