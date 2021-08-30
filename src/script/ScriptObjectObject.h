/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define ZS_NEW_OBJECT_OBJECT(zs) (ScriptObjectObject::newScriptObjectObject(zs))

namespace zetscript{

	class ZetScript;
	class  ScriptObjectObject:public ScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectObject * newScriptObjectObject(ZetScript	*_zs);
		static ScriptObjectObject * concat(ZetScript *zs,ScriptObjectObject *o1,ScriptObjectObject *o2);
		//
		// Helpers
		//
		//----------------------------------------------


		ScriptObjectObject();

		virtual StackElement * addProperty(
				const std::string & symbol_value
				,std::string & error
				,StackElement * stk_element = NULL
		);

		StackElement * addUserProperty(
						const std::string & symbol_value
						,std::string & error
						,StackElement * stk_element = NULL
				);

		virtual StackElement 	* getProperty(const std::string & property_name);

		bool existUserProperty(const std::string & property_name);

		std::map<std::string,StackElement *>::iterator begin();

		std::map<std::string,StackElement *>::iterator end();

		std::map<std::string,StackElement *>::iterator begin_builtin();

		std::map<std::string,StackElement *>::iterator end_builtin();


		bool eraseUserProperty(const std::string & symbol_value);
		void eraseAllUserProperties(/*const ScriptFunction *info_function=NULL*/);

		virtual std::string toString();
		virtual ~ScriptObjectObject();

	protected:

		std::map<std::string,StackElement *>			*	map_user_property_keys; // to search faster each property by its name

		StackElement * 			getUserProperty(const std::string & property_name);
	};

}
