/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{


	class  ScriptObjectStringIterator:public ScriptObjectBase{

	public:

		ScriptObjectStringIterator(ScriptObjectString *sov);

		static StackElement 	_getSf(ScriptObjectStringIterator *si);
		static void				_nextSf(ScriptObjectStringIterator *si);
		static bool				endSf();

	protected:

		StackElement _get();
		void _inc();
		bool end();

	};

}
