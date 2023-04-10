/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	struct SharedPointerInfo {
		ScriptObject 			*ptr_script_object_shared;
		unsigned short 			n_shares;
		//int						created_idx_call;
		VM_ScopeBlock			*vm_scope_block_where_created;
	};

	struct InfoSharedPointerNode{
		SharedPointerInfo data;
		InfoSharedPointerNode *previous, *next;
	};

	struct InfoSharedList{
		InfoSharedPointerNode *first, *last;
	};

	bool 			vm_share_script_object(VirtualMachine *vm,ScriptObject *_obj);
	bool 			vm_create_shared_script_object(
			VirtualMachine *vm
			, ScriptObject *_obj
			,VM_ScopeBlock *_vm_scope_block=NULL
	);

	bool vm_unref_shared_script_object(
		VirtualMachine 	*	_vm
		, ScriptObject 	*	_obj
		,VM_ScopeBlock 	*	_scope_block
	);

	bool vm_deattach_shared_node(
		VirtualMachine *vm
		, InfoSharedList * list
		, InfoSharedPointerNode *_node
	);

	void vm_remove_empty_shared_pointers(
		VirtualMachine *vm
		,VM_ScopeBlock *scope_block
	);


}
