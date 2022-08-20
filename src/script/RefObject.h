/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class ScriptObject;
	class RefObject{
	public:
		RefObject(ScriptObject *_target_obj, ScriptObject *_link_obj);
		void deRefObject();
		ScriptObject *getTargetObject();
		~RefObject();

	private:
		ScriptObject *target_obj;
		ScriptObject *link_obj;
	};

}
