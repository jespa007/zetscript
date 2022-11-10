/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define ZS_ONE_OVER_MS_PER_SECONDS	(1.0/1000.0)

namespace zetscript{
	namespace zs_system{
		static zs_int __begin_ms__=0;

		zs_float clock(){
			zs_int current_ms=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			zs_int elapsed_ms_from_begin=0;

			if(__begin_ms__==0){
				__begin_ms__=current_ms;
			}else{
				elapsed_ms_from_begin=current_ms-__begin_ms__;
			}

			return elapsed_ms_from_begin*ZS_ONE_OVER_MS_PER_SECONDS;
		}
	}
}
