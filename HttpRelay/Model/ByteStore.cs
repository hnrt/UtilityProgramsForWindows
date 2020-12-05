using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace hnrt.HttpRelay.Model
{
    internal class ByteStore : IByteStore
    {
        public static readonly int EOS = -1;

        private static readonly int TIMEOUT = 1000; // milliseconds

        private readonly object lockObject = new object();

        private readonly Queue<byte[]> queue = new Queue<byte[]>();

        private readonly SemaphoreSlim semaphore = new SemaphoreSlim(0, int.MaxValue);

        private byte[] buffer = null;

        private int index = -1;

        public bool BlockingMode { get; set; } = true;

        public bool CanRead { get; set; } = true;

        public int RemainingByteCount
        {
            get
            {
                lock (lockObject)
                {
                    return GetRemainingByteCount();
                }
            }
        }

        public ByteStore()
        {
        }

        /// <summary>
        /// Frees the internal semaphore.
        /// </summary>
        public void Dispose()
        {
            semaphore.Dispose();
        }

        /// <summary>
        /// Discards all the buffered data and resets the internal parameters.
        /// </summary>
        public void Clear()
        {
            lock (lockObject)
            {
                while (semaphore.Wait(0))
                {
                    continue;
                }
                queue.Clear();
                buffer = null;
                index = 0;
                CanRead = true;
            }
        }

        /// <summary>
        /// Reads the bytes from the buffered data.
        /// </summary>
        /// <param name="buf">The byte array in which the data is to be stored.</param>
        /// <param name="offset">Index from which the data is to be stored.</param>
        /// <param name="count">Maximum number of bytes to be stored.</param>
        /// <returns>Number of bytes stored in the designated buffer. -1 on the end of stream.</returns>
        public int Read(byte[] buf, int offset, int count)
        {
            while (CanRead)
            {
                lock (lockObject)
                {
                    if (buffer != null && index < buffer.Length)
                    {
                        int lengthMax = buffer.Length - index;
                        int length = count < lengthMax ? count : lengthMax;
                        Buffer.BlockCopy(buffer, index, buf, offset, length);
                        index += length;
                        return length;
                    }
                }
                if (!UpdateBuffer() && !BlockingMode)
                {
                    return 0;
                }
            }
            return EOS;
        }

        /// <summary>
        /// Returns one byte that read from the buffered data. 
        /// </summary>
        /// <returns></returns>
        public int ReadByte()
        {
            while (CanRead)
            {
                if (buffer != null && index < buffer.Length)
                {
                    return buffer[index++];
                }
                if (!UpdateBuffer() && !BlockingMode)
                {
                    return 0;
                }
            }
            return EOS;
        }

        private bool UpdateBuffer()
        {
            if (semaphore.Wait(BlockingMode ? TIMEOUT : 0))
            {
                lock (lockObject)
                {
                    buffer = queue.Dequeue();
                    index = 0;
                    if (buffer.Length == 0 && queue.Count == 0)
                    {
                        CanRead = false;
                    }
                }
                return true;
            }
            else
            {
                return false;
            }
        }

        public void Write(byte[] buf)
        {
            Write(buf, 0, buf.Length);
        }

        public void Write(byte[] buf, int offset, int count)
        {
            var data = new byte[count];
            Buffer.BlockCopy(buf, offset, data, 0, count);
            lock (lockObject)
            {
                if (CanRead || count > 0)
                {
                    queue.Enqueue(data);
                    semaphore.Release();
                    CanRead = true;
                }
            }
        }

        /// <summary>
        /// Stores the byte data back at the beginning of the internal buffer.
        /// </summary>
        /// <param name="buf"></param>
        /// <param name="offset"></param>
        /// <param name="count"></param>
        public void Unread(byte[] buf, int offset, int count)
        {
            if (count > 0)
            {
                lock (lockObject)
                {
                    if (count <= index)
                    {
                        index -= count;
                        Buffer.BlockCopy(buf, offset, buffer, index, count);
                    }
                    else if (buffer != null)
                    {
                        int unreadBytes = buffer.Length - index;
                        var buf2 = new byte[count + unreadBytes];
                        Buffer.BlockCopy(buf, offset, buf2, 0, count);
                        Buffer.BlockCopy(buffer, index, buf2, count, unreadBytes);
                        buffer = buf2;
                        index = 0;
                    }
                    else
                    {
                        buffer = new byte[count];
                        Buffer.BlockCopy(buf, offset, buffer, 0, count);
                        index = 0;
                    }
                }
            }
        }

        /// <summary>
        /// Reads all the byte data stored in this object.
        /// </summary>
        /// <returns>Byte array in which the data are stored.</returns>
        public byte[] ReadBytes()
        {
            lock (lockObject)
            {
                int length = GetRemainingByteCount();
                var data = new byte[length];
                length = 0;
                if (buffer != null)
                {
                    var count = buffer.Length - index;
                    Buffer.BlockCopy(buffer, index, data, length, count);
                    length += count;
                }
                foreach (var item in queue)
                {
                    Buffer.BlockCopy(item, 0, data, length, item.Length);
                    length += item.Length;
                }
                Clear();
                return data;
            }
        }

        protected int GetRemainingByteCount()
        {
            int count = (buffer != null) ? (buffer.Length - index) : 0;
            foreach (var item in queue)
            {
                count += item.Length;
            }
            return count;
        }
    }
}
