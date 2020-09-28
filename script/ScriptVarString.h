/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_STRING_VAR (new ScriptVarString(this->zs)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassString()),NULL))


namespace zetscript{

	class  ScriptVarString: public ScriptVar{

	public:
		std::string str_value;

		ScriptVarString(){}
		ScriptVarString(ZetScript *_zs);
		ScriptVarString(ZetScript *_zs,const std::string & s);

		virtual std::string toString();

	protected:
		virtual void setup();
	
	};

}
