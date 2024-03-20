#include <maya/async.hpp>
#include <iostream>

namespace maya
{

AsyncWorker::AsyncWorker()
	: onwork(0)
{
	worklist.reserve(10);
}

AsyncWorker::~AsyncWorker()
{
	if (thread.joinable())
		thread.join();
}

void AsyncWorker::Start()
{
	// assert !IsRunning
	if (thread.joinable()) thread.join();
	thread = stl::thread(&AsyncWorker::CompleteWorks, this);
}

unsigned AsyncWorker::Work(stl::fnptr<void()> const& work)
{
	worklist.emplace_back(work);
	return static_cast<unsigned>(worklist.size());
}

void AsyncWorker::Clear()
{
	worklist.clear();
}

void AsyncWorker::CompleteWorks()
{
	for (auto& work : worklist) {
		float f = CoreManager::Instance()->GetTimeSince();
		work();
		std::cout << CoreManager::Instance()->GetTimeSince() - f << '\n';
		onwork++;
	}
}

bool AsyncWorker::IsWorkDone(unsigned work) const
{
	return onwork >= work;
}

bool AsyncWorker::IsRunning() const
{
	return onwork == worklist.size();
}

unsigned AsyncWorker::GetWorkCount() const
{
	return static_cast<unsigned>(worklist.size());
}

unsigned AsyncWorker::GetWorkProgress() const
{
	return onwork;
}

}