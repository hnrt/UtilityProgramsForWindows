#pragma once


#include <Windows.h>


namespace hnrt
{
	class ButtonCollection
	{
	public:

		ButtonCollection();
		ButtonCollection(const ButtonCollection&) = delete;
		~ButtonCollection();
		void operator =(const ButtonCollection&) = delete;
		HWND operator [](ULONG index) const;
		void Resize(ULONG count);
		void Add(HINSTANCE hInstance, HWND hwndParent, UINT id, PCWSTR pszLabel, HFONT hFont, bool bVisible);
		void RemoveAll();
		void ArrangePositions(LONG& x, LONG& y, LONG cx, LONG cy, LONG marginTop, LONG marginBottom);

		ULONG get_Count() const;

		__declspec(property(get = get_Count)) ULONG Count;

	private:

		ULONG m_cButtons;
		HWND* m_hButtons;
	};

	inline ULONG ButtonCollection::get_Count() const
	{
		return m_cButtons;
	}
}
