#pragma once

class CFunctor:public CScriptVariable{

public:
	tInfoRegisteredFunctionSymbol *m_value;
	CFunctor(tInfoRegisteredFunctionSymbol *_irfs);

};
