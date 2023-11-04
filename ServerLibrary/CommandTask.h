#pragma once

using CallbackType = std::function<void()>;

class CommandTask
{
public:
	template <typename ObjectType, typename RetValType, typename... Args>
	CommandTask(shared_ptr<ObjectType> object, RetValType (ObjectType::* memberFunc)(Args ...), Args&&... args)
	{
		callback_ = [object, memberFunc, args...]()
		{
			(object.get()->*memberFunc)(args...);
		};
	}

	void Execute()
	{
		callback_();
	}

protected:
	CallbackType callback_;
};

using CommandTaskRef = shared_ptr<CommandTask>;

class CommandTaskManager
{
public:
	template <typename ObjectType, typename RetValType, typename... Args>
	void DoTaskAsync(shared_ptr<ObjectType> object, RetValType (ObjectType::* memberFunc)(Args ...), Args ... args)
	{
		tasks_.Push(ObjectPool<CommandTask>::MakeShared(object, memberFunc, forward<Args>(args)...));
	}

	void Execute();

private:
	LockQueue<CommandTaskRef> tasks_;
};
