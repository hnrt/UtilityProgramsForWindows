#pragma once


#include <Windows.h>
#include <intrin.h>


namespace hnrt
{
	template<typename T>
	class Interlocked
	{
	public:

		static T ExchangePointer(T* target, T value);
		static T CompareExchangePointer(T* target, T value, T comperand);
	};

	template<typename T>
	inline T Interlocked<T>::ExchangePointer(T* target, T value)
	{
		return reinterpret_cast<T>(_InterlockedExchangePointer(reinterpret_cast<PVOID*>(target), value));
	}

	template<typename T>
	inline T Interlocked<T>::CompareExchangePointer(T* target, T value, T comperand)
	{
		return reinterpret_cast<T>(_InterlockedCompareExchangePointer(reinterpret_cast<PVOID*>(target), value, comperand));
	}
}
