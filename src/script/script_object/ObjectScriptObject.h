/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define ZS_NEW_OBJECT_OBJECT(zs) (ObjectScriptObject::newObjectScriptObject(zs))

namespace zetscript{

	class ZetScript;
	class  ObjectScriptObject:public ContainerScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ObjectScriptObject * newObjectScriptObject(ZetScript	*_zs);
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
		virtual StackElement * setStackElementByKeyName(
				const zs_string &  _key_name
				//,zs_string & error
				,StackElement * stk_element = NULL
		);

		template<typename _T>
		void set(const zs_string &  _key_name, _T _value);

		/*StackElement * setInteger(
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
		);*/

		/**
		 * Object getter
		 */
		virtual StackElement 			* getStackElementByKeyName(const zs_string &  _key_name);
		template<typename _T>
		_T 								get(const zs_string &  _key_name);


		bool exists(const zs_string &  _key_name);

		virtual int	length();


		bool erase(const zs_string &  _key_name);
		void eraseAll(/*const ScriptFunction *info_function=NULL*/);
		zs_vector<zs_string> getKeys();

		zs_map *getMapFields();


		virtual zs_string toString();
		virtual ~ObjectScriptObject();

	protected:

		zs_map				*	map_fields; // to search faster each property by its name
	};

}
