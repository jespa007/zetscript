/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

namespace zetscript{
	RefObjectObject::RefObjectObject(
			ZetScript *_zs
			, Object *_so_ref
	):Object(_zs){
		ref_object=_so_ref;
		ref_object_node=new ListNode<RefObjectObject *>;
		ref_object_node->data=this;

		ref_object->attachRefObjectNode(ref_object_node);
	}


	void RefObjectObject::setRefObject(Object *_ref_object){

		if(ref_object!=_ref_object){
			//detach current
			if(ref_object!=NULL){
				ref_object->deattachRefObjectNode(ref_object_node);
			}
		}

		if(_ref_object!=NULL){
			ref_object_node->data=this;
			ref_object->attachRefObjectNode(ref_object_node);
		}
		ref_object=_ref_object;
	}

	Object *RefObjectObject::getRefObject(){
		return ref_object;
	}

	void RefObjectObject::dettachRefObject(){
		// if ref_object != is not already deattached
		if(ref_object != NULL){
			ref_object->deattachRefObjectNode(ref_object_node);
			ref_object_node->data->ref_object=NULL;
		}
	}

	RefObjectObject::~RefObjectObject(){
		dettachRefObject();
		delete ref_object_node;
		ref_object_node=NULL;
	}
}
