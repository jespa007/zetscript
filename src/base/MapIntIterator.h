/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	class MapInt;
	struct MapIntNode;
	class MapIntIterator{
	public:

		zs_int 		key;
		zs_int		value;


		MapIntIterator();
		MapIntIterator(MapInt *map);
		void next();
		bool end();


	private:
		MapIntNode *current_node;
		MapInt *map;
		bool _end;


		void setup();
		void setCurrentPair();
	};

}
