#include "pch.h"
#include "hnrt/LogicalFont.h"


using namespace hnrt;


LogicalFont::LogicalFont()
	: m_spec()
{
	m_spec.lfCharSet = DEFAULT_CHARSET;
}


HFONT LogicalFont::Create()
{
	return CreateFontIndirectW(&m_spec);
}


LogicalFont& LogicalFont::SetHeight(LONG value)
{
	m_spec.lfHeight = value;
	return *this;
}


LogicalFont& LogicalFont::SetHeight(int pointSize, HDC hdc)
{
	m_spec.lfHeight = -MulDiv(pointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	return *this;
}


LogicalFont& LogicalFont::SetHeight(int pointSize, HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	m_spec.lfHeight = -MulDiv(pointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(hwnd, hdc);
	return *this;
}


LogicalFont& LogicalFont::SetWidth(LONG value)
{
	m_spec.lfWidth = value;
	return *this;
}


LogicalFont& LogicalFont::SetEscapement(LONG value)
{
	m_spec.lfEscapement = value;
	return *this;
}


LogicalFont& LogicalFont::SetOrientation(LONG value)
{
	m_spec.lfOrientation = value;
	return *this;
}


LogicalFont& LogicalFont::SetWeight(LONG value)
{
	m_spec.lfWeight = value;
	return *this;
}


LogicalFont& LogicalFont::SetLightWeight()
{
	m_spec.lfWeight = FW_LIGHT;
	return *this;
}


LogicalFont& LogicalFont::SetRegularWeight()
{
	m_spec.lfWeight = FW_REGULAR;
	return *this;
}


LogicalFont& LogicalFont::SetMediumWeight()
{
	m_spec.lfWeight = FW_MEDIUM;
	return *this;
}


LogicalFont& LogicalFont::SetBoldWeight()
{
	m_spec.lfWeight = FW_BOLD;
	return *this;
}


LogicalFont& LogicalFont::SetItalic(BYTE value)
{
	m_spec.lfItalic = value;
	return *this;
}


LogicalFont& LogicalFont::SetUnderline(BYTE value)
{
	m_spec.lfUnderline = value;
	return *this;
}


LogicalFont& LogicalFont::SetStrikeOut(BYTE value)
{
	m_spec.lfStrikeOut = value;
	return *this;
}


LogicalFont& LogicalFont::SetCharSet(BYTE value)
{
	m_spec.lfCharSet = value;
	return *this;
}


LogicalFont& LogicalFont::SetJapaneseCharSet()
{
	m_spec.lfCharSet = SHIFTJIS_CHARSET;
	return *this;
}


LogicalFont& LogicalFont::SetAnsiCharSet()
{
	m_spec.lfCharSet = ANSI_CHARSET;
	return *this;
}


LogicalFont& LogicalFont::SetOemCharSet()
{
	m_spec.lfCharSet = OEM_CHARSET;
	return *this;
}


LogicalFont& LogicalFont::SetOutPrecision(BYTE value)
{
	m_spec.lfOutPrecision = value;
	return *this;
}


LogicalFont& LogicalFont::SetClipPrecision(BYTE value)
{
	m_spec.lfClipPrecision = value;
	return *this;
}


LogicalFont& LogicalFont::SetQuality(BYTE value)
{
	m_spec.lfQuality = value;
	return *this;
}


LogicalFont& LogicalFont::SetPitchAndFamily(BYTE value)
{
	m_spec.lfPitchAndFamily = value;
	return *this;
}


LogicalFont& LogicalFont::SetFaceName(PCWSTR pszFaceName)
{
	wcscpy_s(m_spec.lfFaceName, pszFaceName);
	return *this;
}
