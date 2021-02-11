/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define ZS_NEW_OBJECT_VAR_REF(zs) ScriptObjectVarRef::newScriptObjectVarRef(zs)

namespace zetscript{

	class  ScriptObjectVarRef: public ScriptObject{

	public:

		static ScriptObjectVarRef * newScriptObjectVarRef(ZetScript *zs);
		ScriptObjectVarRef();


	private:
		void setup();
	};

}
