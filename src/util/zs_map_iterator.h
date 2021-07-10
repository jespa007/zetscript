#pragma once

namespace zetscript{
	class zs_map;
	struct zs_map_node;
	class zs_map_iterator{
	public:

		zs_map_iterator();
		zs_map_iterator(zs_map *map);
		void next();
		bool end();
		const char *getKey();
		zs_int getValue();

	private:
		zs_map_node *current_node;
		zs_map_node *current_list;
		zs_map *map;
		int idx_current_hash;
		const char *key;
		zs_int		value;


		void setup();
		void findNextListNotEmpty();
		void setCurrentPair();
	};

}
