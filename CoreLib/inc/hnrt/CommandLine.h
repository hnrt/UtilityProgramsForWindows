#pragma once


#include <Windows.h>


namespace hnrt
{
	class CommandLine
	{
	public:

		CommandLine(PCWSTR pszCmdLine);
		CommandLine(const CommandLine&) = delete;
		virtual ~CommandLine();
		void operator =(const CommandLine&) = delete;
		PCWSTR operator [](int index) const;
		void Remove(int index);

		int get_Count() const;

		__declspec(property(get = get_Count)) int Count;

	protected:

		PWSTR* m_ppsz;
		int m_count;
	};

	class CommandLineIterator
	{
	public:

		CommandLineIterator(CommandLine& cmdLine);
		CommandLineIterator(const CommandLineIterator&) = delete;
		~CommandLineIterator() = default;
		void operator =(const CommandLineIterator&) = delete;
		void RemoveNext();

		bool get_HasNext();
		PCWSTR get_Next() const;

		__declspec(property(get = get_HasNext)) bool HasNext;
		__declspec(property(get = get_Next)) PCWSTR Next;

	private:

		CommandLine& m_CommandLine;
		int m_index;
	};

	inline int CommandLine::get_Count() const
	{
		return m_count;
	}
}
