/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	template<typename _N>
	zs_list<_N>::zs_list(){
		this->first=NULL;
		this->last=NULL;
	}

	template<typename _N>
	bool zs_list<_N>::insert( zs_list_node<_N> * _node){

		if(_node->next != NULL || _node->previous != NULL){
			THROW_RUNTIME_ERRORF("Internal error: An already attached node");
			return false;
		}

		if(this->first == NULL){ /*one  node: trivial ?*/
			_node->previous=_node->next= this->last = this->first =_node;
		}
		else{ /* >1 node add to the end */
			// attach last-previous
			_node->previous=this->last;
			this->last->next=_node;
			this->last=_node;

			// attach next
			_node->next=this->first;
			this->first->previous=_node;
		}
		return true;
	}


	template<typename _N>
	bool zs_list<_N>::remove(zs_list_node<_N> * _node){

		if(_node->next == NULL || _node->previous == NULL){
			THROW_RUNTIME_ERRORF("Internal error: An already deattached node");
			return false;
		}

		if((_node->previous == _node) && (_node->next == _node)){ // 1 single node...
			this->last=this->first=NULL;
		}
		else{ // dettach and attach next...
			// [1]<->[2]<-> ...[P]<->[C]<->[N]...[M-1]<->[M]
			if(_node == this->first){
				this->first=_node->next;
			}
			else if(_node == this->last){
				this->last=_node->previous;
			}
			_node->previous->next=_node->next;
			_node->next->previous=_node->previous;

		}
		_node->previous = _node->next = NULL;
	}


	template<typename _N>
	void zs_list<_N>::remove_all(){

		zs_list_node<_N> *next_node=NULL,*current=this->first;

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
	zs_list<_N>::~zs_list(){
		remove_all();
	}
}

