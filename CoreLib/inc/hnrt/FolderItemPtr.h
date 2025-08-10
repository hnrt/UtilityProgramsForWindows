#pragma once

#include <ShlDisp.h>
#include "hnrt/String.h"

namespace hnrt
{
	class FolderItemPtr;

	class FolderItemsPtr;

	struct IFolderItemForEachCallback
	{
		virtual BOOL ForEach(FolderItemPtr&) = 0;
	};

	struct IFolderItemProcessCallbacks
	{
		virtual void OnStart(PCWSTR, LONG) = 0;
		virtual void OnSuccess(PCWSTR) = 0;
		virtual void OnFailure(PCWSTR, HRESULT) = 0;
	};

	class FolderItemPtr
	{
	private:

		FolderItem* m_ptr;

		FolderItemPtr(FolderItem*);

	public:

		FolderItemPtr(const FolderItemPtr&);
		virtual ~FolderItemPtr();
		BOOL ForEach(IFolderItemForEachCallback&);
		void CopyTo(PCWSTR, IFolderItemProcessCallbacks&);

		FolderItemPtr& operator =(const FolderItemPtr&);
		FolderItem* operator ->() const;

		static FolderItemPtr Get(long, const FolderItemsPtr&);

		String get_Name() const;
		String get_Type() const;
		LONG get_Size() const;
		BOOL get_IsFolder() const;

		__declspec(property(get = get_Name)) String Name;
		__declspec(property(get = get_Type)) String Type;
		__declspec(property(get = get_Size)) LONG Size;
		__declspec(property(get = get_IsFolder)) BOOL IsFolder;
	};

	inline FolderItem* FolderItemPtr::operator ->() const
	{
		return m_ptr;
	}
}
