/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class ScriptObject;
	class RefObject{
	public:
		RefObject(ScriptObject *_ref_object, ScriptObject *_ref_from_object);
		void deRefObject();
		ScriptObject *getRefObject();
		~RefObject();

	private:
		ScriptObject *ref_object;
		ScriptObject *ref_from_object;
	};

}
