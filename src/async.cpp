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
	thread = std::thread(&AsyncWorker::CompleteWorks, this);
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
#if MAYA_DEBUG
		auto& cm = *CoreManager::Instance();
		float start = cm.GetTimeSince();
#endif
		work();
#if MAYA_DEBUG
		MAYA_DEBUG_LOG_INFO("Async work " + std::to_string(onwork + 1) + " completed in "
			+ std::to_string(cm.GetTimeSince() - start) + "s");
#endif
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