/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_VECTOR_VAR (new CVectorScriptVariable(this->zs)) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassVector())))//,(void *)(new CVectorScriptVariable())))

namespace zetscript{

	class  CVectorScriptVariable: public CScriptVariable{


	public:

		StackElement return_callc;
		//int _i_size;

		//std::vector<StackElement> m_objVector;

		CVectorScriptVariable(){}
		CVectorScriptVariable(CZetScript *_zs);

		virtual bool unrefSharedPtr();


		//StackElement 		  * getValue(const std::string & s);
		//virtual StackElement * getValue(int idx);


		StackElement * push();
		void 			push(StackElement  * v);
		StackElement * pop();

		 int size();

		virtual void destroy();

		virtual ~CVectorScriptVariable();


	};

}
