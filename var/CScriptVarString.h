/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_STRING_VAR (new CScriptVarString(this->zs)) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassString()),NULL))


namespace zetscript{

	class  CScriptVarString: public CScriptVar{

	public:

		CScriptVarString(){}
		CScriptVarString(CZetScript *_zs);
		CScriptVarString(CZetScript *_zs,const std::string & s);

	protected:
		virtual void setup();
	
	};

}
