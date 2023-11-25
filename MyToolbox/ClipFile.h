#pragma once

#include "hnrt/String.h"

namespace hnrt
{
	class FileMapper;

	class ClipFile
	{
	public:

		ClipFile(const String& szPath);
		~ClipFile();
		void Open() const;
		void Close() const;
		void Save(const String& szHash, const String& szContent) const;
		void Save(const String& szHeader, const String& szHash, const String& szContent) const;
		String get_Header() const;
		String get_Hash() const;
		String get_Content() const;

		__declspec(property(get = get_Header)) String Header;
		__declspec(property(get = get_Hash)) String Hash;
		__declspec(property(get = get_Content)) String Content;

	private:

		String m_szPath;
		mutable FileMapper* m_pMapper;
		mutable size_t m_offsets[2];
	};
}
