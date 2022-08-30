/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	ContainerSlotNode::ContainerSlotNode(const ContainerSlotData & _data){
		data = _data;
		parent=NULL;
	}

	void ContainerSlotNode::addChild(zs_list_node<ContainerSlotNode *> * _node){
		if(_node->data->parent != NULL){
			THROW_RUNTIME_ERRORF("parent node not NULL");
		}

		childs.insert(_node);
	}
}
