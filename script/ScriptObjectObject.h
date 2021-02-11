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

		ScriptObjectObject();

		StackElement * addProperty(
				const std::string & symbol_value
				//, const ScriptFunction *info_function
				//, Instruction *src_instruction
				,std::string & error
				,StackElement * stk_element = NULL
				,int * idx_stk_element = NULL

		);

		int			   			getPropertyIdx(const std::string & varname);
		StackElement * 			getProperty(const std::string & property_name, int * idx=NULL);

		StackElement *getThisProperty();


		bool eraseProperty(const std::string & symbol_value, const ScriptFunction *info_function=NULL);


		virtual ~ScriptObjectObject();

	protected:

		StackElement stk_this;
		zs_map				*	map_property_keys; // to search faster each property by its name

	};

}
