/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	template<typename _N>
	typedef struct _zs_list_node{
		_N data;
		_zs_list_node *previous, *next;

		_zs_list_node(){
			previous=next=NULL;
		}
	} zs_list_node;


	template<typename _N>
	class zs_list{
	public:
		/*typedef struct _SharedPointerInfo {
			ScriptObject 			*ptr_script_object_shared;
			unsigned short 			n_shares;
			//int						created_idx_call;
			VM_ScopeBlock			*created_scope_block;
		} SharedPointerInfo;*/


		zs_list_node<_N> *first, *last;

		zs_list();

		bool insert(const _N & _data);
		bool remove(zs_list_node<_N> * _node);

		~zs_list();
	};
}

#include "zs_list.tcc"
