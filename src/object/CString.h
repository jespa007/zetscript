#pragma once

class CString: public CVariable<string>{
	
public:


	CString();
	CString(const string & s);
	virtual string toString();
	
};
