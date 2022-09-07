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

		if(_ref_object==NULL && ref_object != NULL){
			dettachRefObject();
			return;
		}


		if(ref_object!=NULL){
			THROW_RUNTIME_ERRORF("ref_object!=NULL && _ref_object!=NULL, it should dettach first");
		}

		// ref_object == NULL so attach
		ref_object=_ref_object;

		ref_object_node->data=this;
		ref_object->attachRefObjectNode(ref_object_node);
	}

	ScriptObject *RefObjectScriptObject::getRefObject(){
		return ref_object;
	}

	void RefObjectScriptObject::dettachRefObject(){
		if(ref_object != NULL){
			ref_object->deattachRefObjectNode(ref_object_node);
			ref_object_node->data=NULL;
		}
	}

	RefObjectScriptObject::~RefObjectScriptObject(){
		dettachRefObject();
		delete ref_object_node;
		ref_object_node=NULL;
	}
}
