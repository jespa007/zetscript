#pragma once

class CVector: public CScriptVariable{

public:

	vector<CVariable *> m_value;

	CVector();

	~CVector();
};
