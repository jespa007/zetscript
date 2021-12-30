/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../zetscript.h"

#define ZS_MAP_KEY_NOT_FOUND 	-1
#define ZS_MAP_MAX_ELEMENTS  	16000
#define ZS_MAP_N_SLOT_ELEMENTS 	10

namespace zetscript{

	int			zs_map::search(const char * _key){

		int idx_min=0;
		int idx_max=(int)count-1;

		if(idx_max<=0){
			return ZS_MAP_KEY_NOT_FOUND;
		}

		// continue searching while [imin,imax] is not empty
		while (idx_max >= idx_min)
		{
			// calculate the midpoint for roughly equal partition
			int idx_mid = (idx_min + idx_max ) >> 1;
			if(strcmp(items[idx_mid].key,_key)==0){
				// key found at index idx_mid
				return idx_mid;
			// determine which subarray to search
			}else if (strcmp(items[idx_mid].key,_key)<0){ // item.key < key
				// change min index to search upper subarray
				idx_min = idx_mid + 1;
			}else{ // item.key > key
				// change max index to search lower subarray
				idx_max = idx_mid - 1;
			}
		}
		// key was not found
		return ZS_MAP_KEY_NOT_FOUND;
	}

	void	zs_map::push_back_slot(){
		if (this->_size == 0) {
			this->_size = ZS_MAP_N_SLOT_ELEMENTS;
			this->items = (zs_map_item *)ZS_MALLOC(sizeof(zs_map_item) * this->_size);
		}
		// condition to increase this->items:
		// last slot exhausted
		if (this->_size ==this->count) {
			if((this->_size+ZS_MAP_N_SLOT_ELEMENTS) >= ZS_MAP_MAX_ELEMENTS){
				THROW_RUNTIME_ERROR("Max elements vector");
				return;
			}
			this->_size += ZS_MAP_N_SLOT_ELEMENTS;
			this->items =(zs_map_item *) realloc(this->items, sizeof(zs_map_item) * this->_size);
		}

		this->count++;

	}

	zs_map::zs_map(){
		items=NULL;
		count=0; //number of items
		_size=0;
		list=NULL;
	}

	bool	zs_map::exist(const char * _key){
		return search(_key)!=ZS_MAP_KEY_NOT_FOUND;
	}
	void	zs_map::set(const char * _key,zs_int _value){

		int idx=search(_key);
		zs_map_item *item=NULL;

		if(idx != ZS_MAP_KEY_NOT_FOUND){
			item=items+idx;
		}else{

			// 1st CREATE NODE AND ADD AT THE END OF LIST
			zs_map_node *node = NULL;

			uint8_t pos = hash_code(key);
			zs_map_node *first_node = this->list[pos];
			node = (zs_map_node *)ZS_MALLOC(sizeof(zs_map_node));

			// insert node at the end (not ordered)
			size_t key_len=strlen(key)+1;
			node->key = (char *)ZS_MALLOC(key_len);

			strcpy(node->key,key);
			node->val = val;
			node->next = first_node;
			node->previous=NULL;

			if(first_node != NULL){
				first_node->previous=node;
			}

			this->list[pos] = node; //Replace first node by _this one...

			// 2nd CREATE IDX AND ADD TO LOOKUP TABLE
			node->val = val;

			push_back_slot();

			int size=count;
			int idx_min = 0, idx_max = count - 1;

			if(size > 0){

				if (strcmp(items[idx_max].key,_key) < 0){
				  idx_min=size;
				}
				else{
				  while (idx_max > idx_min) {
					int idx_mid = (idx_min + idx_max) >> 1;
					if (strcmp(items[idx_mid].key,_key) > 0) {
						idx_max = idx_mid;
					}
					else{
						idx_min = idx_mid + 1;
					}
				  }
				}
			}

			// move all data..
			if(idx_min >= 0){

				if(idx_min < size){
					// 1. move all elements...
					for(int i=this->count-1;i>idx_min;i--){
						this->items[i]=this->items[i-1];
					}
				}
			}

			item=items+idx_min;
		}

		char *key=(char *)ZS_MALLOC(strlen(_key)+1);
		strcpy(key,_key);

		item->key = key;
		item->value = _value;
	}

	zs_map_iterator zs_map::begin(){
		return zs_map_iterator(this);
	}

	zs_int	zs_map::get(const char * _key, bool * _exist){
		int idx=search(_key);
		zs_int value=0;

		if(idx!=ZS_MAP_KEY_NOT_FOUND){
			value=items[idx].value;
		}

		if(_exist!=NULL){
			*_exist=idx!=ZS_MAP_KEY_NOT_FOUND;
		}

		return value;


	}

	void	zs_map::erase(const char * _key){

		int idx=search(_key);

		if(idx==ZS_MAP_KEY_NOT_FOUND){
			THROW_RUNTIME_ERROR("key '%s' not found");
		}

		// erase node

		// not first...
		if(node->previous != NULL){
			node->previous->next=node->next; // link previous
		}else{ // first, set first element as next
			this->list[pos]=node->next;
		}

		// not last
		if(node->next!=NULL){
			node->next->previous=node->previous; // link previous-next
		}

		free(node->key);
		free(node);

		// erase item from lookup list
		// free allocated mem
		free(this->items[idx].key);

		// reset values to 0
		this->items[idx].key=NULL;
		this->items[idx].value=0;

		for (int i = idx; i < (int)((this->count-1)); i++) {
			this->items[i] = this->items[i+1];
		}


		this->count--;
	}

	void	zs_map::clear(){

		// clear nodes
		for(unsigned i=0;i<this->count;i++){
					zs_map_node * temp=this->list[i];

					while(temp){
						zs_map_node * to_deallocate = temp;
						temp=temp->next;

						/*if(t->on_delete != NULL){
							t->on_delete(to_deallocate);
						}*/

						free(to_deallocate->key);
						free(to_deallocate);

					}
				}
				memset(this->list,0,sizeof(zs_map_node*)*this->count);

		// clear ordered lookup list
		for(int i=0; i < count; i++){
			free((items+i)->key);
		}
		free(items);
		items=NULL;
		count=0;
		_size=0;
	}

	zs_map::~zs_map(){
		clear();
	}

}
