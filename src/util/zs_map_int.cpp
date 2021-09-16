#include "zetscript.h"

namespace zetscript{

	uint8_t zs_map_int::hash_code(zs_int key){
		return key % HASH_MAP_SIZE;
	}

	zs_map_int::zs_map_int(){
		this->count=HASH_MAP_SIZE;
		this->list = (zs_map_node_int **)malloc(sizeof(zs_map_node_int*)*this->count);
		memset(this->list,0,sizeof(zs_map_node_int*)*this->count);

	}


	zs_map_node_int * zs_map_int::lookup_node(zs_int key){
		uint8_t pos = hash_code(key);
		zs_map_node_int *list = this->list[pos];
		zs_map_node_int *temp = list;

		while(temp){
			if(temp->key==key){
				return temp;
			}
			temp = temp->next;
		}
		return NULL;
	}

	void zs_map_int::set(zs_int key,zs_int val){

		zs_map_node_int *node = NULL;

		if((node=lookup_node(key))==NULL){
			uint8_t pos = hash_code(key);
			zs_map_node_int *first_node = this->list[pos];
			node = (zs_map_node_int *)malloc(sizeof(zs_map_node_int));

			// insert node at the end (not ordered)
			node->key=key;
			node->val = val;
			node->next = first_node;
			node->previous=NULL;

			if(first_node != NULL){
				first_node->previous=node;
			}

			this->list[pos] = node; //Replace first node by _this one...
		}
		node->val = val;
	}

	zs_int 	zs_map_int::get(zs_int key, bool * exists){
		zs_map_node_int * node=lookup_node(key);
		if(exists != NULL){
			*exists=false;
		}

		if(node != NULL){
			if(exists != NULL){
				*exists=true;
			}
			return node->val;
		}

		return 0;
	}

	void 		zs_map_int::erase(zs_int key){
		uint8_t pos = hash_code(key);
		zs_map_node_int * node=lookup_node(key);

		if(node == NULL){
			THROW_RUNTIME_ERROR("Element %s not exist",key);
			return;
		}

		// not first...
		if(node->previous != NULL){
			node->previous->next=node->next; // link previous
		}else{ // first, set first element as next
			this->list[pos]=node->next;
		}

		// not last
		if(node->next!=NULL){
			node->next->previous=node->previous; // link previous-next
		}

		free(node);

	}

	bool		zs_map_int::exist(zs_int key){
		return lookup_node(key) != NULL;
	}

	void zs_map_int::clear(){
		for(unsigned i=0;i<this->count;i++){
			zs_map_node_int * temp=this->list[i];

			while(temp){
				zs_map_node_int * to_deallocate = temp;
				temp=temp->next;
				free(to_deallocate);

			}
		}
		memset(this->list,0,sizeof(zs_map_node_int*)*this->count);
	}

	zs_map_int::~zs_map_int(){

		clear();

		free(this->list);
	}
}
