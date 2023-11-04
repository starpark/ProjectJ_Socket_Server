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

	int Size() { return items_.size(); }
	bool IsEmpty() { return items_.empty(); }

private:
	USE_LOCK;
	queue<T> items_;
};
