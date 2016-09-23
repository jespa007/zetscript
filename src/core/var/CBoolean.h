#pragma once

class CBoolean: public CScriptVariable{
	

public:

	bool m_value;


	static CBoolean * Parse(const string & );
	static bool * ParsePrimitive(const string & );

	CBoolean();
	CBoolean(bool b);

	

};
