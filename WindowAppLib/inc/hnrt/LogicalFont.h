#pragma once


#include <Windows.h>


namespace hnrt
{
	class LogicalFont
	{
	public:

		LogicalFont();
		LogicalFont(const LogicalFont&) = delete;
		~LogicalFont() = default;
		void operator =(const LogicalFont&) = delete;
		HFONT Create();
		LogicalFont& SetHeight(LONG value);
		LogicalFont& SetHeight(int pointSize, HDC hdc);
		LogicalFont& SetHeight(int pointSize, HWND hwnd);
		LogicalFont& SetWidth(LONG value);
		LogicalFont& SetEscapement(LONG value);
		LogicalFont& SetOrientation(LONG value);
		LogicalFont& SetWeight(LONG value); // FW_*
		LogicalFont& SetLightWeight();
		LogicalFont& SetRegularWeight();
		LogicalFont& SetMediumWeight();
		LogicalFont& SetBoldWeight();
		LogicalFont& SetItalic(BYTE value); // TRUE/FALSE
		LogicalFont& SetUnderline(BYTE value); // TRUE/FALSE
		LogicalFont& SetStrikeOut(BYTE value); // TRUE/FALSE
		LogicalFont& SetCharSet(BYTE value); // *_CHARSET
		LogicalFont& SetJapaneseCharSet();
		LogicalFont& SetAnsiCharSet();
		LogicalFont& SetOemCharSet();
		LogicalFont& SetOutPrecision(BYTE value); // OUT_*
		LogicalFont& SetClipPrecision(BYTE value); // CLIP_*
		LogicalFont& SetQuality(BYTE value); // *_QUALITY
		LogicalFont& SetPitchAndFamily(BYTE value); // *_PITCH/FF_*
		LogicalFont& SetFaceName(PCWSTR pszFaceName);

	protected:

		LOGFONTW m_spec;
	};
}
