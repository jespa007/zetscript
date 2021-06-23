/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class  ScriptObjectDateTime: public ScriptObject{
	public:
		zs_datetime datetime;

		static ScriptObjectDateTime * newScriptObjectDateTime(ZetScript *_zs);

		ScriptObjectDateTime();

		virtual std::string toString();

	protected:




	};

}
