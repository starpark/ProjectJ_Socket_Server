#pragma once

template <typename T>
class LockQueue
{
public:
	void Push(T item)
	{
		WRITE_LOCK;
		items_.push(item);
	}

	T Pop()
	{
		WRITE_LOCK;
		if (IsEmpty())
		{
			return T();
		}

		T returnVal = items_.front();
		items_.pop();

		return returnVal;
	}

	void PopAll(vector<T>& items)
	{
		WRITE_LOCK;
		while (T item = Pop())
		{
			items.push_back(item);
		}
	}

	void Clear()
	{
		WRITE_LOCK;
		items_ = queue<T>();
	}

	int Size()
	{
		READ_LOCK;
		int size = static_cast<int>(items_.size());
		return items_.size();
	}

	bool IsEmpty()
	{
		READ_LOCK;
		bool isEmpty = items_.empty();
		return isEmpty;
	}

private:
	USE_LOCK;
	queue<T> items_;
};

template <typename T>
class LockPriorityQueue
{
public:
	void Push(T item)
	{
		WRITE_LOCK;
		items_.push(item);
	}

	T Pop()
	{
		WRITE_LOCK;
		if (IsEmpty())
		{
			return T();
		}

		T returnVal = items_.top();
		items_.pop();

		return returnVal;
	}

	void PopAll(vector<T>& items)
	{
		WRITE_LOCK;
		while (T item = Pop())
		{
			items.push_back(item);
		}
	}

	void Clear()
	{
		WRITE_LOCK;
		while (T item = Pop())
		{
		}
	}

	int Size()
	{
		READ_LOCK;
		int size = static_cast<int>(items_.size());
		return items_.size();
	}

	bool IsEmpty()
	{
		READ_LOCK;
		bool isEmpty = items_.empty();
		return isEmpty;
	}

private:
	USE_LOCK;
	priority_queue<T> items_;
};
