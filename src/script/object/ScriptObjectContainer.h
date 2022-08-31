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

	class  ScriptObjectContainer:public ScriptObject{
	public:

		ScriptObjectContainer();
		ScriptObjectContainerSlot 				*getScriptObjectContainerSlotRoot();
		~ScriptObjectContainer();

	protected:
		virtual void init(ZetScript *_zs);
	private:

		// refs where this container is referenced
		zs_list<ScriptObjectContainerSlot *>	so_container_slots;

		// root of herarchy nodes
		ScriptObjectContainerSlot 				*root;
	};

}
