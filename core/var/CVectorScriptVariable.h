/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_VECTOR_VAR (new CVectorScriptVariable()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassVector())))//,(void *)(new CVectorScriptVariable())))

namespace zetscript{

	class  CVectorScriptVariable: public CScriptVariable{

	public:


		int _i_size;

		vector<tStackElement> m_objVector;

		CVectorScriptVariable();

		virtual bool unrefSharedPtr();
		virtual bool initSharedPtr(bool is_assigned=false);

		void add(tStackElement  * v);
		void pop();
		tStackElement *push();
		int size();

		virtual void destroy(bool delete_user_request=false);




	};

}
