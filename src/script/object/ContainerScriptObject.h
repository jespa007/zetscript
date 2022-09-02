/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class ContainerSlotScriptObject;
	class  ContainerScriptObject:public ScriptObject{
	public:

		ContainerScriptObject();
		ContainerSlotScriptObject 				*getScriptObjectContainerSlotRoot();
		zs_list<ContainerSlotScriptObject *>	*getListContainerSlotsRef();
		~ContainerScriptObject();

	protected:
		virtual void init(ZetScript *_zs);
		void countReferences();
		void initContainer();
	private:

		// refs where this container is referenced
		zs_list<ContainerSlotScriptObject *>	so_container_slots;

		// root of herarchy nodes
		ContainerSlotScriptObject 				*root;
	};

}
