/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void zs_map_iterator::setup(){
		map=NULL;
		current_node=NULL;
	}

	zs_map_iterator::zs_map_iterator(){
		setup();
	}

	zs_map_iterator::zs_map_iterator(zs_map *_map){
		setup();
		map=_map;
		current_node=map->data();
	}

	void zs_map_iterator::setCurrentPair(){

	}

	void zs_map_iterator::next(){
		if(end()){
			return;
		}

		current_node =current_node->next;
	}

	bool zs_map_iterator::end(){
		return current_node==NULL;
	}
}
