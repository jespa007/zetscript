/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#define ZS_VECTOR_MAX_ELEMENTS	16000
#define ZS_VECTOR_EXPAND_SLOT_ELEMENTS 10

namespace zetscript{

	template<typename _T>
	int zs_vector<_T>::npos=-1;

	template<typename _T>
	bool	zs_vector<_T>::push_back_slot(){
		if (this->_size == 0) {
			this->_size = ZS_VECTOR_EXPAND_SLOT_ELEMENTS;
			this->items = new _T[ this->_size]; //(zs_int *)ZS_MALLOC(sizeof(zs_int) * this->_size);
		}
		// condition to increase this->items:
		// last slot exhausted
		if (this->_size ==this->count) {
			if((this->_size+ ZS_VECTOR_EXPAND_SLOT_ELEMENTS) >= ZS_VECTOR_MAX_ELEMENTS){
				THROW_RUNTIME_ERRORF("Max elements vector");
			}
			this->_size += ZS_VECTOR_EXPAND_SLOT_ELEMENTS;
			_T *aux_buf=new _T[ this->_size];

			// copy.
			for(int i=0; i < this->count; i++){
				aux_buf[i]=this->items[i];// =(zs_int *) realloc(this->items, sizeof(zs_int) * this->_size);
			}

			// delete old buffer
			if(this->items != NULL){
				delete [] this->items;
				this->items=NULL;
			}

			this->items=aux_buf;


		}

		this->count++;
		return true;
	}


	template<typename _T>
	void zs_vector<_T>::copy(const zs_vector<_T> & _vector) {

		if (_vector.items == NULL) { return; } // do not create string from NULL pointers

		clear(); // cleanup any existing data

		count = _vector.count;
		_size = _vector._size;
		this->items = new _T[_size]; //(zs_int*)ZS_MALLOC(sizeof(zs_int) * _size + 1); // + 1 for the keeping the null character
		
		//memcpy(this->items, _vector.items, count*sizeof(zs_int)); // copy from the incoming buffer to character buffer of the new object
		for(int i=0; i < count; i++){
			this->items[i]=_vector.items[i];
		}

	}

	template<typename _T>
	zs_vector<_T>::zs_vector(){
		items=NULL;
		count=0; //number of items
		_size=0;
	}

	template<typename _T>
	zs_vector<_T>::zs_vector(const zs_vector& _vector) // copy constructor
	{
		items = NULL;
		count = _size = 0;
		copy(_vector);
	}

	template<typename _T>
	zs_vector<_T> & zs_vector<_T>::operator=(const zs_vector& _vector) // copy assignment
	{
		copy(_vector);
		return *this;
	}

	template<typename _T>
	void zs_vector<_T>::set(int _pos, const _T & _val){
		if (_pos<0 || _pos >= this->count) {
			THROW_RUNTIME_ERRORF("vector::set => idx out of bounds 1");
			return;
		}
		this->items[_pos] = _val;
	}

	template<typename _T>
	const _T &	zs_vector<_T>::get(int  _pos){
		if (_pos<0 ||_pos >= this->count) {
			THROW_RUNTIME_ERRORF("vector::get => idx out of bounds");
		}

		return this->items[_pos];
	}

	template<typename _T>
	void zs_vector<_T>::erase(int  _pos){
		if (_pos<0 ||_pos >= this->count) {
			THROW_RUNTIME_ERRORF("vector::erase => idx out of bounds");
			return;
		}

		for (int i = _pos; i < (this->count-1); i++) {
			this->items[i] = this->items[i+1];
		}

		this->count--;
	}

	template<typename _T>
	_T zs_vector<_T>::pop_back(){
		_T item;
		if (this->count==0) {
			THROW_RUNTIME_ERRORF("no elements");
			return 0;
		}

		item=this->items[this->count-1];
		erase(this->count-1);
		return item;
	}

	template<typename _T>
	void zs_vector<_T>::resize(int _new_size){
		// the new size is greather (do not copy, only decrease the number of elements)
		if( _new_size <= _size){
			count=_new_size;
			return;
		}

		if(_new_size >=  ZS_VECTOR_MAX_ELEMENTS){
			THROW_RUNTIME_ERROR("Cannot resize zs_vector. Reached maximum",_new_size);
			return;
		}

		_T *new_buf=new _T[_new_size];
		//this->items=(zs_int *)realloc(this->items,_new_size*sizeof(zs_int));

		if(new_buf == NULL){
			THROW_RUNTIME_ERROR("Cannot resize zs_vector to %i bytes",_new_size);
			return;
		}

		// copy old values
		for(int i=0; i < count; i++){
			new_buf[i]= this->items[i];
		}

		// delete old buf
		if(this->items != NULL){
			delete [] this->items;
			this->items=NULL;
		}

		this->items=new_buf;
		this->_size=_new_size;
		//this->count=_new_size;

	}

	template<typename _T>
	bool zs_vector<_T>::push_back(const _T & _val){
		if(push_back_slot()){
			this->items[this->count-1] = _val; // add element to end list...
			return true;
		}
		return false;
	}

	template<typename _T>
	void zs_vector<_T>::concat(const zs_vector<_T> & _vector){
		insert(count,_vector, _vector.count);
	}

	template<typename _T>
	void 		zs_vector<_T>::insert(int  _pos,const zs_vector<_T>  & _src_vector, int _n_elements_src_vector_to_copy){


		if(_src_vector.count==0) { // no insert
			return;
		}

		if(_n_elements_src_vector_to_copy == npos){
			_n_elements_src_vector_to_copy=_src_vector.count;
		}


		if(_src_vector.count < _n_elements_src_vector_to_copy) {
			THROW_RUNTIME_ERROR("there's more elements than the list to insert: list->count (%i) < n_list_elements_to_copy (%i)"
				,_src_vector.count
				,_n_elements_src_vector_to_copy
			);
		}

		if(_pos > count){
			THROW_RUNTIME_ERROR("idx position to insert out of bounds: idx (%i) >= count(%i)",_pos,count);
		}


		// needs to resize
		if(this->_size<=(_n_elements_src_vector_to_copy+count)){
			_size=_n_elements_src_vector_to_copy+count;
			if(_size >=  ZS_VECTOR_MAX_ELEMENTS){
				THROW_RUNTIME_ERROR("Cannot insert. Reached max capacity",_pos,count);
			}
		}

		_T *new_items=new _T[_size];//(zs_int *)ZS_MALLOC(sizeof(zs_int)*_size);

		// copy first part dst_vector
		//memcpy(new_items,items,idx*sizeof(zs_int));
		for(int i=0; i < _pos; i++){
			new_items[i]=this->items[i];
		}


		// copy src_vector -> dst_vector middle
		//memcpy(new_items+idx,list->items,n_list_elements_to_copy*sizeof(zs_int));
		for(int i=0; i < _n_elements_src_vector_to_copy; i++){
			new_items[i+_pos]=_src_vector.items[i];
		}


		// copy last part of dst_vector
		//memcpy(new_items+idx+n_list_elements_to_copy,items+idx,(count-idx)*sizeof(zs_int));
		for(int i=0; i < (count-_pos); i++){
			new_items[i+_pos+_n_elements_src_vector_to_copy]=this->items[i+_pos];
		}

		if(this->items != NULL){
			// free old
			delete [] this->items;
			this->items=NULL;
		}

		// update new
		this->items=new_items;
		count+=_src_vector.count;
	}

	template<typename _T>
	void 		zs_vector<_T>::insert(int  _pos, const _T & _val){
		if(_pos > (this->count+1)){
			THROW_RUNTIME_ERROR("idx should be 0 to %i",this->count+1);
			return;
		}

		if(push_back_slot()){
			// 1. move all elements...
			for(int i=this->count-1;i>_pos;i--){
				this->items[i]=this->items[i-1];
			}
			// 2. Assign element
			this->items[_pos]=_val;
		}
	}

	template<typename _T>
	void 	zs_vector<_T>::clear(){
		if(this->items!=NULL){
			delete [] this->items;
		}

		this->items=NULL;
		this->count=0; //number of items
		this->_size=0;
	}

	template<typename _T>
	zs_vector<_T>::~zs_vector(){
		clear();
	}

}

