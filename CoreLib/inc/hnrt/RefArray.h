#pragma once

#include <malloc.h>
#include <Windows.h>
#include "hnrt/RefObj.h"
#include "hnrt/Heap.h"

namespace hnrt
{
	template<typename T>
	class RefArray
		: public RefObj
	{
	private:

		/// <summary>
		/// Number of the elements in the array part
		/// </summary>
		ULONG m_Capacity;
		/// <summary>
		/// Number of the constructed elements in the array part
		/// </summary>
		ULONG m_Length;

		// followed by the array part, the length of which is determined by the capacity value.

		RefArray(size_t capacity);

	public:

		RefArray(const RefArray&) = delete;
		virtual ~RefArray();
		size_t GetCapacity() const;
		/// <summary>
		/// Changes the capacity to the specified value, which must be smaller than the current one.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="capacity">New capacity</param>
		void SetCapacity(size_t capacity);
		size_t GetLength() const;
		/// <summary>
		/// Changes the length to the specified value, which must be equal to or smaller than the capacity.
		/// </summary>
		/// <param name="length">New length</param>
		void SetLength(size_t length);
		bool PushBack(const T& value);
		bool PushBack(const RefArray& src);

		void* operator new(size_t, size_t);
		void operator delete(void*);
		void operator delete(void*, size_t);
		void operator =(const RefArray&) = delete;

		__declspec(property(get = GetCapacity, put = SetCapacity)) size_t Capacity;
		__declspec(property(get = GetLength, put = SetLength)) size_t Length;

	public:

		static RefArray<T>& Get(T*);
		static T* Get(RefArray<T>&);
		static const T* Get(const RefArray<T>&);
		static T* Create(size_t);
	};

	template<typename T>
	RefArray<T>::RefArray(size_t capacity)
		: RefObj()
		, m_Capacity(static_cast<ULONG>(capacity))
		, m_Length(0)
	{
	}

	template<typename T>
	RefArray<T>::~RefArray()
	{
		SetLength(0);
	}

	template<typename T>
	size_t RefArray<T>::GetCapacity() const
	{
		return static_cast<size_t>(m_Capacity);
	}

	template<typename T>
	void RefArray<T>::SetCapacity(size_t capacity)
	{
		ULONG uCapacity = static_cast<ULONG>(capacity);
		if (uCapacity < m_Capacity)
		{
			if (uCapacity < m_Length)
			{
				SetLength(uCapacity);
			}
			m_Capacity = uCapacity;
		}
	}

	template<typename T>
	size_t RefArray<T>::GetLength() const
	{
		return static_cast<size_t>(m_Length);
	}

	template<typename T>
	void RefArray<T>::SetLength(size_t length)
	{
		ULONG uLength = static_cast<ULONG>(length);
		if (m_Capacity < uLength)
		{
			uLength = m_Capacity;
		}
		T* pBase = Get(*this);
		if (m_Length < uLength)
		{
			do
			{
				new (&pBase[m_Length++]) T();
			} while (m_Length < uLength);
		}
		else
		{
			while (uLength < m_Length)
			{
				pBase[--m_Length].~T();
			}
		}
	}

	template<typename T>
	bool RefArray<T>::PushBack(const T& value)
	{
		if (m_Length < m_Capacity)
		{
			T* pBase = Get(*this);
			new (&pBase[m_Length++]) T(value);
			return true;
		}
		else
		{
			return false;
		}
	}

	template<typename T>
	bool RefArray<T>::PushBack(const RefArray& src)
	{
		T* pBaseDst = RefArray<T>::Get(*this);
		const T* pBaseSrc = RefArray<T>::Get(src);
		for (size_t index = 0; index < src.Length; index++)
		{
			if (m_Length < m_Capacity)
			{
				new (&pBaseDst[m_Length++]) T(pBaseSrc[index]);
			}
			else
			{
				return false;
			}
		}
		return true;
	}

	template<typename T>
	void* RefArray<T>::operator new(size_t size, size_t capacity)
	{
		size_t required = sizeof(RefArray<T>) + capacity * sizeof(T);
		return Malloc(required > size ? required : size);
	}

	template<typename T>
	void RefArray<T>::operator delete(void* ptr)
	{
		::free(ptr);
	}

	template<typename T>
	void RefArray<T>::operator delete(void* ptr, size_t)
	{
		::free(ptr);
	}

	template<typename T>
	RefArray<T>& RefArray<T>::Get(T* pBase)
	{
		return *(reinterpret_cast<RefArray<T>*>(pBase) - 1);
	}

	template<typename T>
	T* RefArray<T>::Get(RefArray<T>& ra)
	{
		return reinterpret_cast<T*>(&ra + 1);
	}

	template<typename T>
	const T* RefArray<T>::Get(const RefArray<T>& ra)
	{
		return reinterpret_cast<const T*>(&ra + 1);
	}

	template<typename T>
	T* RefArray<T>::Create(size_t capacity)
	{
		return Get(*(new(capacity) RefArray<T>(capacity)));
	}

	/// <summary>
	/// Increments the referencing count for the array.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="pBase">Pointer to the array part</param>
	/// <returns>pBase</returns>
	template<typename T>
	T* ArrayAddRef(T* pBase)
	{
		if (pBase)
		{
			RefArray<T>::Get(pBase).AddRef();
		}
		return pBase;
	}

	/// <summary>
	/// Decrements the referencing count for the array.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="pBase">Pointer to the array part</param>
	/// <returns>pBase</returns>
	template<typename T>
	void ArrayRelease(T* pBase)
	{
		if (pBase)
		{
			RefArray<T>::Get(pBase).Release();
		}
	}
}
