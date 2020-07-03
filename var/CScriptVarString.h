/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_STRING_VAR (new CScriptVarString(this->zs)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassString()),NULL))


namespace ZetScript{

	class  CScriptVarString: public CScriptVar{

	public:

		CScriptVarString(){}
		CScriptVarString(ZetScript *_zs);
		CScriptVarString(ZetScript *_zs,const std::string & s);

	protected:
		virtual void setup();
	
	};

}
