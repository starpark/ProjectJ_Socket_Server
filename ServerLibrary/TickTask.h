#pragma once


class TickTask : public enable_shared_from_this<TickTask>
{
public:
	TickTask();
	virtual ~TickTask();
	virtual void Tick(double deltaTime) = 0;
};

struct TickItem
{
	bool operator<(const TickItem& other) const
	{
		return executeTick_ > other.executeTick_;
	}

	UINT64 lastTick_;
	UINT64 executeTick_;
	weak_ptr<TickTask> task_;
};

class TickTaskManager
{
	enum
	{
		NEXT_TICK = 10
	};

public:
	void DoTask(UINT64 currentTick);
	void AddTask(const shared_ptr<TickTask>& task);
	void RemoveTask(const shared_ptr<TickTask>& task);

	template <typename TaskType, typename ...Args>
	static shared_ptr<TaskType> MakeTask(Args&&... args)
	{
		static_assert(is_base_of_v<TickTask, TaskType>, "");
		shared_ptr<TaskType> task = ObjectPool<TaskType>::MakeShared(forward<Args>(args)...);
		GTickTaskManager->AddTask(task);

		return task;
	}

private:
	bool FindPendingRemove(shared_ptr<TickTask> task);

private:
	USE_LOCK;
	priority_queue<TickItem> tickTaskQueue_;
	set<shared_ptr<TickTask>> workingTasks_;
};
