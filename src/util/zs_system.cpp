/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{
	namespace zs_system{
		static zs_float __clock_last_ms__=0;

		zs_float clock(){
			zs_float current_ms=(1.0/1000000.0f)*std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			zs_float diff=0;
			if(__clock_last_ms__!=0){
				diff=current_ms-__clock_last_ms__;
			}
			__clock_last_ms__=current_ms;

			return diff;
		}
	}
}
