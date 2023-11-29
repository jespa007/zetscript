/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


namespace zetscript{

	class RefObjectObject;
	struct InfoSharedPointerNode;
	struct VirtualMachine;
	class TypesFactory;
	struct VM_ScopeBlock;
	class Function;

	typedef enum{
		OBJECT_PROPERTY_CONSTANT=0x1<<0
	}ObjectProperty;

	class Object{
	public:
		InfoSharedPointerNode 				* 	shared_pointer; // 8
		TypeId							type_id; // 2
		uint16_t 								properties; // it tells its properties

		Object(
				ZetScript 		*	_zs
				,TypeId _type_id=TYPE_ID_INVALID
		);

		Symbol 								*	getGetter();
		Vector<StackElement *> 			*	getSetterList(Metamethod _metamethod);

		virtual	int 							length();

		ZetScript      						* 	getZetScript();
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
		const char * 							getTypeName();
		Type * 	    					getType();
		StackElement 						*	getThis();
		virtual void						*	getNativeObject();
		virtual String 						toString();

		void 									attachRefObjectNode(ListNode<RefObjectObject *> * _ref_object_node);
		void 									deattachRefObjectNode(ListNode<RefObjectObject *> * _ref_object_node);

		void 									printAllMemberFunctions();
		MapString 						*			getMapStringBuiltinFields();

		virtual 								~Object();
	protected:

		StackElement 							stk_this;

		ZetScript 							*	zs; // 8
		VirtualMachine 						*	vm; // 8
		Vector<StackElement *>				stk_builtin_elements;
		MapString    							*	map_builtin_fields; // to search faster each property by its name

		static void 							onDettachRefObjectNode(ListNode<RefObjectObject *> *node);
		List<RefObjectObject *>	*	ref_objects;
		TypesFactory					*	getTypeFactory();

		virtual StackElement 				* 	newBuiltinSlot();
		virtual StackElement 				* 	addBuiltinField(const String & symbol_value, StackElement stk=k_stk_undefined);
		void 									unrefStackElementContainer(StackElement *si);
		void 									unrefAndFreeStackElementContainer(StackElement *si);
	};

}
