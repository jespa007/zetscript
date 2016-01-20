#ifndef    __JGT_RAM_H__
#define    __JGT_RAM_H__

#define __HAVE_OGL__

#include          <stdlib.h>
#include          <memory.h>

#include          <stdio.h>
#include          <math.h>

#include          <vector>
#include 			<regex>
#include                  <new>
#include           <iostream>
#include <functional>
#include 		<sstream>
#include <string>
#include <cstring>
#include <list>
#include <utility>
#include <float.h>
#include <cstdarg>
#include <assert.h>
#include <dirent.h>
#include <typeinfo>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <string>
#include <map>
#include <vector>
#include <sstream>



#include <mutex>


#if defined(__GNUC__)
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory.h>
#include <fcntl.h>//Definiciondelibreriadecontroldeficheros

#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#else
#include <dlfcn.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/ioctl.h>
#include <termios.h>/*POSIXdefinicionesdeterninaldecontrol*/
#endif




#endif



extern "C"  {
	/* The ffmpeg library is completely written in C, so we need to tell the C++ compiler that so it links correctly. */

#include <dir.h>




}




using std::string;
using std::vector;
using std::map;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::ostringstream;



#if defined(__DEBUG__) && defined(__MEMMANAGER__)

#ifndef  __FUNCTION__
	#define	__FUNCTION__  "??"
#endif  

enum{
        UNKNOWN_ALLOCATE=0,
        MALLOC_ALLOCATOR,  //  by  default
        NEW_ALLOCATOR,  
        NEW_WITH_BRACETS_ALLOCATOR,  
        SDL_SURFACE_ALLOCATOR,
        SDL_SOCKET_ALLOCATOR,
        SDL_SOUND_ALLOCATOR,
        MAX_ALLOCATE_TYPES
};



typedef  void  (*  void_function_void_pointer)(const  char  *module,const  char  *function,  int  line,  
                                                                    void  *pointer);


#define ZG_MEM_DISABLE_REGISTER_MEMORYLEAKS	 	MEM_setEnableRegisterMemoryLeaks(0)
#define ZG_MEM_ENABLE_REGISTER_MEMORYLEAKS 		MEM_setEnableRegisterMemoryLeaks(1)

//namespace  __jespa_mem__  {
//------------------------------------------------------------------------------------------------------------

void        MEM_setEnableRegisterMemoryLeaks(int enable);
void        MEM_ViewStatus();
void        MEM_Init();
void        MEM_FreeAllAllocatedPointers( );
void        MEM_ListAllocatedPointers();
#define  MEM_New(type,n_items)              ((type  *)JGT_malloc(sizeof(type)*n_items,  __FILE__,  __LINE__))
//------------------------------------------------------------------------------------------------------------
void        JGT_AddReservedPointer(void  *pointer,  void_function_void_pointer deallocator, const  char  *filename,  const  char  *function,  int  line);
int          JGT_DeleteReservedPointer(void  *pointer, const  char  *filename,  const  char  *function,  int  line);
int 	    JGT_PreAllocatedPointerExist(void *pointer, const  char  *filename,  const  char  *function,  int  line);
void        *JGT_malloc(size_t  size,  const  char  *filename,  int  line);
void        *JGT_realloc(void *ptr, size_t  size,  const  char  *filename,  int  line);
void        JGT_addPointer(void  *pointer,  int  type_allocated_pointer,  const  char  *filename,  int  line);
void        JGT_PreCheckFree(void  *pointer,  const  char  *filename,  int  row  );
//bool        JGT_Free(void  *pointer,  const  char  *filename,  int  row  );
#ifdef free
#undef free
#endif

#ifdef malloc
#undef malloc
#endif

#ifdef realloc
#undef realloc
#endif


#define  free(p)                               JGT_PreCheckFree(p,__FILE__,  __LINE__)
#define  malloc(size)	                        JGT_malloc(size,  __FILE__,  __LINE__)
#define  realloc(ptr,size)	                        JGT_realloc(ptr,size,  __FILE__,  __LINE__)
//------------------------------------------------------------------------------------------------------------
//void*  operator  new        (size_t  size);
//void*  operator  delete  (size_t  size);


#ifdef  __cplusplus

bool	m_setOwner(const  char  *file,  const  unsigned  int  line,  const  char  *func);

#ifdef  __GNUC__
	void*  operator  new(size_t  size);
	void*  operator  new(size_t  size,    const  char  *file,  const  unsigned  int  line,  const  char  *func);
	
	void*  operator  new[](size_t  size);
	void*  operator  new[](size_t  size,const  char  *file,  const  unsigned  int  line,  const  char  *func);
	
	void    operator  delete(void  *p) throw();
	void    operator  delete(void  *p,const  char  *file,  const  unsigned  int  line,  const  char  *func);
	
	void    operator  delete[](void  *p) throw();
	void    operator  delete[](void  *p,const  char  *file,  const  unsigned  int  line,  const  char  *func);
/*#else

	void*  operator  new(std::size_t)  throw  (std::bad_alloc);
	void*  operator  new[](std::size_t)  throw  (std::bad_alloc);
	void    operator  delete(void*)  throw();
	void  operator  delete[](void*)  throw();*/

#endif  

#endif  

//---------------------------------  REDEFINE  MALLOC  FUNCTIONS  HERE  ------------------

#include                  "MEM_ram_Nommgr.h"



#define  malloc(p)                                      	JGT_malloc(p,__FILE__,  __LINE__)//MEM_Malloc
#define  realloc(p,s)                                      	JGT_realloc(p,s,__FILE__,  __LINE__)//MEM_Malloc
#define  free(p)                                         	JGT_PreCheckFree(p,__FILE__,  __LINE__)//MEM_Free
//#undef  new
//#undef  delete
#ifdef  __cplusplus

#define	new		                                        (m_setOwner(__FILE__,__LINE__,__FUNCTION__),false)  ?  NULL  :  new
//#define	delete		                                (m_setOwner(__FILE__,__LINE__,__FUNCTION__),false)  ?  NULL  :  delete
//#ifndef  __GNUC__
#define	delete		                                	(m_setOwner(__FILE__,__LINE__,__FUNCTION__) , false)? throw : delete
//#endif

#endif
//------------------------------------------------------------------------------------------------------------
//}
//}
#define  CREATE_ARRAY1D(ptr,  type,  num_elements)\
        ptr  =  new  type[num_elements];


#define  DEL_ARRAY1D(ptr)  if(ptr!=NULL)delete  []  ptr

//------------------------------------------------------------------------------------------------

#define  CREATE_ARRAY2D(ptr2D,  type,  num_rows,  num_columns)\
{\
        type  *_aux  =  new  type[(num_rows)*(num_columns)];\
        ptr2D  =  new  type  *[num_rows];\
        for(int  i  =  0;  i  <  (num_rows);i++)  ptr2D[i]=&_aux[i*(num_columns)];\
}

//#define  SIZEOF_ARRAY(ptr)  ptr[num_rows]

#define  DEL_ARRAY2D(ptr)\
        if(ptr!=NULL){delete  []  ptr[0];\
        delete  []  ptr;}
//#endif  
//------------------------------------------------------------------------------------------------
#define  CREATE_ARRAY3D(ptr3D,  type,  num_z, num_rows,  num_columns)\
{\
        type  *_aux  =  new  type[(num_z)*(num_rows)*(num_columns)];\
        ptr3D  =  new  type  **[(num_z)];\
		for(int  z  =  0;  z  <  (num_z);z++)\
		{\
		type **ptr2D  =  new  type  *[(num_rows)];\
		for(int  r  =  0;  r  <  (num_rows);r++) \
		{\
			ptr2D[r]=&_aux[r*(num_columns)+(num_rows)*(num_columns)*z];\
		}\
		ptr3D[z]=ptr2D;\
		}\
}

#define  DEL_ARRAY3D(ptr)\
        if(ptr!=NULL){delete  []  ptr[0];\
        delete  []  ptr;}
//#endif  
//------------------------------------------------------------------------------------------------

#endif
#endif

#if !(defined(__DEBUG__) && defined(__MEMMANAGER__))

#define ZG_MEM_DISABLE_REGISTER_MEMORYLEAKS
#define ZG_MEM_ENABLE_REGISTER_MEMORYLEAKS

#endif  
