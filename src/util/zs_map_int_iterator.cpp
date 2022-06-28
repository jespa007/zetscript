/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void zs_map_int_iterator::setup(){
		map=NULL;
		current_node=NULL;
		key=-1;
		value=0;
		_end=true;
	}

	zs_map_int_iterator::zs_map_int_iterator(){
		setup();
	}

	zs_map_int_iterator::zs_map_int_iterator(zs_map_int *_map){
		setup();
		map=_map;
		current_node=map->data();
		setCurrentPair();

		if(current_node!=NULL){
			_end=false;
		}
	}

	void zs_map_int_iterator::setCurrentPair(){
		if(current_node==NULL){
			return;
		}
		key=current_node->key;
		value=current_node->value;

	}

	void zs_map_int_iterator::next(){

		if(current_node == NULL){
			return;
		}

		current_node =current_node->next;

		setCurrentPair();

		_end=current_node==map->data();
	}

	bool zs_map_int_iterator::end(){
		return _end;
	}
}
