#pragma once

#include "hnrt/DataFeeder.h"

namespace hnrt
{
	class NoDataFeeder
		: public DataFeeder
	{
	public:

		NoDataFeeder();
		NoDataFeeder(const NoDataFeeder&) = delete;
		virtual ~NoDataFeeder() = default;
		void operator =(const NoDataFeeder&) = delete;
		virtual bool HasNext();
	};

	inline NoDataFeeder::NoDataFeeder()
		: DataFeeder(0)
	{
	}

	inline bool NoDataFeeder::HasNext()
	{
		return false;
	}
}