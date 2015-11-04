#pragma once

#include <memory>
namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			template<typename T>
			class pimpl {
			private:
				std::unique_ptr<T> m;
			public:
				pimpl();
				pimpl(pimpl&& p);
				template<typename ...Args> pimpl(Args&& ...);
				~pimpl();
				T* get();
				T* operator->();
				T& operator*();
			};
		}
	}
}
