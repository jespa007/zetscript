#pragma once

class CBoolean: public CObject{
	

public:
	bool m_value;
	static CBoolean * Parse(const string & );

	CBoolean();
	CBoolean(bool b);

	virtual string toString();
	
};
