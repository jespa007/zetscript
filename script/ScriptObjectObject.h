/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define ZS_NEW_OBJECT_OBJECT(zs) (ScriptObjectObject::newScriptObjectObject(zs))

namespace zetscript{

	class ZetScript;
	class  ScriptObjectObject:public ScriptObjectVector{

	public:

		//----------------------
		// public vars ...

		// Construct ...
		static ScriptObjectObject * newScriptObjectObject(ZetScript	*_zs);
		static ScriptObjectObject * newScriptObjectObjectAdd(ZetScript *zs,ScriptObjectObject *o1,ScriptObjectObject *o2);

		ScriptObjectObject();

		virtual StackElement * addProperty(
				const std::string & symbol_value
				//, const ScriptFunction *info_function
				//, Instruction *src_instruction
				,std::string & error
				,StackElement * stk_element = NULL
				,int * idx_stk_element = NULL
		);

		virtual StackElement 	* getProperty(const std::string & property_name, int * idx=NULL);

		zs_map_iterator begin();


		bool eraseProperty(const std::string & symbol_value, const ScriptFunction *info_function=NULL);


		virtual ~ScriptObjectObject();

	protected:

		zs_map				*	map_user_property_keys; // to search faster each property by its name

		StackElement * addUserProperty(
				const std::string & symbol_value
				//, const ScriptFunction *info_function
				//, Instruction *src_instruction
				,std::string & error
				,StackElement * stk_element = NULL
				,int * idx_stk_element = NULL
		);

		StackElement * 			getUserProperty(const std::string & property_name, int * idx=NULL);
	};

}
