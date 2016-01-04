#include "stdafx.h"
#include "ThreadPool.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class ThreadPool_Impl {
			public:
				ThreadPool_Impl(int threads) : terminate(false), stopped(false) {
					task_count = 0;
					// Create number of required threads and add them to the thread pool vector.
					for (int i = 0; i < threads; i++)
					{
						threadPool.emplace_back(std::thread(&ThreadPool_Impl::Invoke, this));
					}
				}
				~ThreadPool_Impl() {
					{
						std::unique_lock<std::mutex> lock(tasksMutex);
						terminate = true;
					}
					condition.notify_all();
					for (auto &thread : threadPool)
					{
						thread.join();
					}
					threadPool.empty();
					stopped = true;
				}
				std::vector<std::thread> threadPool;
				std::queue<std::function<void()>> tasks;
				std::mutex tasksMutex;
				std::condition_variable condition;
				bool terminate;
				bool stopped;
				std::atomic<unsigned int> task_count;
				void Invoke() {
					while (true)
					{
						std::function<void()> task;
						{
							std::unique_lock<std::mutex> lock(tasksMutex);
							condition.wait(lock, [this] { return !tasks.empty() || terminate; });
							if (terminate && tasks.empty())
							{
								return;
							}
							task = tasks.front();
							tasks.pop();
						}
						task();
						task_count -= 1;
					}
				}
				void Enqueue(std::function<void()> f)
				{
					task_count += 1;
					{
						std::unique_lock<std::mutex> lock(tasksMutex);
						tasks.push(f);
					}
					condition.notify_one();
				}
			};
		};
	};
};


// Constructor.
SL::Remote_Access_Library::Utilities::ThreadPool::ThreadPool(int threads) : _ThreadPool_Impl(new ThreadPool_Impl(threads)) {}

void SL::Remote_Access_Library::Utilities::ThreadPool::Enqueue(std::function<void()> f)
{
	_ThreadPool_Impl->Enqueue(f);
}

unsigned int SL::Remote_Access_Library::Utilities::ThreadPool::TaskCount() const
{
	return _ThreadPool_Impl->task_count;
}

SL::Remote_Access_Library::Utilities::ThreadPool::~ThreadPool()
{
	delete _ThreadPool_Impl;
}