/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"

#define ZS_MAP_INT_KEY_NOT_FOUND 	-1
#define ZS_MAP_INT_MAX_ITEMS  			16000
#define ZS_MAP_INT_N_SLOT_ITEMS 		10

namespace zetscript{

	int			MapInt::search(zs_int _key){

		int idx_min=0;
		int idx_max=(int)_count-1;

		// trivial case (no items)
		if(idx_max<0){
			return ZS_MAP_INT_KEY_NOT_FOUND;
		}

		// continue searching while [imin,imax] is not empty
		while (idx_max >= idx_min)
		{
			// calculate the midpoint for roughly equal partition
			int idx_mid = (idx_min + idx_max ) >> 1;
			if(items[idx_mid].key==_key){
				// key found at index idx_mid
				return idx_mid;
			// determine which subarray to search
			}else if (items[idx_mid].key<_key){ // item.key < key
				// change min index to search upper subarray
				idx_min = idx_mid + 1;
			}else{ // item.key > key
				// change max index to search lower subarray
				idx_max = idx_mid - 1;
			}
		}
		// key was not found
		return ZS_MAP_INT_KEY_NOT_FOUND;
	}

	void	MapInt::pushSlot(){
		if (this->_size == 0) {
			this->_size = ZS_MAP_INT_N_SLOT_ITEMS;
			this->items = (MapIntItem *)ZS_MALLOC(sizeof(MapIntItem) * this->_size);
		}
		// condition to increase this->items:
		// last slot exhausted
		if (this->_size ==this->_count) {
			if((this->_size+ZS_MAP_INT_N_SLOT_ITEMS) >= ZS_MAP_INT_MAX_ITEMS){
				ZS_THROW_RUNTIME_ERRORF("Reached Max elements");
				return;
			}
			this->_size += ZS_MAP_INT_N_SLOT_ITEMS;
			this->items =(MapIntItem *) realloc(this->items, sizeof(MapIntItem) * this->_size);
		}

		this->_count++;

	}

	MapInt::MapInt(){
		items=NULL;
		_count=0; //number of items
		_size=0;
		first=NULL;
		last=NULL;
	}

	bool	MapInt::exist(zs_int _key){
		return search(_key)!=ZS_MAP_INT_KEY_NOT_FOUND;
	}

	size_t 		MapInt::count()
	{
		return _count;
	}

	int MapInt::newSlot(zs_int _key){

		int max_size=_count;
		int idx_min = 0, idx_max = _count - 1;

		if(max_size > 0){

			if (items[idx_max].key<_key){
			  idx_min=max_size;
			}
			else{
			  while (idx_max > idx_min) {
				int idx_mid = (idx_min + idx_max) >> 1;
				if (items[idx_mid].key>_key) {
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

	void	MapInt::set(zs_int _key,zs_int _value){

		int idx=search(_key);
		MapIntItem *item=NULL;
		MapIntNode *node = NULL;

		if(idx != ZS_MAP_INT_KEY_NOT_FOUND){
			item=items+idx;
		}else{

			//------------------------------------------
			// 1st CREATE NODE AND ADD AT THE END OF LIST
			node = (MapIntNode *)ZS_MALLOC(sizeof(MapIntNode));

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

			item->key = _key;
			item->node=node;
			node->key=_key;
		}


		item->node->value=_value;
	}

	MapIntIterator MapInt::begin(){
		return MapIntIterator(this);
	}

	zs_int	MapInt::get(zs_int _key, bool * _exist){

		int idx=search(_key);
		zs_int value=0;

		if(idx!=ZS_MAP_INT_KEY_NOT_FOUND){
			value=items[idx].node->value;
		}

		if(_exist!=NULL){
			*_exist=idx!=ZS_MAP_INT_KEY_NOT_FOUND;
		}

		return value;
	}

	zs_int		MapInt::getValueByIdx(zs_int _idx){
		if(_idx < 0 || _idx >= _count){
			ZS_THROW_RUNTIME_ERRORF("getIdx out of bound");
		}

		return this->items[_idx].node->value;
	}

	void		MapInt::eraseByIdx(zs_int _idx){
		if(_idx < 0 || _idx >= _count){
			ZS_THROW_RUNTIME_ERRORF("getIdx out of bound");
		}

		MapIntNode *node = items[_idx].node;

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
		free(node);

		// erase item from lookup list
		for (int i = _idx; i < (int)((this->_count-1)); i++) {
			this->items[i] = this->items[i+1];
		}

		// reset last value
		this->items[this->_count-1].key=0;
		this->items[this->_count-1].node=NULL;

		this->_count--;
	}

	void	MapInt::erase(zs_int _key){

		int idx=search(_key);

		if(idx==ZS_MAP_INT_KEY_NOT_FOUND){
			ZS_THROW_RUNTIME_ERROR("key '%lu' not found",_key);
		}

		eraseByIdx(idx);

	}

	MapIntNode *MapInt::data(){
		return first;
	}

	void	MapInt::clear(){
		// clear first
		MapIntNode * node=this->first;

		if(node != NULL){

			node=node->next;
			while(node!=first){
				MapIntNode * to_deallocate = node;
				node=node->next;
				free(to_deallocate);
			}

			// deallocate first one
			free(first);
		}


		first=NULL;
		last=NULL;

		// clear ordered lookup list
		if(items != NULL){
			free(items);
			items=NULL;
		}

		_count=0;
		_size=0;
	}

	MapInt::~MapInt(){
		clear();
	}

}
