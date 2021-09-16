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
				const zs_string & symbol_value
				,zs_string & error
				,StackElement * stk_element = NULL
		);

		StackElement * addUserProperty(
						const zs_string & symbol_value
						,zs_string & error
						,StackElement * stk_element = NULL
				);

		virtual StackElement 	* getProperty(const zs_string & property_name);

		bool existUserProperty(const zs_string & property_name);

		zs_map_iterator begin();
		zs_map_iterator begin_builtin();


		bool eraseUserProperty(const zs_string & symbol_value);
		void eraseAllUserProperties(/*const ScriptFunction *info_function=NULL*/);

		virtual zs_string toString();
		virtual ~ScriptObjectObject();

	protected:

		zs_map				*	map_user_property_keys; // to search faster each property by its name

		StackElement * 			getUserProperty(const zs_string & property_name);
	};

}
