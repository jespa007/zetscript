/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

namespace zetscript{
	RefObjectScriptObject::RefObjectScriptObject(
			ZetScript *_zs
			, ScriptObject *_so_ref
	):ScriptObject(_zs){
		ref_object=_so_ref;
		ref_object_node=new zs_list_node<RefObjectScriptObject *>;
		ref_object_node->data=this;

		ref_object->attachRefObjectNode(ref_object_node);
	}


	void RefObjectScriptObject::setRefObject(ScriptObject *_ref_object){

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

	ScriptObject *RefObjectScriptObject::getRefObject(){
		return ref_object;
	}

	void RefObjectScriptObject::dettachRefObject(){
		// if ref_object != is not already deattached
		if(ref_object != NULL){
			ref_object->deattachRefObjectNode(ref_object_node);
			ref_object_node->data->ref_object=NULL;
		}
	}

	RefObjectScriptObject::~RefObjectScriptObject(){
		dettachRefObject();
		delete ref_object_node;
		ref_object_node=NULL;
	}
}
