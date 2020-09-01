#pragma once

namespace zetscript{

	class zs_vector{
	public:
		//public
		intptr_t* items;
		uint16_t count; //number of items

		//ZPList  	*	ZPList_New(void);
		zs_vector();

		void 		set( uint16_t  idx, intptr_t e);
		intptr_t	get( uint16_t  idx);
		void 		erase( uint16_t  idx);
		/**
		 * Insert item at the end
		 */
		bool 		push_back( intptr_t e);
		void 		concat( zs_vector *list);
		/**
		 * Insert item at position idx.
		 */
		void 		insert(uint16_t  idx, intptr_t e);

		void 		clear();
		intptr_t    pop_back();

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
