#pragma once

#include <Windows.h>

namespace hnrt
{
	class ControlIdCollection
	{
	public:

		ControlIdCollection();
		ControlIdCollection(const ControlIdCollection&) = delete;
		virtual ~ControlIdCollection();
		void operator =(const ControlIdCollection&) = delete;
		ControlIdCollection& Add(int id);
		ControlIdCollection& RemoveAll();
		bool Contains(int id);

	private:

		WORD m_count;
		WORD* m_values;
	};
}