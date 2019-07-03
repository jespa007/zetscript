/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_VECTOR_VAR (new CVectorScriptVariable()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassVector())))//,(void *)(new CVectorScriptVariable())))

namespace zs{

	class  CVectorScriptVariable: public CScriptVariable{


	public:

		tStackElement return_callc;
		//int _i_size;

		//std::vector<tStackElement> m_objVector;

		CVectorScriptVariable();

		virtual bool unrefSharedPtr();


		//tStackElement 		  * getValue(const std::string & s);
		//virtual tStackElement * getValue(int idx);


		tStackElement * push();
		void 			push(tStackElement  * v);
		tStackElement * pop();

		 int size();

		virtual void destroy();

		virtual ~CVectorScriptVariable();


	};

}
