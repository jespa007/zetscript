#pragma once

class CVector: public CVariable{

public:

	vector<CVariable *> m_value;

	CVector();

	~CVector();
};
