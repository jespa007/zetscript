#pragma once

namespace zetscript{

	class MemberAttribute{
	public:
		ScriptClass *script_class;
		zs_vector setters; // setter that contains a list of script functions C++
		 ScriptFunction *getter,*post_inc,*post_dec,*pre_inc,*pre_dec; // getter
		 zs_string attribute_name;

		 MemberAttribute(ScriptClass *_script_class, const zs_string & _attribute_name);

		 void addSetter(ScriptFunction *f);

		~MemberAttribute();
	};
}
