#include "zetscript.h"

namespace zetscript{

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
		this->list = (zs_map_node **)ZS_MALLOC(sizeof(zs_map_node*)*this->count);

	}

	zs_map_iterator zs_map::begin(){
		return zs_map_iterator(this);
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

	void zs_map::set(const char * key,zs_int val){

		zs_map_node *node = NULL;

		if((node=lookup_node(key))==NULL){
			uint8_t pos = hash_code(key);
			zs_map_node *first_node = this->list[pos];
			node = (zs_map_node *)ZS_MALLOC(sizeof(zs_map_node));

			// insert node at the end (not ordered)
			size_t key_len=strlen(key)+1;
			node->key = (char *)ZS_MALLOC(key_len);

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

	zs_int 	zs_map::get(const char * key, bool * exists){
		zs_map_node * node=lookup_node(key);

		if(exists!=NULL){
			*exists=false;
		}

		if(node != NULL){
			if(exists){
				*exists=true;
			}
			return node->val;
		}

		return 0;
	}

	void 		zs_map::erase(const char * key){
		uint8_t pos = hash_code(key);
		zs_map_node * node=lookup_node(key);

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

		free(node->key);
		free(node);

	}

	bool		zs_map::exist(const char * key){
		return lookup_node(key) != NULL;
	}

	void zs_map::clear(){
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
		memset(this->list,0,sizeof(zs_map_node*)*this->count);
	}

	zs_map::~zs_map(){

		clear();

		free(this->list);
	}
}
