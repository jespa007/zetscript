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

		static ScriptObjectVector *keysSf(ScriptObjectObject *o1);
		//static ScriptObjectObjectIterator *iteratorSf(ScriptObjectObject *o1);
		static bool containsSf(ScriptObjectObject *o1, std::string * key);
		static void clearSf(ScriptObjectObject *o1);
		static void eraseSf(ScriptObjectObject *o1, std::string * key);
		//static void addSf(ScriptObjectObject *dst, ScriptObjectObject *o1);
		static ScriptObjectObject * concatSf(ZetScript *zs,ScriptObjectObject *o1,ScriptObjectObject *o2);
		//
		// Helpers
		//
		//----------------------------------------------


		ScriptObjectObject();

		virtual StackElement * addProperty(
				const std::string & symbol_value
				//, const ScriptFunction *info_function
				//, Instruction *src_instruction
				,std::string & error
				,StackElement * stk_element = NULL
				/*,int * idx_stk_element = NULL*/
		);

		virtual StackElement 	* getProperty(const std::string & property_name, int * idx=NULL);

		zs_map_iterator begin();


		bool eraseUserProperty(const std::string & symbol_value);
		void eraseAllUserProperties(/*const ScriptFunction *info_function=NULL*/);


		virtual ~ScriptObjectObject();

	protected:

		zs_map				*	map_user_property_keys; // to search faster each property by its name

		StackElement * addUserProperty(
				const std::string & symbol_value
				//, const ScriptFunction *info_function
				//, Instruction *src_instruction
				,std::string & error
				,StackElement * stk_element = NULL
			//	,int * idx_stk_element = NULL
		);

		StackElement * 			getUserProperty(const std::string & property_name/*, int * idx=NULL*/);
	};

}
