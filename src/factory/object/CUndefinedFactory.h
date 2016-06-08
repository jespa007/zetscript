#pragma once

class CUndefinedFactory: public CFactory<CUndefined> {
public:
	static void registerScriptFunctions();

};
