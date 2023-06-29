/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


namespace zetscript{

	class RefObjectScriptObject;
	struct InfoSharedPointerNode;
	class VirtualMachine;
	class ScriptTypeFactory;
	class VM_ScopeBlock;
	class ScriptFunction;

	typedef enum{
		SCRIPT_OBJECT_PROPERTY_CONSTANT=0x1<<0
	}ScriptObjectProperty;

	class ScriptObject{
	public:
		InfoSharedPointerNode 				* 	shared_pointer; // 8
		short									idx_script_type; // 2
		uint16_t 								properties; // it tells its properties

		ScriptObject(
				ZetScript 		*	_zs
				,short _idx_script_type=ZS_IDX_UNDEFINED
		);

		Symbol 								*	getGetter();
		zs_vector<StackElement *> 			*	getSetterList(MetamethodByteCode _metamethod_byte_code);

		virtual	int 							length();

		ZetScript      						* 	getZetScript();
		StackElement 						* 	getBuiltinField(const zs_string & _key_name);

		StackElement 						* 	getBuiltinField(int idx);

		virtual StackElement 				* 	setStackElement(
			const zs_string 		& 	key_name
			,StackElement 			*	stk_element = NULL
		);

		virtual StackElement 				* 	getStackElement(const zs_string & key_name);
		virtual Symbol 						*  	getScriptFunctionSymbol(const zs_string & _function_member_name);


		zs_vector<StackElement *> 			* 	getStkBuiltinListElements();
		bool 									isNativeObject();
		const char * 							getTypeName();
		ScriptType * 	    					getScriptType();
		StackElement 						*	getThis();
		virtual void						*	getNativeObject();
		virtual zs_string 						toString();

		void 									attachRefObjectNode(zs_list_node<RefObjectScriptObject *> * _ref_object_node);
		void 									deattachRefObjectNode(zs_list_node<RefObjectScriptObject *> * _ref_object_node);

		void 									printAllMemberFunctions();
		zs_map 						*			getMapBuiltinFields();

		virtual 								~ScriptObject();
	protected:

		StackElement 							stk_this;

		ZetScript 							*	zs; // 8
		VirtualMachine 						*	vm; // 8
		zs_vector<StackElement *>				stk_builtin_elements;
		zs_map    							*	map_builtin_fields; // to search faster each property by its name

		static void 							onDettachRefObjectNode(zs_list_node<RefObjectScriptObject *> *node);
		zs_list<RefObjectScriptObject *>	*	ref_objects;
		ScriptTypeFactory					*	getScriptTypeFactory();

		virtual StackElement 				* 	newBuiltinSlot();
		virtual StackElement 				* 	addBuiltinField(const zs_string & symbol_value, StackElement stk=k_stk_undefined);
		void 									unrefStackElementContainer(StackElement *si);
		void 									unrefAndFreeStackElementContainer(StackElement *si);
	};

}
