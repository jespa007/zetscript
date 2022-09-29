/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once



namespace zetscript{


	struct zs_map_int_node{
		zs_int key;
		zs_int value;
		zs_map_int_node *next;
		zs_map_int_node *previous;
	};


	class zs_map_int{
	public:
		zs_map_int();
		bool		exist(zs_int key);
		zs_map_int_iterator begin();
		void 		set(zs_int key,zs_int val);
		zs_int 		get(zs_int key, bool * exists=NULL);
		zs_int		getValueByIdx(zs_int _idx);
		void		eraseByIdx(zs_int _idx);
		void  		erase(zs_int key);
		void 		clear();
		zs_map_int_node *data();
		size_t 		count();
		~zs_map_int();

	private:
		struct zs_map_int_item{
			zs_int  		key;
			zs_map_int_node *node;
		};

		zs_map_int_item  		*items; // an ordered indexed of nodes, it can iterate through it
		zs_map_int_node 		*first;
		zs_map_int_node 		*last;

		void		push_back_slot();
		int			search(zs_int key);
		int			new_slot(zs_int _key);
		unsigned int _size; // size vector (user count for iterate through items)
		uint16_t _count;

	};

}

