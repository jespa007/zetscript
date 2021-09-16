#pragma once

#define HASH_MAP_SIZE 256

namespace zetscript{

	struct zs_map_node_int{
		zs_int key;
		zs_int val;
		zs_map_node_int *next;
		zs_map_node_int *previous;
	};

	class zs_map_int{
	public:
		uint16_t count;
		zs_map_node_int **list;

		zs_map_int();
		bool		exist(zs_int key);
		void 		set(zs_int key,zs_int val);
		zs_int 		get(zs_int key, bool * exists=NULL);
		void  		erase(zs_int key);
		void 		clear();

		~zs_map_int();
	private:

		zs_map_node_int * 	lookup_node(zs_int key);
		uint8_t 		hash_code(zs_int key);
	};

}




