#pragma once
#include <functional>

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class ThreadPool_Impl;
			class ThreadPool
			{
			public:

				ThreadPool(int threads=2);
				~ThreadPool();
				void Enqueue(std::function<void()> f);
				unsigned int TaskCount() const;
			private:
				ThreadPool_Impl* _ThreadPool_Impl = nullptr; 
			
			};
		}
	}
}
