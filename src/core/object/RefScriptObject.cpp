/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

namespace zetscript{
	RefScriptObject::RefScriptObject(
			ScriptEngine *_se
			, ScriptObject *_so_ref
	):ScriptObject(_se){
		ref_object=_so_ref;
		ref_object_node=new ListNode<RefScriptObject *>;
		ref_object_node->data=this;

		ref_object->attachRefObjectNode(ref_object_node);
	}


	void RefScriptObject::setRefObject(ScriptObject *_ref_object){

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

	ScriptObject *RefScriptObject::getRefObject(){
		return ref_object;
	}

	void RefScriptObject::dettachRefObject(){
		// if ref_object != is not already deattached
		if(ref_object != NULL){
			ref_object->deattachRefObjectNode(ref_object_node);
			ref_object_node->data->ref_object=NULL;
		}
	}

	RefScriptObject::~RefScriptObject(){
		dettachRefObject();
		delete ref_object_node;
		ref_object_node=NULL;
	}
}
