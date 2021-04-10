/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class  ScriptObjectDateTime: public ScriptObject{
	public:
		static ScriptObjectDateTime * nowSf(ZetScript *_zs);
		static ScriptObjectDateTime * newScriptObjectDateTime(ZetScript *_zs);


		ScriptObjectDateTime();
	};

}