#pragma once


#include "hnrt/FileDataFeeder.h"


namespace hnrt
{
	class MyToolbox;

	class MyFileDataFeeder
		: public FileDataFeeder
	{
	public:

		MyFileDataFeeder(MyToolbox&);
		MyFileDataFeeder(const MyFileDataFeeder&) = delete;
		virtual ~MyFileDataFeeder() = default;
		void operator =(const MyFileDataFeeder&) = delete;
		virtual bool HasNext();

	private:

		MyToolbox& m_app;
		ULONGLONG m_LastTick;
	};
}
