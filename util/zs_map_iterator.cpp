#include "zetscript.h"

namespace zetscript{

	void zs_map_iterator::setup(){
		map=NULL;
		current_node=NULL;
		current_list=NULL;
		idx_current_hash=0;
	}

	zs_map_iterator::zs_map_iterator(){
		setup();

	}

	zs_map_iterator::zs_map_iterator(zs_map *_map){
		setup();
		map=_map;
		current_node=NULL;
		current_list=NULL;
		idx_current_hash=0;
		findNextListNotEmpty();
	}

	const char *zs_map_iterator::getKey(){
		return key;
	}

	zs_int zs_map_iterator::getValue(){
		return value;
	}

	void zs_map_iterator::setCurrentPair(){
		key=current_node->key;
		value=current_node->val;
	}


	void zs_map_iterator::findNextListNotEmpty(){
		for(; idx_current_hash < HASH_MAP_SIZE && current_list==NULL; idx_current_hash++){
			if(map->list[idx_current_hash] != NULL){
				current_list=(zs_map_node *)map->list[idx_current_hash];
				current_node=current_list;
				setCurrentPair();
			}
		}
	}

	void zs_map_iterator::next(){

		if(current_node->next == current_node->previous){ // set current list as null for next search
			current_list=NULL;
		}else{
			current_node =current_node->next;
		}

		if(current_list==NULL){ // get first node not NULL
			findNextListNotEmpty();
		}else{
			//else if(current_node != NULL){
			// set current key/value
			setCurrentPair();
		}
		//}
	}

	bool zs_map_iterator::end(){
		return idx_current_hash == HASH_MAP_SIZE;
	}
}
