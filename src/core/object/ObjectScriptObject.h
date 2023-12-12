/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define ZS_NEW_OBJECT_SCRIPT_OBJECT(zs) (ObjectScriptObject::newObjectScriptObject(zs))

namespace zetscript{

	class ScriptEngine;
	class  ObjectScriptObject:public ContainerScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ObjectScriptObject * newObjectScriptObject(ScriptEngine	*_script_engine);
		static ObjectScriptObject * concat(ScriptEngine *zs,ObjectScriptObject *o1,ObjectScriptObject *o2);
		static void  append(ScriptEngine *zs,ObjectScriptObject *o1,ObjectScriptObject *o2);
		//
		// Helpers
		//
		//----------------------------------------------


		ObjectScriptObject(ScriptEngine	*_script_engine);

		/**
		 * setters to set field value
		 */
		virtual StackElement * setStackElementByKeyName(
				const String &  _key_name
				//,String & error
				,StackElement * stk_element = NULL
		);

		template<typename _T>
		void set(const String &  _key_name, _T _value);


		template<typename _T>
		bool 								elementInstanceOf(const String & _key);


		/*StackElement * setInteger(
				const String &  _property_name
				,zs_int _value
		);

		StackElement * setFloat(
				const String &  _key_name
				,zs_float _value
		);

		StackElement * setBoolean(
				const String &  _key_name
				,bool _value
		);

		StackElement * setString(
				const String &  _key_name
				,const String & _value
		);*/

		/**
		 * ScriptObject getter
		 */
		virtual StackElement 			* getStackElementByKeyName(const String &  _key_name);
		template<typename _T>
		_T 								get(const String &  _key_name);


		bool exists(const String &  _key_name);

		virtual int	length();


		bool erase(const String &  _key_name);
		void eraseAll(/*const ScriptFunction *info_function=NULL*/);
		Vector<String> getKeys();

		MapString *getMapStringFields();


		virtual String toString();
		virtual ~ObjectScriptObject();

	protected:

		MapString				*	map_fields; // to search faster each property by its name
	};

}
