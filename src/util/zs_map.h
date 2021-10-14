/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define HASH_MAP_SIZE 256

namespace zetscript{

	struct zs_map_node{
		char  *key;
		zs_int val;
		zs_map_node *next;
		zs_map_node *previous;
	};

	class zs_map{
	public:
		uint16_t count;
		zs_map_node **list;

		zs_map();
		zs_map_iterator begin();
		bool		exist(const char * key);
		void 		set(const char * key,zs_int val);
		zs_int 		get(const char * key, bool * exists=NULL);
		void  		erase(const char * key);
		void clear();

		~zs_map();
	private:

		zs_map_node * 	lookup_node(const char * key);
		uint8_t 		hash_code(const char * key);
	};

}




