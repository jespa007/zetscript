/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once



namespace zetscript{

	class zs_vector{
	public:

		static int npos;
		//public
		zs_int* items;
		int count; //number of items

		//ZPList  	*	ZPList_New(void);
		zs_vector();
		zs_vector(const zs_vector& _obj);
		zs_vector& operator=(const zs_vector& _obj);

		void 		set( int  idx, zs_int e);
		zs_int		get( int  idx);
		void 		erase( int  idx);
		/**
		 * Insert item at the end
		 */
		bool 		push_back( zs_int e);
		void 		concat(zs_vector  * list);
		void 		insert(int  idx,zs_vector  * list, int n_list_elements_to_copy=npos);
		/**
		 * Insert item at position idx.
		 */
		void 		insert(int  idx, zs_int e);

		void 		clear();
		zs_int    	pop_back();
		void    	resize(int n);

		//void		free_all_items_and_clear();
		/**
		 * Deletes list and its elements.
		 */
		~zs_vector();
	private:
		//private
		int			_size; // size vector (user count for iterate through items)
		bool		push_back_slot();

		void copy(const zs_vector& _vector);
		void __cleanup__();


	};



}
