/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once



namespace zetscript{


	struct MapStringNode{
		char  *key;
		zs_int value;
		MapStringNode *next;
		MapStringNode *previous;
	};


	class MapString{
	public:
		MapString();
		bool		exist(const char * key);
		MapStringIterator begin();
		MapStringNode	*set(const char * key,zs_int val);
		zs_int 		get(const char * key, bool * exists=NULL);
		const char *getKey(const char * _key);
		void  		erase(const char * key);
		void 		clear();
		MapStringNode *data();
		int 		count();
		~MapString();

	private:
		struct MapStringItem{
			char  		*key;
			MapStringNode *node;
		};

		MapStringItem  		*items; // an ordered indexed of nodes, it can iterate through it
		MapStringNode 		*first;
		MapStringNode 		*last;

		void		pushSlot();
		int			search(const char * key);
		int			newSlot(const char *_key);
		 int _size; // size vector (user count for iterate through items)
		int _count;

	};

}

