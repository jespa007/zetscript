#pragma once


//#include <assert.h>

//#include "../video/OGL_utils.h"
//#include "utils.h"

/*
#define GET_ARRAY_PTR(x) ((x.type) === (CArray::TYPE_INTEGER))?(int *)(x->array):\
						  ((x.type) === (CArray::TYPE_FLOAT))?(float *)(x->array):\
						  throwException(__FILE__, __LINE__,x)*/
/*
#define GET_SIZE_ELEMENT_PTR(x) ((type) == (CArray::TYPE_INTEGER))?(sizeof(int)):\
								 ((type) == (CArray::TYPE_FLOAT))?(sizeof(float)):\
								 0
*/
#define NEW_FLOAT(length)	(new CInfoArray(CDinamicArray::TYPE_FLOAT, length))
#define NEW_INT(length)	(new CInfoArray(CDinamicArray::TYPE_FLOAT, length))



template<typename _T>
class CBuffer
{
public:

	enum{
		TYPE_UNKNOWW=0,
		TYPE_INTEGER,
		TYPE_FLOAT

	};

	_T *data_buffer;

	unsigned int length;

	/*CBuffer()
	{
		allocated_buffer = data_buffer = NULL;
		length = 0;
	}*/
	CBuffer(const CBuffer<_T> & v_data)
	{
		data_buffer = NULL;
		length = v_data.length;

		data_buffer = new _T[length];

		for(unsigned k = 0; k < length; k++)
		{
			data_buffer[k] = v_data[k];
		}

	}

	CBuffer(_T * v_data, int lenght)
	{
		data_buffer = NULL;
		this->length = lenght;

		data_buffer = new _T[length];

		for(unsigned k = 0; k < length; k++)
		{
			data_buffer[k] = v_data[k];
		}

	}

	CBuffer(const std::vector<_T> & v_data)
	{
		data_buffer = NULL;
		length = v_data.size();

		data_buffer = new _T[length];

		for(unsigned k = 0; k < length; k++)
		{
			data_buffer[k] = v_data.at(k);
		}

	}

	explicit CBuffer( const int  _length)
	{
		data_buffer = NULL;
		length = _length;

		data_buffer = new _T[length];


	}

	/*void set(_T *_array_buffer, int _length)
	{
		if(data_buffer != NULL)
		{
			data_buffer = _array_buffer;
			length = _length;
		}
	}*/

	_T & operator[](const int index)
	{
		if(index < 0 || index >= (int)this->length)
		{
			throw("Index out of bounds");
			return data_buffer[0];
		}

		return data_buffer[index];
	}

	const _T & operator[](const int index) const
	{

		if(index < 0 || index >= (int)this->length)
		{
			throw("Index out of bounds");
			return data_buffer[0];
		}


		return data_buffer[index];
	}



	~CBuffer()
	{
		if(data_buffer != NULL)
		{
			delete [] data_buffer;
			length = 0;
			data_buffer = NULL;
		}
	}
};

typedef CBuffer<short> 	ShortBuffer;
typedef CBuffer<float> 	FloatBuffer;
typedef CBuffer<int> 		IntBuffer;
typedef CBuffer<char> 		CharBuffer;
typedef CBuffer<unsigned char> 		ByteBuffer;

typedef CBuffer<double> 	DoubleBuffer;


class CInterpolator1d;
class CInterpolator2D;
class CInterpolator3d;
typedef CBuffer<CInterpolator1d *> 	PInterpolator1d_Buffer;
typedef CBuffer<CInterpolator2D *> 	PInterpolator2D_Buffer;
typedef CBuffer<CInterpolator3d *> 	PInterpolator3D_Buffer;



/*

void throwException(const char *_file, int _line, CArray * c)
{
	print_error_cr("Error passing array invalid type %i at (%s:%i)", c->type, _file, _line);
	exit(EXIT_FAILURE);
}*/

