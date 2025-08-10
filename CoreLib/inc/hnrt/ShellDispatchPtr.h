#pragma once

#include <ShlDisp.h>

namespace hnrt
{
	class ShellDispatchPtr
	{
	private:

		IShellDispatch6* m_ptr;

		ShellDispatchPtr(IShellDispatch6*);

	public:

		ShellDispatchPtr(const ShellDispatchPtr&);
		virtual ~ShellDispatchPtr();

		ShellDispatchPtr& operator =(const ShellDispatchPtr&);

		IShellDispatch6* operator ->() const;

		static ShellDispatchPtr Create();
	};

	inline IShellDispatch6* ShellDispatchPtr::operator ->() const
	{
		return m_ptr;
	}
}
