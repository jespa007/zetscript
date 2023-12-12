/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


namespace zetscript{

	class RefScriptObject;
	struct InfoSharedPointerNode;
	struct VirtualMachine;
	class ScriptTypesFactory;
	struct VM_ScopeBlock;
	class ScriptFunction;

	typedef enum{
		OBJECT_PROPERTY_CONSTANT=0x1<<0
	}ObjectProperty;

	class ScriptObject{
	public:
		InfoSharedPointerNode 				* 	shared_pointer; // 8
		ScriptTypeId							script_type_id; // 2
		uint16_t 								properties; // it tells its properties

		ScriptObject(
				ScriptEngine 		*	_script_engine
				,ScriptTypeId _script_type_id=SCRIPT_TYPE_ID_INVALID
		);

		Symbol 								*	getGetter();
		Vector<StackElement *> 			*	getSetterList(Metamethod _metamethod);

		virtual	int 							length();

		ScriptEngine      						* 	getScriptEngine();
		StackElement 						* 	getBuiltinField(const String & _key_name);

		StackElement 						* 	getBuiltinField(int idx);

		virtual StackElement 				* 	setStackElementByKeyName(
			const String 		& 	key_name
			,StackElement 			*	stk_element = NULL
		);

		virtual StackElement 				* 	getStackElementByKeyName(const String & key_name);
		virtual Symbol 						*  	getFunctionSymbol(const String & _function_member_name);


		Vector<StackElement *> 			* 	getStkBuiltinListElements();
		bool 									isNativeObject();
		const char * 							getScriptTypeName();
		ScriptType * 	    					getScriptType();
		StackElement 						*	getThis();
		virtual void						*	getNativeObject();
		virtual String 						toString();

		void 									attachRefObjectNode(ListNode<RefScriptObject *> * _ref_object_node);
		void 									deattachRefObjectNode(ListNode<RefScriptObject *> * _ref_object_node);

		void 									printAllMemberFunctions();
		MapString 						*		getMapStringBuiltinFields();

		virtual 								~ScriptObject();
	protected:

		StackElement 							stk_this;

		ScriptEngine 						*	script_engine; // 8
		VirtualMachine 						*	vm; // 8
		Vector<StackElement *>					stk_builtin_elements;
		MapString    						*	map_builtin_fields; // to search faster each property by its name

		static void 							onDettachRefObjectNode(ListNode<RefScriptObject *> *node);
		List<RefScriptObject *>	*	ref_objects;
		ScriptTypesFactory					*	getScriptTypesFactory();

		virtual StackElement 				* 	newBuiltinSlot();
		virtual StackElement 				* 	addBuiltinField(const String & symbol_value, StackElement stk=k_stk_undefined);
		void 									unrefStackElementContainer(StackElement *si);
		void 									unrefAndFreeStackElementContainer(StackElement *si);
	};

}
