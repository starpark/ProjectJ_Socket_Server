#pragma once

#include "CommandTask.h"

class TimerTask : public CommandTask
{
public:
	template <typename ObjectType, typename RetValType, typename... Args>
	TimerTask(shared_ptr<ObjectType> object, RetValType (ObjectType::* memberFunc)(Args ...), Args&&... args)
		: CommandTask(object, memberFunc, forward<Args>(args)...)
	{
	}
};

struct TimerItem
{
	bool operator<(const TimerItem& other) const
	{
		return executeTick_ > other.executeTick_;
	}

	UINT64 executeTick_ = 0;
	shared_ptr<TimerTask> task_ = nullptr;
};

class TimerTaskManager
{
public:
	template <typename ObjectType, typename RetValType, typename... Args>
	void Reserve(UINT64 tickAfter, shared_ptr<ObjectType> object, RetValType (ObjectType::* memberFunc)(Args ...),
	             Args&&... args)
	{
		const UINT64 executeTick = GetTickCount64() + tickAfter;
		shared_ptr<TimerTask> task = ObjectPool<TimerTask>::MakeShared(object, memberFunc, forward<Args>(args)...);

		WRITE_LOCK;
		timerTaskQueue_.push(TimerItem{executeTick, task});
	}

	void Execute();


private:
	USE_LOCK;
	priority_queue<TimerItem> timerTaskQueue_;
};
