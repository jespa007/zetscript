/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"

namespace zetscript{

	void MapIntIterator::setup(){
		map=NULL;
		current_node=NULL;
		key=-1;
		value=0;
		_end=true;
	}

	MapIntIterator::MapIntIterator(){
		setup();
	}

	MapIntIterator::MapIntIterator(MapInt *_map){
		setup();
		map=_map;
		current_node=map->data();
		setCurrentPair();

		if(current_node!=NULL){
			_end=false;
		}
	}

	void MapIntIterator::setCurrentPair(){
		if(current_node==NULL){
			return;
		}
		key=current_node->key;
		value=current_node->value;

	}

	void MapIntIterator::next(){

		if(current_node == NULL){
			return;
		}

		current_node =current_node->next;

		setCurrentPair();

		_end=current_node==map->data();
	}

	bool MapIntIterator::end(){
		return _end;
	}
}
