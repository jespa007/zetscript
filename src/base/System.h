/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	namespace System{
		zs_float clock();
		const char * getIoErrorCodeDetails(int _error_code);
		const char * getErrorCodeDetails(int _error_code);
		int getLastError();

	}
}
