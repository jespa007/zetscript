#pragma once

class CString: public CObject{
	
public:

	string m_value;

	CString();
	CString(const string & s);
	virtual string toString();
	
};
