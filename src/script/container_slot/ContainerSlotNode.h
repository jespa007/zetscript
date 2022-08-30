/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{


	class ContainerSlotNode{
	public:
		ContainerSlotData  data;
		ContainerSlotNode *parent;
		zs_list<ContainerSlotNode *> childs;

		ContainerSlotNode(const ContainerSlotData & _container_slot);

		void addChild(zs_list_node<ContainerSlotNode *> *_node);
	};
}
