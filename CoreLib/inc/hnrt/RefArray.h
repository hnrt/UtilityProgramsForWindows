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
		/// Number of elements in the T array
		/// </summary>
		ULONG m_Capacity;
		/// <summary>
		/// Number of instances in the T array
		/// </summary>
		ULONG m_Length;
		//
		// followed by the T array part
		//

		/// <summary>
		/// Constructs the RefArray instance.
		/// </summary>
		/// <param name="initialCapacity">Number of elements in the T array</param>
		RefArray(size_t initialCapacity);

	public:

		RefArray(const RefArray&) = delete;
		/// <summary>
		/// Destructs the RefArray instance.
		/// </summary>
		virtual ~RefArray();
		/// <summary>
		/// Returns the number of elements in the T array.
		/// </summary>
		/// <returns>Number of the elements in the T array</returns>
		size_t GetCapacity() const;
		/// <summary>
		/// Returns the number of instances in the T array.
		/// </summary>
		/// <returns>Number of instances in the T array</returns>
		size_t GetLength() const;
		/// <summary>
		/// Sets the number of instances to the designated value, which must be equal to or smaller than the number of elements in the T array.
		/// </summary>
		/// <param name="newLength">Number of instances to be set</param>
		void SetLength(size_t newLength);
		/// <summary>
		/// Adds a T instance to the last instance in the T array.
		/// </summary>
		/// <param name="value">Reference of the T instance to be copied</param>
		/// <returns>True on success, false otherwise</returns>
		bool PushBack(const T& value);
		/// <summary>
		/// Adds all the T instances in the designated RefArray instance to the last instance in the T array.
		/// </summary>
		/// <param name="src">Reference of the RefArray instance in which the T instances are to be copied</param>
		/// <returns>True on success, false otherwise</returns>
		bool PushBack(const RefArray& src);

		void* operator new(size_t, size_t);
		void operator delete(void*);
		void operator delete(void*, size_t);
		void operator =(const RefArray&) = delete;

		__declspec(property(get = GetCapacity)) size_t Capacity;
		__declspec(property(get = GetLength, put = SetLength)) size_t Length;

	public:

		/// <summary>
		/// Creates a new RefArray instance if initialCapacity is set to a value of non-zero.
		/// </summary>
		/// <param name="initialCapacity">Initial size of the array</param>
		/// <returns>Pointer to the first element of the array if initialCapacity is set to a value of non-zero or nullptr if initialCapacity is set to a value of zero</returns>
		static T* Create(size_t initialCapacity);
		/// <summary>
		/// Changes the number of elements in the array the RefArray instance holds.
		/// </summary>
		/// <param name="pBase">Pointer to the original array</param>
		/// <param name="newCapacity">Number of the elements in the new array</param>
		/// <returns></returns>
		static T* Resize(T* pBase, size_t newCapacity);
		/// <summary>
		/// Returns the reference of the RefArray instance pointed to by pBase.
		/// </summary>
		/// <param name="pBase">Pointer to the first element of the array</param>
		/// <returns>Reference of the RefArray instance</returns>
		static RefArray<T>& Get(T* pBase);
		/// <summary>
		/// Returns the pointer to the array the designated RefArray instance holds.
		/// </summary>
		/// <param name="ra">Reference of the RefArray instance</param>
		/// <returns>Pointer to the first element of the array</returns>
		static T* Get(RefArray<T>& ra);
		/// <summary>
		/// Returns the pointer to the constant array the designated RefArray instance holds.
		/// </summary>
		/// <param name="ra">Reference of the RefArray instance</param>
		/// <returns>Pointer to the first element of the constant array</returns>
		static const T* Get(const RefArray<T>& ra);
	};

	template<typename T>
	RefArray<T>::RefArray(size_t initialCapacity)
		: RefObj()
		, m_Capacity(static_cast<ULONG>(initialCapacity))
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
	size_t RefArray<T>::GetLength() const
	{
		return static_cast<size_t>(m_Length);
	}

	template<typename T>
	void RefArray<T>::SetLength(size_t newLength)
	{
		if (Capacity < newLength)
		{
			newLength = Capacity;
		}
		T* pBase = Get(*this);
		if (Length < newLength)
		{
			do
			{
				new (&pBase[m_Length++]) T();
			} while (Length < newLength);
		}
		else
		{
			while (newLength < Length)
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
		for (ULONG index = 0; index < src.m_Length; index++)
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
	T* RefArray<T>::Create(size_t initialCapacity)
	{
		return initialCapacity ? Get(*(new(initialCapacity) RefArray<T>(initialCapacity))) : nullptr;
	}

	template<typename T>
	T* RefArray<T>::Resize(T* pBase, size_t newCapacity)
	{
		if (pBase)
		{
			RefArray<T>& instance = Get(pBase);
			if (instance.RefCnt == 1)
			{
				if (newCapacity == instance.m_Capacity)
				{
					return pBase;
				}
				else if (newCapacity)
				{
					if (newCapacity < instance.Length)
					{
						instance.Length = newCapacity;
					}
					size_t required = sizeof(RefArray<T>) + newCapacity * sizeof(T);
					RefArray<T>* pInstance = reinterpret_cast<RefArray<T>*>(Realloc(&instance, required));
					pInstance->m_Capacity = static_cast<ULONG>(newCapacity);
					return Get(*pInstance);
				}
				else
				{
					instance.Release();
					return nullptr;
				}
			}
			else
			{
				T* pNewBase = Create(newCapacity);
				if (pNewBase)
				{
					Get(pNewBase).PushBack(instance);
				}
				instance.Release();
				return pNewBase;
			}
		}
		else
		{
			return Create(newCapacity);
		}
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

	/// <summary>
	/// Increments the referencing count for the RefArray instance.
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
	/// Decrements the referencing count for the RefArray instance.
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
