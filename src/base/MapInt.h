/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once



namespace zetscript{


	struct MapIntNode{
		zs_int key;
		zs_int value;
		MapIntNode *next;
		MapIntNode *previous;
	};


	class MapInt{
	public:
		MapInt();
		bool		exist(zs_int key);
		MapIntIterator begin();
		void 		set(zs_int key,zs_int val);
		zs_int 		get(zs_int key, bool * exists=NULL);
		zs_int		getValueByIdx(zs_int _idx);
		void		eraseByIdx(zs_int _idx);
		void  		erase(zs_int key);
		void 		clear();
		MapIntNode *data();
		size_t 		count();
		~MapInt();

	private:
		struct MapIntItem{
			zs_int  		key;
			MapIntNode *node;
		};

		MapIntItem  		*items; // an ordered indexed of nodes, it can iterate through it
		MapIntNode 		*first;
		MapIntNode 		*last;

		void		pushSlot();
		int			search(zs_int key);
		int			newSlot(zs_int _key);
		unsigned int _size; // size vector (user count for iterate through items)
		uint16_t _count;

	};

}

