/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define TYPE_SCRIPT_VARIABLE 						"__ScriptObject__"

namespace zetscript{

	struct _InfoSharedList;
	struct VirtualMachine;
	class ScriptObject;
	class ScriptFunction;
	class ScriptObjectMemberFunction;
	class ScriptTypeFactory;
	class ScriptObjectContainerSlotAssignment;
	class RefObject;
	struct VM_ScopeBlock;

	typedef struct _SharedPointerInfo {
		ScriptObject 			*ptr_script_object_shared;
		unsigned short 			n_shares;
		//int						created_idx_call;
		VM_ScopeBlock			*created_scope_block;
	} SharedPointerInfo;

	typedef struct _InfoSharedPointerNode{
		SharedPointerInfo data;
		_InfoSharedPointerNode *previous, *next;
	} InfoSharedPointerNode;

	typedef struct _InfoSharedList{
		InfoSharedPointerNode *first, *last;
	}InfoSharedList;


	class MemberProperty;
	struct StackMemberProperty{
	public:
		MemberProperty 		*member_property; // make function pointer first to make compatible with stk
		ScriptObject		*so_object;

		StackMemberProperty(
				ScriptObject		*_so_object
				,MemberProperty 	*_member_property){
			so_object = _so_object;
			member_property= _member_property;
		}
	};

	class ScriptObject{
	public:
		InfoSharedPointerNode 	* 	shared_pointer; // 8
		short						idx_script_type; // 2

		ScriptObject();

		Symbol 						*	getGetter();
		zs_vector<StackElement *> 	*	getSetterList(ByteCodeMetamethod _byte_code_metamethod);

		virtual	int 					length();

		ZetScript      				* 	getZetScript();
		StackElement 				* 	getBuiltinProperty(const zs_string & property_name);

		StackElement 			* 	getBuiltinElementAt(short idx);

		virtual StackElement 	* 	addProperty(
			const zs_string 	& 	symbol_value
			,zs_string 		& 	error
			,StackElement 		*	stk_element = NULL
		);

		virtual StackElement 	* 	getProperty(const zs_string & property_name);
		virtual Symbol 			*  	getScriptFunctionSymbol(const zs_string & _function_member_name);


		zs_vector<StackElement *> 				* 	getStkBuiltinListElements();
		bool 						isNativeObject();
		const char * 				getTypeName();
		ScriptType * 	    		getScriptType();
		StackElement 			*	getThisProperty();
		virtual void			*	getNativeObject();
		virtual zs_string 			toString();

		void 						removeRefObject(RefObject * _ref_object);
		void 						addRefObject(RefObject * _ref_object);

		void 						addContainerSlotAssignment(ScriptObjectContainerSlotAssignment *_wp);
		void 						removeContainerSlotAssignment(ScriptObjectContainerSlotAssignment * _wp);
		//bool 						deRefContainerSlotAssignment();

		void 						printAllMemberFunctions();

		virtual 					~ScriptObject();
	protected:

		StackElement 								stk_this;

		ZetScript 								*	zs; // 8
		VirtualMachine 							*	vm; // 8
		zs_vector<StackElement *>					stk_builtin_elements;
		zs_map    								*	map_builtin_properties; // to search faster each property by its name

		// TODO: replace zs_vector by zs_map_int to search ref_objects/container_slot_assignments quickly
		zs_vector<RefObject *>			    	*   ref_script_objects;
		zs_vector<ScriptObjectContainerSlotAssignment *>	*   container_slot_assignments;

		void 										init(ZetScript *zs);

		ScriptTypeFactory						*	getScriptTypeFactory();

		virtual StackElement 					* 	newBuiltinSlot();
		virtual StackElement 					* 	addBuiltinProperty(const zs_string & symbol_value, StackElement stk=k_stk_undefined);
		bool 										unrefAndFreeStackElementContainer(StackElement *si);
		int 										idxContainerSlotAssignment(ScriptObjectContainerSlotAssignment *_wp);
		int 										idxRefObject(RefObject  *_ref_object);
	};

}
