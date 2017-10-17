/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_STRING_VAR (new CStringScriptVariable()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassString()),NULL))


namespace zetscript{

	class  CStringScriptVariable: public CScriptVariable{

	public:

		CStringScriptVariable();
		CStringScriptVariable(const string & s);

	protected:
		virtual void setup();
	
	};

}
