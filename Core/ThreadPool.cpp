#include "stdafx.h"
#include "ThreadPool.h"


// Constructor.
SL::Remote_Access_Library::Utilities::ThreadPool::ThreadPool(int threads) :
	terminate(false),
	stopped(false)
{
	// Create number of required threads and add them to the thread pool vector.
	for (int i = 0; i < threads; i++)
	{
		threadPool.emplace_back(std::thread(&ThreadPool::Invoke, this));
	}
}

void SL::Remote_Access_Library::Utilities::ThreadPool::Enqueue(std::function<void()>&& f)
{
	// Scope based locking.
	{
		// Put unique lock on task mutex.
		std::unique_lock<std::mutex> lock(tasksMutex);

		// Push task into queue.
		tasks.emplace(std::move(f));
	}

	// Wake up one thread.
	condition.notify_one();
}

void SL::Remote_Access_Library::Utilities::ThreadPool::Invoke() {

	std::function<void()> task;
	while (true)
	{
		// Scope based locking.
		{
			// Put unique lock on task mutex.
			std::unique_lock<std::mutex> lock(tasksMutex);

			// Wait until queue is not empty or termination signal is sent.
			condition.wait(lock, [this] { return !tasks.empty() || terminate; });

			// If termination signal received and queue is empty then exit else continue clearing the queue.
			if (terminate && tasks.empty())
			{
				return;
			}

			// Get next task in the queue.
			task = tasks.front();

			// Remove it from the queue.
			tasks.pop();
		}

		// Execute the task.
		task();
	}
}

void SL::Remote_Access_Library::Utilities::ThreadPool::ShutDown()
{
	// Scope based locking.
	{
		// Put unique lock on task mutex.
		std::unique_lock<std::mutex> lock(tasksMutex);

		// Set termination flag to true.
		terminate = true;
	}

	// Wake up all threads.
	condition.notify_all();

	// Join all threads.
	for (auto &thread : threadPool)
	{
		thread.join();
	}

	// Empty workers vector.
	threadPool.empty();

	// Indicate that the pool has been shut down.
	stopped = true;
}

// Destructor.
SL::Remote_Access_Library::Utilities::ThreadPool::~ThreadPool()
{
	if (!stopped)
	{
		ShutDown();
	}
}