/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


namespace zetscript{

class ScriptClass;
	class ScriptClass;
	class ZetScript;
	class VirtualMachine;
	class ScriptClassFactory;
	class  ScriptObjectAnonymous:public ScriptObjectVector{

	public:

		//----------------------
		// public vars ...

		// Construct ...

		ScriptObjectAnonymous(ZetScript	*_zs);



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


		virtual ~ScriptObjectAnonymous();

	protected:

		StackElement stk_this;
		zs_map				*	map_property_keys; // to search faster each property by its name

	};

}
