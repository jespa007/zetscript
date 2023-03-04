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

		/**
		 * setters to set field value
		 */
		virtual StackElement * set(
				const zs_string &  _key_name
				//,zs_string & error
				,StackElement * stk_element = NULL
		);

		StackElement * setInteger(
				const zs_string &  _property_name
				,zs_int _value
		);

		StackElement * setFloat(
				const zs_string &  _key_name
				,zs_float _value
		);

		StackElement * setBoolean(
				const zs_string &  _key_name
				,bool _value
		);

		StackElement * setString(
				const zs_string &  _key_name
				,const zs_string & _value
		);

		/**
		 * Object getter
		 */
		virtual StackElement 	* get(const zs_string &  _key_name);

		bool exists(const zs_string &  _key_name);

		virtual int	length();


		bool erase(const zs_string &  _key_name);
		void eraseAll(/*const ScriptFunction *info_function=NULL*/);
		zs_map getKeys();
		//zs_map *getMapUserFields();
		//zs_map *getMapBuiltinFields();




		virtual zs_string toString();
		virtual ~ObjectScriptObject();

	protected:

		zs_map				*	map_user_fields; // to search faster each property by its name


		StackElement * 			getUserField(const zs_string &  _key);
	};

}
