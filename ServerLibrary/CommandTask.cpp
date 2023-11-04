#include "pch.h"
#include "CommandTask.h"

void CommandTaskManager::Execute()
{
	while (CommandTaskRef task = tasks_.Pop())
	{
		const UINT64 nowTick = GetTickCount64();
		if (nowTick >= LEndTickCount)
		{
			break;
		}

		task->Execute();
	}
}
