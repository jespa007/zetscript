/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../zetscript.h"

#define ZS_VECTOR_MAX_ELEMENTS	16000
#define ZS_VECTOR_EXPAND_SLOT_ELEMENTS 10
namespace zetscript{

	int zs_vector::npos=-1;

	bool	zs_vector::push_back_slot(){
		if (this->_size == 0) {
			this->_size = ZS_VECTOR_MAX_ELEMENTS;
			this->items = (zs_int *)ZS_MALLOC(sizeof(zs_int) * this->_size);
		}
		// condition to increase this->items:
		// last slot exhausted
		if (this->_size ==this->count) {
			if((this->_size+ ZS_VECTOR_EXPAND_SLOT_ELEMENTS) >= ZS_VECTOR_MAX_ELEMENTS){
				THROW_RUNTIME_ERRORF("Max elements vector");
				return false;
			}
			this->_size += ZS_VECTOR_EXPAND_SLOT_ELEMENTS;
			this->items =(zs_int *) realloc(this->items, sizeof(zs_int) * this->_size);
		}

		this->count++;
		return true;
	}


	void zs_vector::copy(const zs_vector & _vector) {

		if (_vector.items == NULL) { return; } // do not create string from NULL pointers

		__cleanup__(); // cleanup any existing data
		count = _vector.count;
		_size = _vector._size;
		this->items = (zs_int*)ZS_MALLOC(sizeof(zs_int) * _size + 1); // + 1 for the keeping the null character
		
		memcpy(this->items, _vector.items, count*sizeof(zs_int)); // copy from the incoming buffer to character buffer of the new object
	}

	zs_vector::zs_vector(){
		items=NULL;
		count=0; //number of items
		_size=0;
	}

	zs_vector::zs_vector(const zs_vector& _vector) // copy constructor
	{
		items = NULL;
		count = _size = 0;
		copy(_vector);
	}

	zs_vector& zs_vector::operator=(const zs_vector& _vector) // copy assignment
	{
		copy(_vector);
		return *this;
	}

	void zs_vector::set(int idx, zs_int e){
		if (idx >= this->count) {
			THROW_RUNTIME_ERRORF("vector::set => idx out of bounds 1");
			return;
		}
		this->items[idx] = e;
	}

	zs_int zs_vector::get(int  idx){
		if (idx >= this->count) {
			THROW_RUNTIME_ERRORF("vector::get => idx out of bounds");
			return 0;
		}

		return this->items[idx];
	}

	void zs_vector::erase(int  idx){
		if (idx >= this->count) {
			THROW_RUNTIME_ERRORF("vector::erase => idx out of bounds");
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
			THROW_RUNTIME_ERRORF("no elements");
			return 0;
		}

		item=this->items[this->count-1];
		erase(this->count-1);

		return item;
	}

	void zs_vector::resize(int _new_size){
		if (_new_size==0) {
			if(this->items!=NULL){
				free(this->items);
				this->items=NULL;
				this->count=0;
				this->_size=0;
			}
			return;
		}

		this->items=(zs_int *)realloc(this->items,_new_size*sizeof(zs_int));

		if(this->items == NULL){
			THROW_RUNTIME_ERROR("Cannot resize zs_vector to %i bytes",_new_size);
			return;
		}

		this->_size=_new_size;
		this->count=_new_size;

	}

	bool zs_vector::push_back(zs_int e){
		if(push_back_slot()){
			this->items[this->count-1] = e; // add element to end list...
			return true;
		}
		return false;
	}

	void zs_vector::concat(zs_vector * list){
		insert(count,list, list->count);
	}

	void 		zs_vector::insert(int  idx,zs_vector  * list, int n_list_elements_to_copy){


		if(list==NULL || list->count==0) { // not insert
			return;
		}

		if(n_list_elements_to_copy == npos){
			n_list_elements_to_copy=list->count;
		}


		if(list->count < n_list_elements_to_copy) THROW_RUNTIME_ERROR("there's more elements than the list to insert: list->count (%i) < n_list_elements_to_copy (%i)",list->count,n_list_elements_to_copy);
		if(idx > count) THROW_RUNTIME_ERROR("idx position to insert out of bounds: idx (%i) >= count(%i)",idx,count);


		// needs to resize
		if(this->_size<=(n_list_elements_to_copy+count)){
			_size=n_list_elements_to_copy+count;
		}

		zs_int *new_items=(zs_int *)ZS_MALLOC(sizeof(zs_int)*_size);

		// 1st part
		memcpy(new_items,items,idx*sizeof(zs_int));

		// middle
		memcpy(new_items+idx,list->items,n_list_elements_to_copy*sizeof(zs_int));


		// last
		memcpy(new_items+idx+n_list_elements_to_copy,items+idx,(count-idx)*sizeof(zs_int));

		// free old
		if(items != NULL){
			free(items);
		}

		// update new
		items=new_items;
		count+=list->count;
	}

	void 		zs_vector::insert(int  idx, zs_int e){
		if(idx > (this->count+1)){
			THROW_RUNTIME_ERROR("idx should be 0 to %i",this->count+1);
			return;
		}

		if(push_back_slot()){
			// 1. move all elements...
			for(int i=(int)this->count-1;i>(int)idx;i--){
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

	void zs_vector::__cleanup__()
	{
		if (this->items != NULL) {
			free(this->items);
		}

		this->count = 0;
		this->_size = 0;
	}


	zs_vector::~zs_vector(){
		__cleanup__();
	}

}

