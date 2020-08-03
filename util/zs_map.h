#pragma once

namespace zetscript{

	struct zs_map_node{
		char  *key;
		intptr_t val;
		zs_map_node *next;
		zs_map_node *previous;
	};

	class zs_map{
	public:
		uint16_t count;
		zs_map_node **list;

		zs_map();

		bool		exist(const char * key);
		void 		set(const char * key,intptr_t val);
		intptr_t 	get(const char * key);
		void  		erase(const char * key);

		~zs_map();
	private:

		zs_map_node * 	lookup_node(const char * key);
		uint8_t 		hash_code(const char * key);
	};

}






#
