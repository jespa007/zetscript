/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class  ScriptObjectVarRef: public ScriptObject{

	public:

		static ScriptObjectVarRef * newVarRefObject(ZetScript *zs);
		ScriptObjectVarRef();


	private:
		void setup();
	};

}
