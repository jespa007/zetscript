#pragma once

namespace zetscript{

	class zs_vector{
	public:
		//public
		intptr_t* items;
		unsigned int count; //number of items

		//ZPList  	*	ZPList_New(void);
		zs_vector();

		void 		set( unsigned int  idx, intptr_t e);
		intptr_t	get( unsigned int  idx);
		void 		erase( unsigned int  idx);
		/**
		 * Insert item at the end
		 */
		bool 		push_back( intptr_t e);
		void 		concat( zs_vector *list);
		/**
		 * Insert item at position idx.
		 */
		void 		insert(unsigned int  idx, intptr_t e);

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
