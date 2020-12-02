/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{


	class  IteratorScriptObject{

	public:

		IteratorScriptObject(ScriptObjectVector *sov);

		static StackElement 	_getSf(IteratorScriptObject *si);
		static void				_nextSf(IteratorScriptObject *si);
		static bool				endSf();

	protected:

		StackElement get();
		void next();
		bool end();

	};

}
