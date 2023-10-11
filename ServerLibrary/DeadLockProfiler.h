#pragma once
#include <stack>
#include <map>
#include <vector>

class DeadLockProfiler
{
public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();

private:
	void Dfs(int index);

private:
	unordered_map<const char*, int> _nameToId;
	unordered_map<int, const char*> _idToName;
	map<int, set<int>> _lockHistory;

	mutex _lock;

private:
	vector<int> _discoveredOrder;
	int _discoveredCount = 0;
	vector<bool> _finished;
	vector<int> _parent;
};
