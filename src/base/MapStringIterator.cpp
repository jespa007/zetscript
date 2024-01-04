/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"

namespace zetscript{

	void MapStringIterator::setup(){
		map=NULL;
		current_node=NULL;
		key=NULL;
		value=0;
		_end=true;
	}

	MapStringIterator::MapStringIterator(){
		setup();
	}

	MapStringIterator::MapStringIterator(MapString *_map){
		setup();
		map=_map;
		current_node=map->data();
		setCurrentPair();

		if(current_node!=NULL){
			_end=false;
		}
	}

	void MapStringIterator::setCurrentPair(){
		if(current_node==NULL){
			return;
		}
		key=current_node->key;
		value=current_node->value;

	}

	void MapStringIterator::next(){

		if(current_node == NULL){
			return;
		}

		current_node =current_node->next;

		setCurrentPair();

		_end=current_node==map->data();
	}

	bool MapStringIterator::end(){
		return _end;
	}
}
