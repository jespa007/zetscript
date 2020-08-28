#include "zetscript.h"

#define HASH_MAP_SIZE 256

namespace zetscript{


	/*struct table{
		int size;
		struct node *list[HASH_MAP_SIZE];
	};*/

	uint8_t zs_map::hash_code(const char * key){
		size_t key_len=strlen(key);
		uint8_t p=0;

		for(unsigned i=0; i < key_len; i++){
			p+=key[i];
		}

		return p;
	}

	zs_map::zs_map(){
		this->count=HASH_MAP_SIZE;
		this->list = (zs_map_node **)malloc(sizeof(zs_map_node*)*this->count);


		for(unsigned i=0;i<this->count;i++){
			this->list[i]=NULL;
		}
	}

	zs_map_node * zs_map::lookup_node(const char * key){
		uint8_t pos = hash_code(key);
		zs_map_node *list = this->list[pos];
		zs_map_node *temp = list;

		while(temp){
			if(strcmp(temp->key,key)==0){
				return temp;
			}
			temp = temp->next;
		}
		return NULL;
	}

	void zs_map::set(const char * key,intptr_t val){

		zs_map_node *node = NULL;

		if((node=lookup_node(key))==NULL){
			uint8_t pos = hash_code(key);
			zs_map_node *first_node = this->list[pos];
			node = (zs_map_node *)malloc(sizeof(zs_map_node));

			// insert node at the end (not ordered)
			size_t key_len=strlen(key)+1;
			node->key = (char *)malloc(key_len);
			memset(node->key,0,key_len);
			strcpy(node->key,key);
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

	intptr_t 	zs_map::get(const char * key){
		zs_map_node * node=lookup_node(key);

		if(node != NULL){
			return node->val;
		}

		THROW_RUNTIME_ERROR("Element %s not exist",key);
		return 0;
	}

	void 		zs_map::erase(const char * key){
		zs_map_node * node=lookup_node(key);

		if(node != NULL){
			THROW_RUNTIME_ERROR("Element %s not exist",key);
			return;
		}

		// not first...
		if(node->previous != NULL){
			node->previous->next=node->next; // link previous
		}

		// not last
		if(node->next!=NULL){
			node->next->previous=node->previous; // link next
		}

		free(node);

	}

	bool		zs_map::exist(const char * key){
		return lookup_node(key) != NULL;
	}

	zs_map::~zs_map(){

		for(unsigned i=0;i<this->count;i++){
			zs_map_node * temp=this->list[i];

			while(temp){
				zs_map_node * to_deallocate = temp;
				temp=temp->next;

				/*if(t->on_delete != NULL){
					t->on_delete(to_deallocate);
				}*/

				free(to_deallocate->key);
				free(to_deallocate);

			}
		}
		free(this->list);
	}

}
