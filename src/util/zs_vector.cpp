#include "../zetscript.h"

#define ZS_MAX_ELEMENTS_VECTOR	16000

namespace zetscript{

	bool	zs_vector::push_back_slot(){
		if (this->_size == 0) {
			this->_size = ZS_VECTOR_N_SLOT_ELEMENTS;
			this->items = (zs_int *)malloc(sizeof(zs_int) * this->_size);
			memset(this->items, '\0', sizeof(zs_int) * this->_size);
		}
		// condition to increase this->items:
		// last slot exhausted
		if (this->_size ==this->count) {
			if((this->_size+ZS_VECTOR_N_SLOT_ELEMENTS) >= ZS_MAX_ELEMENTS_VECTOR){
				THROW_RUNTIME_ERROR("Max elements vector");
				return false;
			}
			this->_size += ZS_VECTOR_N_SLOT_ELEMENTS;
			this->items =(zs_int *) realloc(this->items, sizeof(zs_int) * this->_size);
		}

		this->count++;
		return true;
	}

	zs_vector::zs_vector(){
		items=NULL;
		count=0; //number of items
		_size=0;
	}

	void zs_vector::set(uint16_t idx, zs_int e){
		if (idx >= this->count) {
			THROW_RUNTIME_ERROR("idx out of bounds");
			return;
		}
		this->items[idx] = e;
	}

	zs_int zs_vector::get(uint16_t  idx){
		if (idx >= this->count) {
			THROW_RUNTIME_ERROR("idx out of bounds");
			return 0;
		}

		return this->items[idx];
	}

	void zs_vector::erase(uint16_t  idx){
		if (idx >= this->count) {
			THROW_RUNTIME_ERROR("idx out of bounds");
			return;
		}

		this->items[idx] = 0;

		for (int i = idx; i < (int)((this->count-1)); i++) {
			this->items[i] = this->items[i+1];
		}

		this->items[this->count-1] = 0;
		this->count--;
	}

	zs_int zs_vector::pop_back(){
		zs_int item=0;
		if (this->count==0) {
			THROW_RUNTIME_ERROR("no elements");
			return 0;
		}

		item=this->items[this->count-1];
		erase(this->count-1);

		return item;
	}

	void zs_vector::resize(size_t _size){
		if (_size==0) {
			THROW_RUNTIME_ERROR("_size 0");
			return;
		}

		this->items=(zs_int *)realloc(this->items,_size*sizeof(zs_int));

		if(this->items == NULL){
			THROW_RUNTIME_ERROR("NULL pointer");
			return;
		}

	}

	bool zs_vector::push_back(zs_int e){
		if(push_back_slot()){
			this->items[this->count-1] = e; // add element to end list...
			return true;
		}
		return false;
	}

	void zs_vector::concat(zs_vector * list){
		if( list!=NULL && list->count>0) {

			if(this->_size<=(list->count+count)){
				_size=list->count+count;
				zs_int *new_items=(zs_int *)realloc(items,sizeof(size_t)*_size);
				memcpy(new_items+count,list->items,list->count*sizeof(size_t));
				this->items=new_items;
			}else{
				memcpy(items+count,list->items,list->count*sizeof(size_t));
			}

			count+=list->count;
		}
	}

	void 		zs_vector::insert(uint16_t  idx, zs_int e){
		if(idx > (this->count+1)){
			THROW_RUNTIME_ERROR("idx should be 0 to %i",this->count+1);
			return;
		}

		if(push_back_slot()){
			// 1. move all elements...
			for(int i=this->count-1;i>idx;i--){
				this->items[i]=this->items[i-1];
			}
			// 2. Assign element
			this->items[idx]=e;
		}
	}

	void 		zs_vector::clear(){
		if(this->items!=NULL){
			free(this->items);
		}

		this->items=NULL;
		this->count=0; //number of items
		this->_size=0;
	}

	/*void 		zs_vector::free_all_items_and_clear(){
		if(this->items!=NULL){
			for(unsigned i=0; i < this->count; i++){
				free((void *)this->items[i]);
			}
			this->clear();
		}
	}*/

	zs_vector::~zs_vector(){
		if(this->items!=NULL){
			free(this->items);
		}
	}

	/*void deleteAndFreeAllItems(zs_vector *v){
		for(unsigned i=0; i < this->count; i++){
			free(this->items[i]);
		}
		deleteList(v);
	}
*/

}

