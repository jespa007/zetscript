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
				const char * symbol_value
				,char * error
				,StackElement * stk_element = NULL
		);

		StackElement * addUserProperty(
						const char * symbol_value
						,char * error
						,StackElement * stk_element = NULL
				);

		virtual StackElement 	* getProperty(const char * property_name);

		bool existUserProperty(const char  * property_name);

		virtual int	length();


		bool eraseUserProperty(const char * symbol_value);
		void eraseAllUserProperties(/*const ScriptFunction *info_function=NULL*/);
		zs_map *getMapUserProperties();
		zs_map *getMapBuiltinProperties();




		virtual zs_string toString();
		virtual ~ScriptObjectObject();

	protected:

		zs_map				*	map_user_properties; // to search faster each property by its name


		StackElement * 			getUserProperty(const char  * property_name);
	};

}
