#pragma once

namespace hnrt
{
	class Hook
	{
	public:

		static Hook& Instance();

		Hook() = default;
		Hook(const Hook&) = delete;
		virtual ~Hook() = default;
		void operator =(const Hook&) = delete;
		virtual void SetTimeDisplacement(LONGLONG) = 0;
		virtual void SetDate(int, int, int) = 0;
	};
}
