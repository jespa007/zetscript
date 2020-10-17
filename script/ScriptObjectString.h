/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_STRING_VAR (new ScriptObjectString(this->zs)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassString()),NULL))


namespace zetscript{

	class  ScriptObjectString: public ScriptObject{

	public:
		std::string str_value;

		ScriptObjectString(){}
		ScriptObjectString(ZetScript *_zs);
		ScriptObjectString(ZetScript *_zs,const std::string & s);

		virtual std::string toString();

	protected:
		virtual void setup();
	
	};

}
