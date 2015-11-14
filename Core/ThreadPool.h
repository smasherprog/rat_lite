#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class ThreadPool
			{
			public:

				// Constructor.
				ThreadPool(int threads=2);

				// Destructor.
				~ThreadPool();

				// Adds task to a task queue.
				void Enqueue(std::function<void()> f);

				// Shut down the pool.
				void ShutDown();

			private:
				// Thread pool storage.
				std::vector<std::thread> threadPool;

				// Queue to keep track of incoming tasks.
				std::queue<std::function<void()>> tasks;

				// Task queue mutex.
				std::mutex tasksMutex;

				// Condition variable.
				std::condition_variable condition;

				// Indicates that pool needs to be shut down.
				bool terminate;

				// Indicates that pool has been terminated.
				bool stopped;

				// Function that will be invoked by our threads.
				void Invoke();
			};
		}
	}
}
