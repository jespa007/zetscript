/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	template<typename _T>
	class zs_vector{
	public:

		static int npos;
		//public
		_T* items;
		int count; //number of items

		zs_vector();
		zs_vector(const zs_vector & _obj);
		zs_vector& operator=(const zs_vector& _obj);

		void 		set( int  _idx, const _T & _val);
		const _T &	get( int  _idx);
		void 		erase( int  _idx);
		/**
		 * Insert item at the end
		 */
		bool 		push_back( const _T & _val);
		void 		concat(const zs_vector<_T>  & _vector);
		void 		insert(int _idx,const zs_vector<_T>  & _vector, int _n_list_elements_to_copy=npos);
		/**
		 * Insert item at position idx.
		 */
		void 		insert(int  idx, const _T & _val);

		void 		clear();
		_T    		pop_back();
		void    	resize(int _len);

		inline _T *data(){
			return items;
		}

		inline int	size() const{
			return count;
		}

		/**
		 * Deletes list and its elements.
		 */
		~zs_vector();
	private:
		//private
		int			_size; // size vector (user count for iterate through items)
		bool		push_back_slot();

		void copy(const zs_vector<_T> & _vector);


	};
}

#include "zs_vector.tcc"
