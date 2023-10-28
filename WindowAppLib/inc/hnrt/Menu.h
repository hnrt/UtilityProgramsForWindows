#pragma once


#include <Windows.h>


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
		HMENU operator [](size_t index);
		HMENU operator [](PCWSTR psz) const;
		Menu& Add(PCWSTR psz, HMENU h);
		Menu& Add(PCWSTR psz, UINT id, UINT flags = 0);
		Menu& AddSeparator();
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
}
