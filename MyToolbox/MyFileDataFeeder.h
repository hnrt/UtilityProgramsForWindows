#pragma once


#include "hnrt/FileDataFeeder.h"


namespace hnrt
{
	class MyFileDataFeeder
		: public FileDataFeeder
	{
	public:

		MyFileDataFeeder();
		MyFileDataFeeder(const MyFileDataFeeder&) = delete;
		virtual ~MyFileDataFeeder() = default;
		void operator =(const MyFileDataFeeder&) = delete;
		virtual bool HasNext();

	private:

		ULONGLONG m_LastTick;
	};
}
