/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	template<typename _N>
	struct zs_list_node{
		_N data;
		zs_list_node *previous, *next;

		zs_list_node(){
			previous=next=NULL;
		}
	};


	template<typename _N>
	class zs_list{
	public:

		zs_list_node<_N> *first, *last;

		zs_list();

		bool insert(zs_list_node<_N> * _node);
		bool remove(zs_list_node<_N> * _node);
		void remove_all();

		~zs_list();

	};
}

#include "zs_list.tcc"
