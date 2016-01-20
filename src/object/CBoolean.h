#pragma once

class CBoolean: public CObject{
	
	bool m_value;
public:

	static CBoolean * Parse(const string & );

	CBoolean();
	CBoolean(bool b);

	virtual string toString();
	
};
