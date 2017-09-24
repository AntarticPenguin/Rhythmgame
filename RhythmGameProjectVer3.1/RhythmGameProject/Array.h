#pragma once
#include <stdio.h>

template<class T>
class Array
{
private:
	T* _array;
	int _size;

public:
	void Resize(int newsize);
	void Insert(T value, int index);
	void Remove(int index);
	bool SaveData(const char* filename);
	bool LoadData(const char* filename);
	int GetSize();
	void Set(int index, T newItem);
	T Get(int index);
public:
	Array(int size)
	{
		_array = new T[size];
		_size = size;
	}
	~Array()
	{
		if (NULL != _array)
			delete[] _array;

		_array = NULL;
		_size = 0;
	}
	T& operator[](int index);
	operator T*();
};

template<class T>
T& Array<T>::operator[](int index)
{
	return _array[index];
}

template<class T>
Array<T>::operator T*()
{
	return _array;
}

template<class T>
void Array<T>::Resize(int newsize)
{
	T* newArray = new T[newsize];
	if (NULL == newArray)
		return;

	int minsize = 0;

	if (_size < newsize)
		minsize = _size;
	else
		minsize = newsize;

	for (int i = 0; i < minsize; i++)
		newArray[i] = _array[i];


	if (NULL != _array)
		delete[] _array;

	_array = newArray;
	_size = newsize;
}

template <class T>
void Array<T>::Set(int index, T newItem)
{
	_array[index] = newItem;
}

template <class T>
T Array<T>::Get(int index)
{
	return _array[index];
}

template<class T>
void Array<T>::Insert(T value, int index)
{
	for (int i = _size - 1; i > index; i--)
	{
		_array[i] = _array[i - 1];
	}
	_array[index] = value;
}

template<class T>
void Array<T>::Remove(int index)
{
	for (int i = index; i < _size - 1; i++)
	{
		_array[i] = _array[i + 1];
	}
}

template<class T>
bool Array<T>::SaveData(const char* filename)
{
	FILE* fp = fopen(filename, "wb");
	if (NULL == fp)
	{
		printf("파일을 읽을 수 없습니다!!\n");
		return false;
	}

	int ws = fwrite(_array, sizeof(T), _size, fp);
	fclose(fp);

	if (ws != _size)
		return false;

	return true;
}

template<class T>
bool Array<T>::LoadData(const char* filename)
{
	FILE* fp = fopen(filename, "rb");
	if (NULL == fp)
	{
		printf("파일을 읽을 수 없습니다!!\n");
		return false;
	}

	int rs = fread(_array, sizeof(T), _size, fp);
	fclose(fp);

	if (rs != _size)
		return false;

	return true;
}

template<class T>
int Array<T>::GetSize()
{
	return _size;
}