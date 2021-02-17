/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_STRING(zs) (ScriptObjectString::newScriptObjectString(zs)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassString()),NULL))


namespace zetscript{

	class  ScriptObjectString: public ScriptObject{

	public:

		static ScriptObjectString * newScriptObjectString(ZetScript *zs, const std::string & str="");

		void 		*	value;	// 8
		ScriptObjectString();


		void set(const std::string & s);
		virtual size_t count();

		virtual std::string toString();

	private:
		std::string str_value;

		void setup();
	};

}
