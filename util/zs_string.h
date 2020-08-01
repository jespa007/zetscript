#pragma once

namespace zetgine{
	class zs_string{
	public:
		std::string *to_string();
	private:
		void * str; // it can be std::string or std::wstring (zetgine uses std::string by default
	};
}
