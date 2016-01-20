#pragma once

class CNumber: public CObject{
	double m_value;
public:

	static CNumber * Parse(const string & );

	CNumber();
	CNumber(double v);

	virtual string toString();
	
};
