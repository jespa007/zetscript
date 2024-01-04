/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	class MapString;
	struct MapStringNode;
	class MapStringIterator{
	public:

		const char *key;
		zs_int		value;


		MapStringIterator();
		MapStringIterator(MapString *map);
		void next();
		bool end();


	private:
		MapStringNode *current_node;
		MapString *map;
		bool _end;


		void setup();
		void setCurrentPair();
	};

}
