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
		static ScriptObjectString * newScriptObjectStringAddStk(ZetScript *zs,StackElement *stk_result_op1,StackElement *stk_result_op2);

		void 		*	value;	// 8
		ScriptObjectString();

		virtual StackElement * 			getProperty(const std::string & property_name, int * idx=NULL);
		void set(const std::string & s);
		virtual size_t count();
		virtual std::string toString();

		ScriptObjectString *sub(ScriptObjectString *s1);

	private:
		std::string str_value;

		void setup();
	};

}
