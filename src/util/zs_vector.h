/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once



namespace zetscript{

	template<typename _T>
	class zs_vector{
	public:

		static size_t npos;
		//public
		_T* items;
		size_t count; //number of items

		zs_vector();
		zs_vector(const zs_vector & _obj);
		zs_vector& operator=(const zs_vector& _obj);

		void 		set( unsigned  _idx, const _T & _val);
		const _T &	get( unsigned  _idx);
		void 		erase( unsigned  _idx);
		/**
		 * Insert item at the end
		 */
		bool 		push_back( const _T & _val);
		void 		concat(const zs_vector<_T>  & _vector);
		void 		insert(unsigned _idx,const zs_vector<_T>  & _vector, size_t _n_list_elements_to_copy=npos);
		/**
		 * Insert item at position idx.
		 */
		void 		insert(unsigned  idx, const _T & _val);

		void 		clear();
		_T    		pop_back();
		void    	resize(size_t _len);

		/**
		 * Deletes list and its elements.
		 */
		~zs_vector();
	private:
		//private
		size_t		_size; // size vector (user count for iterate through items)
		bool		push_back_slot();

		void copy(const zs_vector<_T> & _vector);


	};



}
