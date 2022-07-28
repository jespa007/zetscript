/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once



namespace zetscript{


	struct zs_map_node{
		char  *key;
		zs_int value;
		zs_map_node *next;
		zs_map_node *previous;
	};


	class zs_map{
	public:
		zs_map();
		bool		exist(const char * key);
		zs_map_iterator begin();
		void 		set(const char * key,zs_int val);
		zs_int 		get(const char * key, bool * exists=NULL);
		void  		erase(const char * key);
		void 		clear();
		zs_map_node *data();
		int 		count();
		~zs_map();

	private:
		struct zs_map_item{
			char  		*key;
			zs_map_node *node;
		};

		zs_map_item  		*items; // an ordered indexed of nodes, it can iterate through it
		zs_map_node 		*first;
		zs_map_node 		*last;

		void		push_back_slot();
		int			search(const char * key);
		int			new_slot(const char *_key);
		 int _size; // size vector (user count for iterate through items)
		int _count;

	};

}

