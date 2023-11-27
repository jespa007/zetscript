/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	struct SharedPointerInfo {
		Object 			*ptr_object_shared;
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

	void 			vm_share_object(VirtualMachine *vm,Object *_obj);

	void  vm_insert_shared_node(
		VirtualMachine	 		*	_vm
		, InfoSharedList 		*	_list
		, InfoSharedPointerNode *	_node
	);

	void 			vm_create_shared_object(
			VirtualMachine *vm
			, Object *_obj
			,VM_ScopeBlock *_vm_scope_block=NULL
	);

	void vm_unref_shared_object(
		VirtualMachine 	*	_vm
		, Object 	*	_obj
		,VM_ScopeBlock 	*	_scope_block
	);

	void vm_deattach_shared_node(
		VirtualMachine *vm
		, InfoSharedList * list
		, InfoSharedPointerNode *_node
	);

	void vm_remove_empty_shared_pointers(
		VirtualMachine *vm
		,VM_ScopeBlock *scope_block
	);


}
