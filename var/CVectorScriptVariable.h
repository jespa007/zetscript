/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_VECTOR_VAR (new CVectorScriptVariable()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassVector())))//,(void *)(new CVectorScriptVariable())))

namespace zetscript{

	class  CVectorScriptVariable: public CScriptVariable{

		typedef struct {
			string key;
			int		idx;
		}tNamedElementVector;

		vector<tNamedElementVector> vecNamedElement;
	public:

		tStackElement return_callc;
		int _i_size;

		vector<tStackElement> m_objVector;

		CVectorScriptVariable();

		virtual bool unrefSharedPtr();

		tStackElement * get(const string & s);


		tStackElement * push();
		void 			push(tStackElement  * v);
		tStackElement * pop();

		ZETSCRIPT_MODULE_EXPORT int size();

		virtual void destroy();

		virtual ~CVectorScriptVariable();


	};

}
