#pragma once

namespace zetscript{

	class zs_vector_fast{
	public:
		//public
		void* items;
		size_t count; //number of items

		//ZPList  	*	ZPList_New(void);
		zs_vector_fast(size_t _size_item);

		void 		set( uint16_t  idx, void * e);
		zs_int	get( uint16_t  idx);
		void 		erase( void *  idx);
		/**
		 * Insert item at the end
		 */
		bool 		push_back( void * e);
		void 		concat( zs_vector_fast *list);
		/**
		 * Insert item at position idx.
		 */
		void 		insert(uint16_t  idx, void * e);

		void 		clear();
		void    pop_back();

		//void		free_all_items_and_clear();
		/**
		 * Deletes list and its elements.
		 */
		~zs_vector_fast();
	private:
		//private
		size_t size; // size vector (user count for iterate through items)
		size_t  size_item;
		bool	push_back_slot();

	};



}
