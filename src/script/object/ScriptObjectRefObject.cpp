/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

namespace zetscript{
	ScriptObjectRefObject::ScriptObjectRefObject(){
		ref_object_node=NULL;
		ref_object=NULL;
		ref_object_node=new zs_list_node<ScriptObjectRefObject *>;
	}


	void ScriptObjectRefObject::setRefObject(ScriptObject *_ref_object){

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

	ScriptObject *ScriptObjectRefObject::getRefObject(){
		return ref_object;
	}

	void ScriptObjectRefObject::dettachRefObject(){
		if(ref_object_node != NULL){
			ref_object->deattachRefObjectNode(ref_object_node);
			ref_object_node->data=NULL;
		}
	}

	ScriptObjectRefObject::~ScriptObjectRefObject(){
		dettachRefObject();
		delete ref_object_node;
		ref_object_node=NULL;
	}
}
