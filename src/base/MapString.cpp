/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@util.h"

#define ZS_MAP_KEY_NOT_FOUND 	-1
#define ZS_MAP_MAX_ITEMS  		1000000
#define ZS_MAP_N_SLOT_ITEMS 	16

namespace zetscript{

	int			MapString::search(const char * _key){

		int idx_min=0;
		int idx_max=(int)_count-1;

		// trivial case (no items)
		if(idx_max<0){
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

	void	MapString::pushSlot(){
		if (this->_size == 0) {
			this->_size = ZS_MAP_N_SLOT_ITEMS;
			this->items = (MapStringItem *)ZS_MALLOC(sizeof(MapStringItem) * this->_size);
		}
		// condition to increase this->items:
		// last slot exhausted
		if (this->_size ==this->_count) {
			if((this->_size+ZS_MAP_N_SLOT_ITEMS) >= ZS_MAP_MAX_ITEMS){
				ZS_THROW_RUNTIME_ERRORF("Reached Max elements");
				return;
			}
			this->_size += ZS_MAP_N_SLOT_ITEMS;
			this->items =(MapStringItem *) realloc(this->items, sizeof(MapStringItem) * this->_size);
		}

		this->_count++;

	}

	MapString::MapString(){
		items=NULL;
		_count=0; //number of items
		_size=0;
		first=NULL;
		last=NULL;
	}

	bool	MapString::exist(const char * _key){
		return search(_key)!=ZS_MAP_KEY_NOT_FOUND;
	}

	int 		MapString::count()
	{
		return _count;
	}

	int MapString::newSlot(const char *_key){

		int max_size=_count;
		int idx_min = 0, idx_max = _count - 1;

		if(max_size > 0){

			if (strcmp(items[idx_max].key,_key) < 0){
			  idx_min=max_size;
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

		// increment new slot
		pushSlot();

		for(int i=this->_count-1;i>idx_min;i--){
			this->items[i]=this->items[i-1];
		}

		return idx_min;
	}

	MapStringNode	*MapString::set(const char * _key,zs_int _value){

		int idx=search(_key);
		MapStringItem *item=NULL;
		char *key=NULL;
		MapStringNode *node = NULL;

		if(idx != ZS_MAP_KEY_NOT_FOUND){
			item=items+idx;
		}else{

			//------------------------------------------
			// 1st CREATE NODE AND ADD AT THE END OF LIST
			node = (MapStringNode *)ZS_MALLOC(sizeof(MapStringNode));

			if(first == NULL){ /*one  node: trivial ?*/
				node->previous=node->next= last = first =node;
			}
			else{ /* >1 node add to the end */
				// attach last-previous
				node->previous=last;
				last->next=node;
				last=node;

				// attach next
				node->next=first;
				first->previous=node;
			}

			//------------------------------------------
			// 2nd CREATE IDX AND ADD TO LOOKUP TABLE
			int idx_position=newSlot(_key);


			item=items+idx_position;

			key=(char *)ZS_MALLOC(strlen(_key)+1);
			strcpy(key,_key);

			item->key = key;
			item->node=node;
			node->key=key;
		}


		item->node->value=_value;

		return node;
	}

	MapStringIterator MapString::begin(){
		return MapStringIterator(this);
	}

	zs_int	MapString::get(const char * _key, bool * _exist){

		if(_key == NULL){
			ZS_THROW_RUNTIME_ERROR("key NULL",_key);
		}

		int idx=search(_key);
		zs_int value=0;

		if(idx!=ZS_MAP_KEY_NOT_FOUND){
			value=items[idx].node->value;
		}

		if(_exist!=NULL){
			*_exist=idx!=ZS_MAP_KEY_NOT_FOUND;
		}

		return value;
	}

	const char *MapString::getKey(const char * _key){
		if(_key == NULL){
			ZS_THROW_RUNTIME_ERROR("key NULL",_key);
		}

		int idx=search(_key);
		char *key=NULL;

		if(idx!=ZS_MAP_KEY_NOT_FOUND){
			key=items[idx].node->key;
		}

		return key;
	}

	void	MapString::erase(const char * _key){

		int idx=search(_key);

		if(idx==ZS_MAP_KEY_NOT_FOUND){
			ZS_THROW_RUNTIME_ERROR("key '%s' not found",_key);
		}

		MapStringNode *node = items[idx].node;

		if((node->previous == node) && (node->next == node)){ // 1 single node...
			last=first=NULL;
		}
		else{ // dettach and attach next...
			// [1]<->[2]<-> ...[P]<->[C]<->[N]...[M-1]<->[M]
			if(node == first){
				first=node->next;
			}
			else if(node == last){
				last=node->previous;
			}
			node->previous->next=node->next;
			node->next->previous=node->previous;
		}

		node->previous = node->next = NULL;
		free(node->key);
		free(node);

		// erase item from lookup list
		// free allocated mem
		for (int i = idx; i < (int)((this->_count-1)); i++) {
			this->items[i] = this->items[i+1];
		}

		// reset last value
		this->items[this->_count-1].key=NULL;
		this->items[this->_count-1].node=NULL;

		this->_count--;
	}

	MapStringNode *MapString::data(){
		return first;
	}

	void	MapString::clear(){
		// clear first
		MapStringNode * node=this->first;

		if(node != NULL){

			node=node->next;
			while(node!=first){
				MapStringNode * to_deallocate = node;
				node=node->next;
				free(to_deallocate);
			}

			// deallocate first one
			free(first);
		}


		first=NULL;
		last=NULL;

		// clear ordered lookup list
		for(int i=0; i < _count; i++){
			free((items+i)->key);
		}

		if(items != NULL){
			free(items);
			items=NULL;
		}

		_count=0;
		_size=0;
	}

	MapString::~MapString(){
		clear();
	}

}
