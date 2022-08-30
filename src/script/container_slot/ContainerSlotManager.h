/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class ContainerSlotManager{
	public:
		zs_list_node<ContainerSlotNode *> *newNode();
	private:
		zs_list<ContainerSlotNode *> list;
	};

}
