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
		static ScriptObjectObject * newShareableScriptObjectObject(ZetScript	*_zs);
		static ScriptObjectObject * concat(ZetScript *zs,ScriptObjectObject *o1,ScriptObjectObject *o2);
		static void  append(ZetScript *zs,ScriptObjectObject *o1,ScriptObjectObject *o2);
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

		virtual size_t	length();


		bool eraseUserProperty(const zs_string & symbol_value);
		void eraseAllUserProperties(/*const ScriptFunction *info_function=NULL*/);
		zs_map *getMapUserProperties();
		zs_map *getMapBuiltinProperties();




		virtual zs_string toString();
		virtual ~ScriptObjectObject();

	protected:

		zs_map				*	map_user_properties; // to search faster each property by its name


		StackElement * 			getUserProperty(const zs_string & property_name);
	};

}
