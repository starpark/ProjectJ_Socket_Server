#pragma once

#include <any>

using CommandType = std::function<void()>;

class CommandTask
{
public:
	CommandTask(CommandType&& command) : command_(move(command))
	{
	}

	template <typename ObjectType, typename RetValType, typename... Args>
	CommandTask(shared_ptr<ObjectType> object, RetValType (ObjectType::* memberFunc)(Args ...), Args&&... args)
	{
		command_ = [object, memberFunc, args...]()
		{
			(object.get()->*memberFunc)(args...);
		};
	}

	template <typename ObjectType, typename RetValType, typename Lambda, typename... Args>
	CommandTask(shared_ptr<ObjectType> object,
	            RetValType (ObjectType::* memberFunc)(Args ...),
	            Lambda&& callbackFunc,
	            Args&&... args

	)
	{
		command_ = [object, memberFunc, callback = move(callbackFunc), args...]()
		{
			if constexpr (std::is_same_v<RetValType, void>)
			{
				(object.get()->*memberFunc)(args...);

				(callback)(object);
			}
			else
			{
				RetValType retVal = (object.get()->*memberFunc)(args...);

				(callback)(object, retVal);
			}
		};
	}

	void Execute()
	{
		command_();
	}

protected:
	CommandType command_;
};

using CommandTaskRef = shared_ptr<CommandTask>;

class CommandTaskObject : public enable_shared_from_this<CommandTaskObject>
{
public:
	virtual ~CommandTaskObject();

	void DoTaskAsync(CommandType&& command)
	{
		Push(ObjectPool<CommandTask>::MakeShared(move(command)));
	}

	void DoTaskSync(CommandType&& command)
	{
		WRITE_LOCK;
		command();
	}

	template <typename ObjectType, typename RetValType, typename... Args>
	void DoTaskAsync(RetValType (ObjectType::* memberFunc)(Args ...), Args ... args)
	{
		static_assert(is_base_of_v<CommandTaskObject, ObjectType>, "The object must inherit from CommandTaskObject.");

		shared_ptr<ObjectType> object = static_pointer_cast<ObjectType>(shared_from_this());
		Push(ObjectPool<CommandTask>::MakeShared(object, memberFunc, forward<Args>(args)...));
	}

	template <typename ObjectType, typename RetValType, typename Lambda, typename... Args>
	void DoTaskCallback(RetValType (ObjectType::* memberFunc)(Args ...),
	                    Lambda&& callbackFunc,
	                    Args ... commandArgs
	)
	{
		static_assert(is_base_of_v<CommandTaskObject, ObjectType>, "The object must inherit from CommandTaskObject.");

		shared_ptr<ObjectType> object = static_pointer_cast<ObjectType>(shared_from_this());
		Push(
			ObjectPool<CommandTask>::MakeShared(
				object,
				memberFunc,
				forward<decltype(callbackFunc)>(callbackFunc),
				forward<Args>(commandArgs)...
			)
		);
	}

	template <typename ObjectType, typename RetValType, typename... Args>
	RetValType DoTaskSync(RetValType (ObjectType::* memberFunc)(Args ...), Args ... args)
	{
		static_assert(is_base_of_v<CommandTaskObject, ObjectType>, "The object must inherit from CommandTaskObject.");

		shared_ptr<ObjectType> object = static_pointer_cast<ObjectType>(shared_from_this());
		WRITE_LOCK;
		return (object.get()->*memberFunc)(args...);
	}

	void Push(CommandTaskRef task, bool pushOnly = false);
	void Execute();


protected:
	atomic<int> taskCount_ = 0;
	LockQueue<CommandTaskRef> tasks_;

private:
	USE_LOCK;
};

using CommandTaskObjectRef = shared_ptr<CommandTaskObject>;

class CommandTaskManager
{
public:
	void Push(CommandTaskObjectRef object);
	CommandTaskObjectRef Pop();
	void ExecuteTask();

private:
	LockQueue<CommandTaskObjectRef> objectQueues_;
};
