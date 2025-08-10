#pragma once

#include <ShlDisp.h>

namespace hnrt
{
	class ShellDispatchPtr;
	class FolderItemPtr;

	class FolderPtr
	{
	private:

		Folder* m_ptr;

		FolderPtr(Folder*);

	public:

		FolderPtr(const FolderPtr&);
		virtual ~FolderPtr();

		FolderPtr& operator =(const FolderPtr&);
		Folder* operator ->() const;

		static FolderPtr Get(PCWSTR, const ShellDispatchPtr&);
		static FolderPtr Get(const FolderItemPtr&);
	};

	inline Folder* FolderPtr::operator ->() const
	{
		return m_ptr;
	}
}
