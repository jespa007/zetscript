#pragma once

#define ZS_VECTOR_N_SLOT_ELEMENTS 10

namespace zetscript{

	class zs_vector{
	public:
		//public
		zs_int* items;
		uint16_t count; //number of items

		//ZPList  	*	ZPList_New(void);
		zs_vector();

		void 		set( uint16_t  idx, zs_int e);
		zs_int		get( uint16_t  idx);
		void 		erase( uint16_t  idx);
		/**
		 * Insert item at the end
		 */
		bool 		push_back( zs_int e);
		void 		concat(zs_vector  * list);
		/**
		 * Insert item at position idx.
		 */
		void 		insert(uint16_t  idx, zs_int e);

		void 		clear();
		zs_int    	pop_back();
		void    	resize(size_t n);

		//void		free_all_items_and_clear();
		/**
		 * Deletes list and its elements.
		 */
		~zs_vector();
	private:
		//private
		unsigned int _size; // size vector (user count for iterate through items)
		bool	push_back_slot();

	};



}
