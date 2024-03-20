#pragma once

#include "./core.hpp"
#include "./math.hpp"

namespace maya
{

class AsyncWorker
{
public:

	AsyncWorker();

	~AsyncWorker();

	AsyncWorker(AsyncWorker const&) = delete;
	AsyncWorker& operator=(AsyncWorker const&) = delete;

	unsigned Work(stl::fnptr<void()> const& work);

	void Clear();

	void Start();

	bool IsWorkDone(unsigned work) const;

	bool IsRunning() const;

	unsigned GetWorkCount() const;

	unsigned GetWorkProgress() const;

private:

	stl::thread thread;
	stl::list<stl::fnptr<void()>> worklist;
	stl::atomic<unsigned> onwork;

	void CompleteWorks();
};

}