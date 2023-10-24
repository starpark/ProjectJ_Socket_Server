#pragma once

struct TickCounts
{
	LARGE_INTEGER lastTick_{0,};
	LARGE_INTEGER currentTick_{0,};
	LARGE_INTEGER frequency_{0,};
};

class TickTask : public enable_shared_from_this<TickTask>
{
public:
	TickTask();
	virtual ~TickTask();
	virtual void Tick(double deltaTime) = 0;
};

class TickTaskManager
{
public:
	void DoTask();
	void AddTask(const shared_ptr<TickTask>& task);
	void DeleteTask(const shared_ptr<TickTask>& task);

	template <typename TaskType, typename ...Args>
	static shared_ptr<TaskType> MakeTask(Args&&... args)
	{
		static_assert(is_base_of<TickTask, TaskType>::value, "");
		shared_ptr<TaskType> task = make_shared<TaskType>(forward<Args>(args)...);
		GTickTaskManager->AddTask(task);

		return task;
	}

private:
	USE_LOCK;
	queue<weak_ptr<TickTask>> tickTaskQueue_;
	double deltaSeconds_ = 1 / 60.0;
};
