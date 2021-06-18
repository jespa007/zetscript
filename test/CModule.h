#ifndef __CMODULE_H__
#define __CMODULE_H__

typedef struct CModule CModule;

struct CModule{
	void *data;
};


CModule *CModule_New(void);
void     CModule_Delete(CModule * _this);


#endif
