/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	class zs_map;
	struct zs_map_node;
	class zs_map_iterator{
	public:

		const char *key;
		zs_int		value;


		zs_map_iterator();
		zs_map_iterator(zs_map *map);
		void next();
		bool end();


	private:
		zs_map_node *current_node;
		zs_map *map;
		bool _end;


		void setup();
		void setCurrentPair();
	};

}
