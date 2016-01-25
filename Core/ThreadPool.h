#pragma once
// containers
#include <vector>
#include <queue>
// threading
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
// utility wrappers
#include <memory>
#include <functional>


namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {


			// std::thread pool for resources recycling
			class ThreadPool {
			public:
				// the constructor just launches some amount of workers
				ThreadPool(int threads_n = 2) : stop(false)
				{
					this->workers.reserve(threads_n);
					for (; threads_n; --threads_n)
						this->workers.emplace_back(
							[this]
					{
						while (true)
						{
							std::function<void()> task;

							{
								std::unique_lock<std::mutex> lock(this->queue_mutex);
								this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
								if (this->stop && this->tasks.empty())
									return;
								task = std::move(this->tasks.front());
								this->tasks.pop();
								--num_Tasks;
							}

							task();
						}
					}
					);
				}
				// deleted copy&move ctors&assignments
				ThreadPool(const ThreadPool&) = delete;
				ThreadPool& operator=(const ThreadPool&) = delete;
				ThreadPool(ThreadPool&&) = delete;
				ThreadPool& operator=(ThreadPool&&) = delete;
				// add new work item to the pool
				template<class F, class... Args>
				std::future<typename std::result_of<F(Args...)>::type> Enqueue(F&& f, Args&&... args)
				{
					using packaged_task_t = std::packaged_task<typename std::result_of<F(Args...)>::type()>;

					std::shared_ptr<packaged_task_t> task(new packaged_task_t(std::bind(std::forward<F>(f), std::forward<Args>(args)...)));
					auto res = task->get_future();
					{
						++num_Tasks;
						std::unique_lock<std::mutex> lock(this->queue_mutex);
						this->tasks.emplace([task]() { (*task)(); });
					}
					this->condition.notify_one();
					return res;
				}
				// the destructor joins all threads
				~ThreadPool()
				{
					this->stop = true;
					this->condition.notify_all();
					for (std::thread& worker : this->workers)
						worker.join();
				}
				int TaskCount()const {
					return num_Tasks;
				}
			private:
				// need to keep track of threads so we can join them
				std::vector< std::thread > workers;
				// the task queue
				std::queue< std::function<void()> > tasks;
				int num_Tasks;
				// synchronization
				std::mutex queue_mutex;
				std::condition_variable condition;
				// workers finalization flag
				std::atomic_bool stop;
			};
		}
	}
}

