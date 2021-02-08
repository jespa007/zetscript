/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_STRING_VAR (ScriptObjectString::newStringObject(this->zs)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassString()),NULL))


namespace zetscript{

	class  ScriptObjectString: public ScriptObject{

	public:

		static ScriptObjectString * newStringObject(ZetScript *zs, const std::string & str="");

		void 		*	value;	// 8
		ScriptObjectString();


		void set(const std::string & s);
		virtual zs_int count();

		virtual std::string toString();

	private:
		std::string str_value;

		void setup();
	};

}
