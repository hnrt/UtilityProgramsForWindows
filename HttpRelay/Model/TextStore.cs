using System;
using System.Text;
using hnrt.HttpRelay.Data;

namespace hnrt.HttpRelay.Model
{
    internal class TextStore : ByteStore, ITextStore
    {
        private Encoding encoding = null;

        private int maxByteCount = 0;

        private byte[] bBuf = null;

        private char[] cBuf = null;

        public Encoding Encoding
        {
            get
            {
                return encoding;
            }
            set
            {
                encoding = value;
                if (encoding != null)
                {
                    maxByteCount = encoding.GetMaxByteCount(1);
                    if (bBuf == null || bBuf.Length < maxByteCount)
                    {
                        bBuf = new byte[maxByteCount];
                    }
                    if (cBuf == null)
                    {
                        cBuf = new char[1];
                    }
                }
            }
        }

        public TextStore()
            : base()
        {
        }

        public int ReadChar()
        {
            int c1 = ReadByte();
            if (c1 == EOS)
            {
                return EOS;
            }
            else if (c1 < 0x80)
            {
                return c1;
            }
            else if (Encoding != null)
            {
                bBuf[0] = (byte)c1;
                for (int i = 1; i < maxByteCount; i++)
                {
                    int c = ReadByte();
                    if (c == EOS)
                    {
                        Unread(bBuf, 1, i - 1);
                        return UnicodeConstants.REPLACEMENT_CHARACTER;
                    }
                    bBuf[i] = (byte)c;
                    try
                    {
                        Encoding.GetChars(bBuf, 0, i + 1, cBuf, 0);
                        if (cBuf[0] == '?' || cBuf[0] == UnicodeConstants.REPLACEMENT_CHARACTER)
                        {
                            // Decoding failed.
                        }
                        else
                        {
                            return cBuf[0];
                        }
                    }
                    catch (Exception)
                    {
                        // Decoding failed.
                    }
                }
                Unread(bBuf, 1, maxByteCount - 1);
                return UnicodeConstants.REPLACEMENT_CHARACTER;
            }
            else
            {
                return UnicodeConstants.REPLACEMENT_CHARACTER;
            }
        }

        public void UnreadChar(int c)
        {
            if (c < 0)
            {
                return;
            }
            var data = Encoding.GetBytes(new char[] { (char)c });
            Unread(data, 0, data.Length);
        }

        public void UnreadChars(int c1, int c2)
        {
            if (c2 < 0)
            {
                if (c1 < 0)
                {
                    return;
                }
                var data1 = Encoding.GetBytes(new char[] { (char)c1 });
                Unread(data1, 0, data1.Length);
                return;
            }
            var data = Encoding.GetBytes(new char[] { (char)c1, (char)c2 });
            Unread(data, 0, data.Length);
        }

        public void UnreadString(string s)
        {
            var data = Encoding.GetBytes(s);
            Unread(data, 0, data.Length);
        }
    }
}
