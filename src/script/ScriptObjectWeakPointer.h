/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_NEW_OBJECT_WEAK_POINTER(so) (ScriptObjectWeakPointer::newScriptObjectWeakPointer(so))

namespace zetscript{

	class  ScriptObjectWeakPointer:public ScriptObject{

	public:

		//----------------------------------------------
		//
		// Helpers
		//
		static ScriptObjectWeakPointer * newScriptObjectWeakPointer(ScriptObject *_so_reference);

		//
		// Helpers
		//
		//----------------------------------------------

		ScriptObjectWeakPointer();
		ScriptObjectWeakPointer(ScriptObject *_so);
		~ScriptObjectWeakPointer();
	protected:
		void setup();
	private:

		ScriptObject *so_reference;

	};

}
