/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	template<typename _N>
	zs_list::zs_list(){
		this->first=this->last=NULL;
	}

	template<typename _N>
	bool  insert( const _N & _data){
		zs_list_node<_N> *node=new zs_list_node<_N>;

		node->data=_data;

		if(this->first == NULL){ /*one  node: trivial ?*/
			node->previous=node->next= this->last = this->first =node;
		}
		else{ /* >1 node add to the end */
			// attach last-previous
			node->previous=list->last;
			this->last->next=node;
			this->last=node;

			// attach next
			node->next=list->first;
			this->first->previous=node;
		}
		return true;
	}


	template<typename _N>
	bool zs_list::remove(zs_list_node<_N> * _node){

		if(_node->next == NULL || _node->previous == NULL){
			THROW_EXCEPTION(vm," Internal error: An already deattached node");
			return false;
		}

		if((_node->previous == _node) && (_node->next == _node)){ // 1 single node...
			this->last=this->first=NULL;
		}
		else{ // dettach and attach next...
			// [1]<->[2]<-> ...[P]<->[C]<->[N]...[M-1]<->[M]
			if(_node == list->first){
				this->first=_node->next;
			}
			else if(_node == list->last){
				this->last=_node->previous;
			}
			_node->previous->next=_node->next;
			_node->next->previous=_node->previous;

		}
		_node->previous = _node->next = NULL;
	}


	template<typename _N>
	void zs_list::remove_all(){

		zs_list_node<_N> *next_node=NULL,*current=this->first;
		//bool check_empty_shared_pointers=false;

		if(current != NULL){
			bool finish=false;
			do{
				next_node=current->next;
				finish=next_node ==this->first;

				delete current;

				current=next_node;

			}while(!finish);
		}

		this->first=this->last=NULL;
	}


	template<typename _N>
	zs_list::~zs_list(){
		remove_all();
	}

}
