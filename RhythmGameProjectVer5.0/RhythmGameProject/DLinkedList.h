#pragma once
#include <stdio.h>

template <class T> class Node;
template <class T> class DLinkedList;
template <class T> class DLinkedListIterator;

template <class T>
class Node
{
public:
	T _data;
	Node<T>* Next;
	Node<T>* Prev;

	void InsertAfter(T data)
	{
		Node<T>* newNode = new Node<T>();
		newNode->_data = data;

		newNode->Prev = this;
		newNode->Next = Next;
		if (0 != Next)
			Next->Prev = newNode;
		Next = newNode;
	}

	void InsertBefore(T data)
	{
		Node<T>* newNode = new Node<T>();
		newNode->_data = data;

		newNode->Next = this;
		newNode->Prev = Prev;
		if (0 != Prev)
			Prev->Next = newNode;
		Prev = newNode;
	}

	void Dlink()
	{
		if (0 != Prev)
			Prev->Next = Next;
		if (0 != Next)
			Next->Prev = Prev;
	}
};

template <class T>
class DLinkedList
{
private:
	int _count;

public:
	Node<T>* _head;
	Node<T>* _tail;

	DLinkedList()
	{
		_head = 0;
		_tail = 0;
		_count = 0;
	}

	~DLinkedList()
	{
		Node<T>* node = _head;
		Node<T>* next;

		while (0 != node)
		{
			next = node->Next;
			delete node;
			node = next;
		}
	}

	void Append(T data)
	{
		if (0 == _head)
		{
			Node<T>* newNode = new Node<T>();

			_head = newNode;
			_tail = newNode;

			_head->_data = data;
			_head->Next = 0;
			_head->Prev = 0;

			_count++;
		}
		else
		{
			_tail->InsertAfter(data);
			_tail = _tail->Next;

			_count++;
		}
	}

	void Prepend(T data)
	{
		if (0 == _head)
		{
			Node<T>* newNode = new Node<T>();

			_head = newNode;
			_tail = newNode;

			_head->_data = data;
			_head->Next = 0;
			_head->Prev = 0;

			_count++;
		}
		else
		{
			_head->InsertBefore(data);
			_head = _head->Prev;

			_count++;
		}
	}

	void RemoveHead()
	{
		if (0 != _head)
		{
			Node<T>* node = _head->Next;
			delete _head;
			_head = node;

			if (0 == _head)
				_tail = 0;
			else
				_head->Prev = 0;
			_count--;
		}
	}

	void RemoveTail()
	{
		if (0 != _head)
		{
			Node<T>* node = _tail->Prev;
			delete _tail;
			_tail = node;

			if (0 == _tail)
				_head = 0;
			else
				_tail->Next = 0;
			_count--;
		}
	}

	DLinkedListIterator<T> GetIterator()
	{
		return DLinkedListIterator<T>(this, _head);
	}

	void Remove(DLinkedListIterator<T>& itr)
	{
		if (0 == itr._node)
			return;

		Node<T>* newNode = itr._node;

		if (_head == itr._node)
			_head = _head->Next;
		else if (_tail == itr._node)
			_tail = _tail->Prev;

		itr.Forth();
		newNode->Dlink();		//Dlink: 끊어진 노드를 연결

		delete newNode;

		if (0 == _head)
			_tail = 0;

		_count--;
	}

	void InsertAfter(DLinkedListIterator<T>& itr, T data)
	{
		if (0 != itr._node)
		{
			itr._node->InsertAfter(data);
			if (itr._node == _tail)
				_tail = itr._node->Next;
			_count++;
		}
		else
		{
			Append(data);
		}
	}

	void InsertBefore(DLinkedListIterator<T>& itr, T data)
	{
		if (0 != itr._node)
		{
			itr._node->InsertBefore(data);
			if (itr._node == _head)
				_head = itr._node->Prev;
			_count++;
		}
		else
		{
			Prepend(data);
		}
	}

	bool SaveData(const char* filename)
	{
		FILE* fp = fopen(filename, "wb");
		if (0 == fp)
		{
			printf("파일을 읽을 수 없습니다!!\n");
			return false;
		}

		fwrite(&_count, sizeof(int), 1, fp);

		Node<T>* node = _head;
		while (0 != node)
		{
			fwrite(&node->_data, sizeof(T), 1, fp);
			node = node->Next;
		}
		fclose(fp);

		return true;
	}

	bool LoadData(const char* filename)
	{
		FILE* fp = fopen(filename, "rb");
		if (0 == fp)
		{
			printf("파일을 읽을 수 없습니다!!\n");
			return false;
		}

		int count;
		fread(&count, sizeof(int), 1, fp);
		while (0 < count)
		{
			T buffer;
			fread(&buffer, sizeof(T), 1, fp);
			Append(buffer);
			count--;
		}
		fclose(fp);

		return true;
	}

	int Count()
	{
		return _count;
	}
};

template<class T>
class DLinkedListIterator
{
public:
	DLinkedList<T>* _dll;
	Node<T>* _node;

	DLinkedListIterator(DLinkedList<T>* dll, Node<T>* node)
	{
		_dll = dll;
		_node = node;
	}

	void Start()
	{
		if (0 != _dll)
			_node = _dll->_head;
	}

	void End()
	{
		if (0 != _dll)
			_node = _dll->_tail;
	}

	void Forth()
	{
		if (0 != _node)
			_node = _node->Next;
	}

	void Back()
	{
		if (0 != _node)
			_node = _node->Prev;
	}

	bool Valid()
	{
		if (0 == _node)
			return false;
		return true;
	}

	T Item()
	{
		return _node->_data;
	}
};