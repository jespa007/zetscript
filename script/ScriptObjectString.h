/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_STRING_VAR (new ScriptObjectString(this->zs)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassString()),NULL))


namespace zetscript{

	class  ScriptObjectString: public ScriptObject{

	public:
		void 		*	value;	// 8
		ScriptObjectString(ZetScript *_zs);

		void set(const std::string & s);
		virtual zs_int count();

		virtual std::string toString();

	private:
		std::string str_value;
	};

}
