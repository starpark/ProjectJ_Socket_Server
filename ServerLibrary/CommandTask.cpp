#include "pch.h"
#include "CommandTask.h"

CommandTaskObject::~CommandTaskObject()
{
	tasks_.Clear();
}

void CommandTaskObject::Push(CommandTaskRef task, bool pushOnly)
{
	const int currentCount = taskCount_.fetch_add(1);
	tasks_.Push(task);

	if (currentCount == 0)
	{
		if (LCurrentCommandTaskObject == nullptr && pushOnly == false)
		{
			Execute();
		}
		else
		{
			GCommandTaskManager->Push(shared_from_this());
		}
	}
}

void CommandTaskObject::Execute()
{
	LCurrentCommandTaskObject = this;

	while (true)
	{
		vector<CommandTaskRef> tasks;
		tasks_.PopAll(tasks);

		const int taskCount = static_cast<int>(tasks.size());
		for (int i = 0; i < taskCount; i++)
		{
			tasks[i]->Execute();
		}

		if (taskCount_.fetch_sub(taskCount) == taskCount)
		{
			LCurrentCommandTaskObject = nullptr;
			break;
		}

		const UINT64 nowTick = GetTickCount64();
		if (nowTick >= LEndTickCount)
		{
			LCurrentCommandTaskObject = nullptr;
			GCommandTaskManager->Push(shared_from_this());

			break;
		}
	}
}


void CommandTaskManager::Push(CommandTaskObjectRef object)
{
	objectQueues_.Push(object);
}

CommandTaskObjectRef CommandTaskManager::Pop()
{
	return objectQueues_.Pop();
}

void CommandTaskManager::ExecuteTask()
{
	while (true)
	{
		const UINT64 nowTick = GetTickCount64();
		if (nowTick >= LEndTickCount)
		{
			break;
		}

		CommandTaskObjectRef object = GCommandTaskManager->Pop();
		if (object == nullptr)
		{
			break;
		}

		object->Execute();
	}
}
