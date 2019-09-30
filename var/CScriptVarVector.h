/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_VECTOR_VAR (new CScriptVarVector(this->zs)) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassVector())))//,(void *)(new CScriptVarVector())))

namespace zetscript{

	class  CScriptVarVector: public CScriptVar{


	public:

		StackElement return_callc;
		//int _i_size;

		//std::vector<StackElement> m_objVector;

		CScriptVarVector(){}
		CScriptVarVector(CZetScript *_zs);

		virtual bool unrefSharedPtr();


		//StackElement 		  * getValue(const std::string & s);
		//virtual StackElement * getValue(int idx);


		StackElement * push();
		void 			push(StackElement  * v);
		StackElement * pop();

		 int size();

		virtual void destroy();

		virtual ~CScriptVarVector();


	};

}
