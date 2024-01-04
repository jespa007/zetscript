/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	template<typename _N>
	struct ListNode{
		_N data;
		ListNode *previous, *next;

		ListNode(){
			previous=next=NULL;
		}
	};

	template<typename _N>
	class List{
	public:

		ListNode<_N> *first, *last;

		List();

		void insert(ListNode<_N> * _node);
		void remove(ListNode<_N> * _node);
		void dettachAllNodes(void (*_dettachNode)(ListNode<_N> *)=NULL);

		~List();

	};
}

#include "List.tcc"
