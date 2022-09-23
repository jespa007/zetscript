/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define ZS_NEW_OBJECT_OBJECT(zs) (ObjectScriptObject::newScriptObjectObject(zs))

namespace zetscript{

	class ZetScript;
	class  ObjectScriptObject:public ContainerScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ObjectScriptObject * newScriptObjectObject(ZetScript	*_zs);
		static ObjectScriptObject * concat(ZetScript *zs,ObjectScriptObject *o1,ObjectScriptObject *o2);
		static void  append(ZetScript *zs,ObjectScriptObject *o1,ObjectScriptObject *o2);
		//
		// Helpers
		//
		//----------------------------------------------


		ObjectScriptObject(ZetScript	*_zs);

		virtual StackElement * addProperty(
				const zs_string &  symbol_value
				,zs_string & error
				,StackElement * stk_element = NULL
		);

		StackElement * addUserProperty(
				const zs_string &  symbol_value
						,zs_string & error
						,StackElement * stk_element = NULL
				);

		virtual StackElement 	* getProperty(const zs_string &  property_name);

		bool existUserProperty(const zs_string &  property_name);

		virtual int	length();


		bool eraseUserProperty(const zs_string &  symbol_value);
		void eraseAllUserProperties(/*const ScriptFunction *info_function=NULL*/);
		zs_map *getMapUserProperties();
		zs_map *getMapBuiltinProperties();




		virtual zs_string toString();
		virtual ~ObjectScriptObject();

	protected:

		zs_map				*	map_user_properties; // to search faster each property by its name


		StackElement * 			getUserProperty(const zs_string &  property_name);
	};

}
