/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	template<typename _T>
	class Vector{
	public:

		// static
		static int npos;

		//public
		Vector();
		Vector(const Vector & _vector);
		Vector& operator=(const Vector& _vector);

		void 		set( int  _pos, const _T & _element);
		inline const _T &	get( int  _pos) const;
		void 		erase( int  _pos);
		/**
		 * Insert item at the end
		 */
		bool 		push( const _T & _element);
		void 		concat(const Vector<_T>  & _vector);
		void 		insert(int _pos,const Vector<_T>  & _vector, int _n_list_elements_to_copy=npos);
		/**
		 * Insert item at position idx.
		 */
		void 		insert(int  _pos, const _T & _element);

		void 		clear();
		void   		pop();
		void    	resize(int _len);

		inline _T *data() const{
			return items;
		}

		inline int	length() const{
			return count;
		}

		/**
		 * Deletes list and its elements.
		 */
		~Vector();
	private:
		//private
		_T* 		items;
		int 		count; //number of items
		int			_size; // size vector (user count for iterate through items)

		bool		pushSlot();

		void copy(const Vector<_T> & _vector);


	};
}

#include "Vector.tcc"
