/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define TYPE_SCRIPT_VARIABLE 						"__ScriptObject__"

namespace zetscript{

	class ScriptObject{
	public:
		InfoSharedPointerNode 	* 	shared_pointer; // 8
		short						idx_script_class; // 2

		ScriptObject();
		void setZetScript(ZetScript	*_zs);


		virtual				size_t length();

		//bool 				initSharedPtr();
		//bool 				unrefSharedPtr(int _idx_current_call);
		ZetScript      * 	getZetScript();
		virtual StackElement 	* getProperty(const std::string & property_name, int * idx=NULL);
		virtual StackElement * getElementAt(short idx);
		virtual StackElement * addProperty(
				const std::string & symbol_value
				//, const ScriptFunction *info_function
				//, Instruction *src_instruction
				,std::string & error
				,StackElement * stk_element = NULL
				,int * idx_stk_element = NULL

		);
		virtual zs_vector * getAllElements();
		ScriptClass * 	    getNativeScriptClass();
		bool 				isNativeObject();
		const std::string & getClassName();
		ScriptClass * 	    getScriptClass();
		StackElement 	*getThisProperty();
		virtual void	*getNativeObject();
		virtual std::string toString();

		virtual ~ScriptObject();
	protected:

		StackElement stk_this;

		ZetScript 				*	zs; // 8
		zs_vector				stk_elements_builtin;
		zs_map					*map_property_keys_builtin; // to search faster each property by its name

		StackElement 			stk_length;
		StackElement 			* newSlotBuiltin();
		StackElement * addPropertyBuiltIn(const std::string & symbol_value, StackElement stk=stk_undefined);

	};

}
