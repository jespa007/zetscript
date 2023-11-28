/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	class System{
	public:
		static zs_float clock();
		static const char * getIoErrorCodeDetails(int _error_code);
		static const char * getErrorCodeDetails(int _error_code);
		static int getLastError();

	};
}
