#pragma once


#include "hnrt/WindowDesign.h"


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
		LONG ArrangePositions(LONG x, LONG y, LONG cx, LONG cy);

		ULONG get_Count() const;
		void put_Padding(const RectangleMetrics&);
		void put_Margin(const RectangleMetrics&);

		__declspec(property(get = get_Count)) ULONG Count;
		__declspec(property(put = put_Padding)) RectangleMetrics& Padding;
		__declspec(property(put = put_Margin)) RectangleMetrics& Margin;

	private:

		ULONG m_cButtons;
		HWND* m_hButtons;
		RectangleMetrics m_Padding;
		RectangleMetrics m_Margin;
	};

	inline ULONG ButtonCollection::get_Count() const
	{
		return m_cButtons;
	}

	inline void ButtonCollection::put_Padding(const RectangleMetrics& value)
	{
		m_Padding = value;
	}

	inline void ButtonCollection::put_Margin(const RectangleMetrics& value)
	{
		m_Margin = value;
	}
}
