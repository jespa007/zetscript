#include "../zetscript.h"

#define ZS_MAX_ELEMENTS_VECTOR	16000

namespace zetscript{

	bool	zs_vector::push_back_slot(){
		if (this->_size == 0) {
			this->_size = 10;
			this->items = (intptr_t *)malloc(sizeof(intptr_t) * this->_size);
			memset(this->items, '\0', sizeof(intptr_t) * this->_size);
		}
		// condition to increase this->items:
		// last slot exhausted
		if (this->_size ==this->count) {
			if((this->_size+10) >= ZS_MAX_ELEMENTS_VECTOR){
				THROW_RUNTIME_ERROR("Max elements vector");
				return false;
			}
			this->_size += 10;
			this->items =(intptr_t *) realloc(this->items, sizeof(intptr_t) * this->_size);
		}

		this->count++;
		return true;
	}

	zs_vector::zs_vector(){
		items=NULL;
		count=0; //number of items
		_size=0;
	}

	void zs_vector::set(uint16_t idx, intptr_t e){
		if (idx >= this->count) {
			THROW_RUNTIME_ERROR("idx out of bounds");
			return;
		}
		this->items[idx] = e;
	}

	intptr_t zs_vector::get(uint16_t idx){
		if (idx >= this->count) {
			THROW_RUNTIME_ERROR("idx out of bounds");
			return 0;
		}

		return this->items[idx];
	}

	void zs_vector::erase(uint16_t idx){
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

	intptr_t zs_vector::pop_back(){
		intptr_t item=0;
		if (this->count==0) {
			THROW_RUNTIME_ERROR("no elements");
			return 0;
		}

		item=this->items[this->count-1];
		erase(this->count-1);

		return item;
	}

	bool zs_vector::push_back(intptr_t e){
		if(push_back_slot()){
			this->items[this->count-1] = e; // add element to end list...
			return true;
		}
		return false;
	}

	void zs_vector::concat(zs_vector *list){
		if( list!=NULL) {
			for(unsigned i=0; i <  list->count; i++){
				push_back(list->items[i]);
			}
		}
	}

	void 		zs_vector::insert(uint16_t idx, intptr_t e){
		if(idx > (this->count+1)){
			THROW_RUNTIME_ERROR(zs_strutils::format("idx should be 0 to %i",this->count+1));
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

