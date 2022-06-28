/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	class zs_map_int;
	struct zs_map_int_node;
	class zs_map_int_iterator{
	public:

		zs_int 		key;
		zs_int		value;


		zs_map_int_iterator();
		zs_map_int_iterator(zs_map_int *map);
		void next();
		bool end();


	private:
		zs_map_int_node *current_node;
		zs_map_int *map;
		bool _end;


		void setup();
		void setCurrentPair();
	};

}
