#pragma once

#include <utility>
namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			template<typename T>
			pimpl<T>::pimpl() : m{ new T{} } { }

			/*template<typename T>
			pimpl<T>::pimpl(pimpl&& p)
				: m(std::move(p.m))  { }
*/
			template<typename T>
			template<typename ...Args>
			pimpl<T>::pimpl(Args&& ...args)
				: m{ new T(std::forward<Args>(args)... ) } { }

			template<typename T>
			pimpl<T>::~pimpl() { }
	
			template<typename T>
			T* pimpl<T>::get() { return m.get(); }

			template<typename T>
			T* pimpl<T>::operator->() { return m.get(); }

			template<typename T>
			T& pimpl<T>::operator*() { return *m.get(); }
		}
	}
}