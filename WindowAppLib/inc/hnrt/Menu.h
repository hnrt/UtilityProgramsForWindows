#pragma once


#include <Windows.h>


#define HMENU_NULL reinterpret_cast<HMENU>(0)


namespace hnrt
{
	class Menu
	{
	public:

		Menu();
		Menu(HMENU h);
		Menu(HWND hwnd);
		Menu(const Menu&);
		virtual ~Menu() = default;
		void operator =(const Menu&) = delete;
		operator HMENU() const;
		Menu operator [](size_t index) const;
		Menu operator [](PCWSTR psz) const;
		Menu& Set(HMENU);
		Menu& Add(PCWSTR psz, HMENU h);
		Menu& Add(PCWSTR psz, UINT id, UINT flags = 0);
		Menu& AddSeparator();
		Menu& Insert(UINT position, PCWSTR psz, UINT id, UINT flags = 0);
		Menu& InsertSeparator(UINT position);
		Menu& Prepend(PCWSTR psz, UINT id, UINT flags = 0);
		Menu& PrependSeparator();
		Menu& Modify(UINT uPosition, UINT uFlags, UINT_PTR id = 0, PCWSTR psz = nullptr);
		Menu& Enable(UINT uItem, UINT uEnable = MF_BYCOMMAND | MF_ENABLED);
		Menu& RemoveAll();

	protected:

		static HMENU FindMenu(HMENU h, PCWSTR psz);

		HMENU m_h;
	};

	inline Menu::operator HMENU() const
	{
		return m_h;
	}

	inline Menu& Menu::Prepend(PCWSTR psz, UINT id, UINT flags)
	{
		return Insert(0, psz, id, flags);
	}

	inline Menu& Menu::PrependSeparator()
	{
		return InsertSeparator(0);
	}
}
