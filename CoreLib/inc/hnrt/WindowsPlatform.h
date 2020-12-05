#pragma once


#include <Windows.h>


namespace hnrt
{
    class WindowsPlatform
    {
    public:

        static bool Is64bit();
        static bool Is32bit();
        static bool Is64bitProcess();
        static bool Is32bitProcess();
        static bool IsWow64Process();
        static DWORD Is64bitProcess(HANDLE, bool&);
        static DWORD Is32bitProcess(HANDLE, bool&);
        static DWORD IsWow64Process(HANDLE, bool&);

        /// <summary>
        /// Returns the bitness of the platform.
        /// </summary>
        /// <returns>64 if the platform is of 64-bit. Otherwise, 32.</returns>
        static int Bitness();

        /// <summary>
        /// Returns the bitness of the current process.
        /// </summary>
        /// <returns>64 if the current process is of 64-bit. Otherwise, 32.</returns>
        static int ProcessBitness();

        /// <summary>
        /// Finds the bitness of the process specified by the process ID.
        /// </summary>
        /// <param name="">Process ID that specifies the process to be checked.</param>
        /// <returns>Either 64 or 32 will be returned according to its bitness. 0 will be returned on error.</returns>
        static int ProcessBitness(DWORD);
    };
}
