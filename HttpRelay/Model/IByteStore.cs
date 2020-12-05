using System;

namespace hnrt.HttpRelay.Model
{
    internal interface IByteStore : IDisposable
    {
        bool BlockingMode { get; set; }
        bool CanRead { get; set; }
        int RemainingByteCount { get; }
        void Clear();
        int Read(byte[] buf, int offset, int count);
        int ReadByte();
        void Write(byte[] buf);
        void Write(byte[] buf, int offset, int count);
        void Unread(byte[] buf, int offset, int count);
        byte[] ReadBytes();
    }
}
