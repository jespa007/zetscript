/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_STRING(zs) (ScriptObjectString::newScriptObjectString(zs)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassString()),NULL))


namespace zetscript{
	class ScriptObjectVector;
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
		static bool existSf(ScriptObjectString *so, std::string *str);
		static bool existSf(ScriptObjectString *so, zs_int ch);
		static void clearSf(ScriptObjectString *so);
		static void removeAtSf(ScriptObjectString *so, zs_int idx);
		static void insertAtSf(ScriptObjectString *so, zs_int idx,zs_int ch);
		static ScriptObjecVector * splitSf(ScriptObjectString *so);
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
		std::string str_value;

		void setup();
	};

}
