#include "../zetscript.h"



namespace zetscript{

	bool	zs_vector_fast::push_back_slot(){
		if (this->size == 0) {
			this->size = ZS_VECTOR_N_SLOT_ELEMENTS;
			this->items = (zs_int *)malloc(this->size_item * this->size);

		}
		// condition to increase this->items:
		// last slot exhausted
		if (this->size ==this->count) {
			if((this->size+ZS_VECTOR_N_SLOT_ELEMENTS) >= ZS_MAX_ELEMENTS_VECTOR){
				THROW_RUNTIME_ERROR("Max elements vector");
				return false;
			}
			this->size += ZS_VECTOR_N_SLOT_ELEMENTS;
			this->items = realloc(this->items, this->size_item * this->size);
		}

		memset(this->items, 0,  this->size_item * this->count+this->size_item * ZS_VECTOR_N_SLOT_ELEMENTS);

		this->count++;
		return true;
	}

	zs_vector_fast::zs_vector_fast(size_t _size_item){
		items=NULL;
		count=0; //number of items
		size=0;
		size_item=_size_item;
	}

	void zs_vector_fast::set(uint16_t idx, zs_int e){
		if (idx >= this->count) {
			THROW_RUNTIME_ERROR("idx out of bounds");
			return;
		}
		this->items[idx] = e;
	}

	zs_int zs_vector_fast::get(uint16_t  idx){
		if (idx >= this->count) {
			THROW_RUNTIME_ERROR("idx out of bounds");
			return 0;
		}

		return this->items[idx];
	}

	void zs_vector_fast::erase(uint16_t  idx){
		if (idx >= this->count) {
			THROW_RUNTIME_ERROR("idx out of bounds");
			return;
		}

		void *ptr_pos=idx*this->size_item;


		memmove(ptr_pos*size_item,ptr_pos+1*size_item,this->size_item*(this->count-idx));
		memset((this->count-1)*size_item,0,size_item);
		this->count--;
	}

	void zs_vector_fast::pop_back(){
		if (this->count==0) {
			THROW_RUNTIME_ERROR("no elements");
		}

		erase(this->count-1);
	}

	void * zs_vector_fast::push_back(void * e){
		if(push_back_slot()){
			void *ptr_buffer=this->items+(this->count-1)*this->size_item;
			mempcy(ptr_buffer,e,size_item);
			return ptr_buffer;
		}
		return NULL;
	}

	void 		zs_vector_fast::insert(uint16_t  idx, void *e){
		if(idx > (this->count+1)){
			THROW_RUNTIME_ERROR("idx should be 0 to %i",this->count+1);
			return;
		}

		if(push_back_slot()){
			// 1. move all elements...
			void *ptr_pos=idx*this->size_item;
			memmove(ptr_pos+1*size_item,ptr_pos,this->size_item*(this->count-idx));
			// 2. Assign element
			memcpy(ptr_pos,e,this->size_item);
		}
	}

	void 		zs_vector_fast::clear(){
		if(this->items!=NULL){
			free(this->items);
		}

		this->items=NULL;
		this->count=0; //number of items
		this->size=0;
	}

	zs_vector_fast::~zs_vector_fast(){
		if(this->items!=NULL){
			free(this->items);
		}
	}


}

