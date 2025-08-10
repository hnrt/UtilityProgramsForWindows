#pragma once

#include <ShlDisp.h>

namespace hnrt
{
	class FolderPtr;

	class FolderItemsPtr
	{
	private:

		FolderItems* m_ptr;

		FolderItemsPtr(FolderItems*);

	public:

		FolderItemsPtr(const FolderItemsPtr&);
		virtual ~FolderItemsPtr();

		FolderItemsPtr& operator =(const FolderItemsPtr&);
		FolderItems* operator ->() const;

		static FolderItemsPtr Get(const FolderPtr&);

		long get_Count() const;

		__declspec(property(get = get_Count)) long Count;
	};

	inline FolderItems* FolderItemsPtr::operator ->() const
	{
		return m_ptr;
	}
}
