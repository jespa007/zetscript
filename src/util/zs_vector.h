/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	template<typename _T>
	class zs_vector{
	public:

		// static
		static int npos;

		//public
		zs_vector();
		zs_vector(const zs_vector & _vector);
		zs_vector& operator=(const zs_vector& _vector);

		void 		set( int  _pos, const _T & _element);
		inline const _T &	get( int  _pos) const;
		void 		erase( int  _pos);
		/**
		 * Insert item at the end
		 */
		bool 		push_back( const _T & _element);
		void 		concat(const zs_vector<_T>  & _vector);
		void 		insert(int _pos,const zs_vector<_T>  & _vector, int _n_list_elements_to_copy=npos);
		/**
		 * Insert item at position idx.
		 */
		void 		insert(int  _pos, const _T & _element);

		void 		clear();
		_T    		pop_back();
		void    	resize(int _len);

		inline _T *data() const{
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
		_T* 		items;
		int 		count; //number of items
		int			_size; // size vector (user count for iterate through items)

		bool		push_back_slot();

		void copy(const zs_vector<_T> & _vector);


	};
}

#include "zs_vector.tcc"
