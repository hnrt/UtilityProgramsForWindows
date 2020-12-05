using System;

namespace hnrt.HttpRelay.Data
{
    internal static class ByteArrayHelper
    {
        public static int IndexOf(byte[] data1, byte[] data2, int startIndex)
        {
            if (data1 != null && data2 != null)
            {
                int n = data1.Length - data2.Length;
                for (int i = startIndex; i <= n; i++)
                {
                    if (data1[i] == data2[0])
                    {
                        int j = 0;
                        int k = data2.Length;
                        do
                        {
                            if (++j == k)
                            {
                                return i;
                            }
                        }
                        while (data1[i + j] == data2[j]);
                    }
                }
            }
            return -1;
        }

        public static byte[] Replace(byte[] data, byte[] oldValue, byte[] newValue)
        {
            int index = IndexOf(data, oldValue, 0);
            if (index >= 0)
            {
                int[] found = new int[data.Length / oldValue.Length];
                int count = 0;
                do
                {
                    found[count++] = index;
                    index = IndexOf(data, oldValue, index + oldValue.Length);
                }
                while (index >= 0);
                int length = data.Length + (newValue.Length - oldValue.Length) * count;
                byte[] data2 = new byte[length];
                int srcIndex1 = 0;
                int dstIndex = 0;
                for (index = 0; index < count; index++)
                {
                    int srcIndex2 = found[index];
                    if (srcIndex1 < srcIndex2)
                    {
                        int delta = srcIndex2 - srcIndex1;
                        Buffer.BlockCopy(data, srcIndex1, data2, dstIndex, delta);
                        srcIndex1 += delta;
                        dstIndex += delta;
                    }
                    srcIndex1 += oldValue.Length;
                    Buffer.BlockCopy(newValue, 0, data2, dstIndex, newValue.Length);
                    dstIndex += newValue.Length;
                }
                if (srcIndex1 < data.Length)
                {
                    Buffer.BlockCopy(data, srcIndex1, data2, dstIndex, data.Length - srcIndex1);
                }
                return data2;
            }
            else
            {
                return data;
            }
        }
    }
}
