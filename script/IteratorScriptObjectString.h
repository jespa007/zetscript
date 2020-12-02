/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{


	class  IteratorScriptObjectString:public ScriptObjectBase{

	public:

		IteratorScriptObjectString(ScriptObjectString *sov);

		static StackElement 	_getSf(IteratorScriptObjectString *si);
		static void				_nextSf(IteratorScriptObjectString *si);
		static bool				endSf();

	protected:

		StackElement _get();
		void _inc();
		bool end();

	};

}
