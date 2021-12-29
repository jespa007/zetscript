/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once



namespace zetscript{


	struct zs_map_item{
		char  *key;
		zs_int value;
	};

	class zs_map{
	public:
		uint16_t 	count; //number of items
		zs_map_item  *items; // an ordered indexed of nodes, it can iterate through it

		zs_map();
		bool		exist(const char * key);
		void 		set(const char * key,zs_int val);
		zs_int 		get(const char * key, bool * exists=NULL);
		void  		erase(const char * key);
		void 		clear();

		~zs_map();

	private:
		void		push_back_slot();
		int			search(const char * key);

		unsigned int _size; // size vector (user count for iterate through items)

	};

}

