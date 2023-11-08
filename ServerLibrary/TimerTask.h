#pragma once

#include "CommandTask.h"

using TickFuncType = function<void(float)>;

struct TimerTask
{
public:
	TimerTask(weak_ptr<CommandTaskObject> object, CommandTaskRef task)
		: object(object), task(task)
	{
	}

	weak_ptr<CommandTaskObject> object;
	CommandTaskRef task;
};

enum TimerStatus : UINT64
{
	DefaultState = 0,
	RemovedState = 1,
};

struct TimerData
{
	TimerData(bool inIsLoop, UINT64 inDelayTick, UINT64 inExecuteTick, TimerTask* inTaskData)
		: isLoop(inIsLoop), delayTick(inDelayTick), executeTick(inExecuteTick),
		  taskData(inTaskData)
	{
	}

	bool operator<(const TimerData& other) const
	{
		return executeTick > other.executeTick;
	}

	bool isLoop = false;
	UINT64 delayTick;
	UINT64 executeTick;
	TimerTask* taskData;
	atomic<UINT64> state{DefaultState};
};

using TimerHandle = weak_ptr<TimerData>;
using TimerElementPtr = shared_ptr<TimerData>;

class TimerTaskManager
{
public:
	~TimerTaskManager();

	template <typename ObjectType, typename RetValType, typename... Args>
	TimerHandle AddTimer(UINT64 delayTick, bool isLoop, shared_ptr<ObjectType> object,
	                     RetValType (ObjectType::* memberFunc)(Args ...),
	                     Args&&... args)
	{
		static_assert(is_base_of_v<CommandTaskObject, ObjectType>, "The object must inherit from CommandTaskObject.");

		shared_ptr<CommandTask> task = ObjectPool<CommandTask>::MakeShared(
			static_pointer_cast<ObjectType>(object), memberFunc, forward<Args>(args)...);
		TimerTask* timerTask = ObjectPool<TimerTask>::Pop(object, task);

		UINT64 nowTick = GetTickCount64();
		auto newTimerElement = make_shared<TimerData>(isLoop, delayTick, nowTick + delayTick, timerTask);
		addedElements_.Push(newTimerElement);

		return newTimerElement;
	}

	void RemoveTimer(TimerHandle handle);
	void Distribute();
	UINT32 GetThreadId(UINT64 state);

private:
	USE_LOCK;
	UINT64 currentTick = 0;
	atomic<bool> distributing_ = false;
	LockPriorityQueue<TimerElementPtr> addedElements_;
	vector<TimerElementPtr> elements_;
};
