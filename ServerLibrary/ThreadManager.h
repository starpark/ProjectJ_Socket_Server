#pragma once

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void Launch(function<void(void)> callback);
	void Join();

	static void InitTLS();
	static void DestroyTLS();

	static void DoGlobalQueueWork();
	static void DistributeReservedJobs();

private:
	USE_LOCK;
	vector<thread> _threads;
};
