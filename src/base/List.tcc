/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	template<typename _N>
	List<_N>::List(){
		this->first=NULL;
		this->last=NULL;
	}

	template<typename _N>
	void List<_N>::insert( ListNode<_N> * _node){

		if(_node->next != NULL || _node->previous != NULL){
			ZS_THROW_RUNTIME_ERRORF("Internal error: An already attached node");
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
	}


	template<typename _N>
	void List<_N>::remove(ListNode<_N> * _node){

		if(_node->next == NULL || _node->previous == NULL){
			ZS_THROW_RUNTIME_ERRORF("Internal error: An already deattached node");
		}

		if((_node->previous == _node) && (_node->next == _node)){ // 1 single node...
			this->last=this->first=NULL;
		}
		else{ // dettach and attach next...
			// [1]<->[2]<-> ...[P]<->[C]<->[N]...[M-1]<->[M]
			if(_node == this->first){
				this->first=_node->next;
			}
			if(_node == this->last){
				this->last=_node->previous;
			}
			_node->previous->next=_node->next;
			_node->next->previous=_node->previous;

		}
		_node->previous = _node->next = NULL;
	}


	template<typename _N>
	void List<_N>::dettachAllNodes(void (*_dettachNode)(ListNode<_N> *)){

		if(this->first != NULL){
			ListNode<_N> * next_node=NULL;
			ListNode<_N> * current_node=this->first;

			do{
				// save next node pointer because on dettach list nodes can be affected
				next_node=current_node->next;

				if(_dettachNode != NULL){
					//fuser-function dettach node (list nodes will be affected)
					_dettachNode(current_node);
				}
				current_node=next_node;
			// when last node is deallocated, first will be NULL
			}while(this->first!=NULL);
		}
	}

	template<typename _N>
	List<_N>::~List(){
		dettachAllNodes();
	}
}

