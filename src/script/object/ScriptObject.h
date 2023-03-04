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
	class MemberFunctionScriptObject;
	class ScriptTypeFactory;
	class RefObjectScriptObject;
	struct VM_ScopeBlock;

	typedef struct _SharedPointerInfo {
		ScriptObject 			*ptr_script_object_shared;
		unsigned short 			n_shares;
		//int						created_idx_call;
		VM_ScopeBlock			*vm_scope_block_where_created;
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
		InfoSharedPointerNode 				* 	shared_pointer; // 8
		short									idx_script_type; // 2

		ScriptObject(
				ZetScript 		*	_zs
				,short _idx_script_type=ZS_IDX_UNDEFINED
		);

		Symbol 								*	getGetter();
		zs_vector<StackElement *> 			*	getSetterList(ByteCodeMetamethod _byte_code_metamethod);

		virtual	int 							length();

		ZetScript      						* 	getZetScript();
		StackElement 						* 	getBuiltinField(const zs_string & _key_name);

		StackElement 						* 	getBuiltinField(int idx);

		virtual StackElement 				* 	set(
			const zs_string 		& 	key_name
			,StackElement 			*	stk_element = NULL
		);

		virtual StackElement 				* 	get(const zs_string & key_name);
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
		bool 									unrefAndFreeStackElementContainer(StackElement *si);
	};

}
