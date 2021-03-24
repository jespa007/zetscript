/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_STRING(zs) (ScriptObjectString::newScriptObjectString(zs)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassString()),NULL))


namespace zetscript{

	class  ScriptObjectString: public ScriptObject{

	public:
		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectString * newScriptObjectString(ZetScript *zs, const std::string & str="");
		static ScriptObjectString * newScriptObjectStringAddStk(ZetScript *zs,StackElement *stk_result_op1,StackElement *stk_result_op2);
		static ScriptObjectString * formatSf(ZetScript *zs,StackElement *str, StackElement *args);


		static zs_int sizeSf(ScriptObjectString *so);
		static bool containsSf(ScriptObjectString *so, std::string *str);
		static bool containsSf(ScriptObjectString *so, zs_int ch);
		static bool existSf(ScriptObjectString *so, zs_int ch);
		static void clearSf(ScriptObjectString *so);
		static ScriptObjectString *replaceSf(ScriptObjectString *str_in,std::string *str_old, std::string *str_new);
		static void eraseAtSf(ScriptObjectString *so, zs_int idx);
		static void insertAtSf(ScriptObjectString *so, zs_int idx,zs_int ch);
		static ScriptObjectVector * splitSf(ScriptObjectString *so,zs_int ch_delim);
		static ScriptObjectVector * splitSf(ScriptObjectString *so,std::string * str_token);
		//
		// Helpers
		//
		//----------------------------------------------


		void 		*	value;	// 8
		ScriptObjectString();

		void set(const std::string & s);
		virtual size_t length();
		virtual std::string toString();

		ScriptObjectString *sub(ScriptObjectString *s1);
	protected:


	private:
		std::string default_str_value;

		void setup();
	};

}
