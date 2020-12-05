using System;
using System.Collections.Generic;
using System.Text;

namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpChunkedMessageBody : IHttpMessageBody
    {
        public List<HttpChunk> BlockList { get; } = new List<HttpChunk>();

        public HttpChunk Current { get; private set; }

        public HttpChunkTrailer Trailer { get; } = new HttpChunkTrailer();

        public HttpChunkedMessageBody()
        {
        }

        public void Add()
        {
            Current = new HttpChunk();
            BlockList.Add(Current);
        }

        public byte[] GetBytes(Encoding encoding)
        {
            var list = new List<byte[]>();
            bool hasEnd = false;
            int length = 0;
            foreach (var block in BlockList)
            {
                var data = block.GetBytes(encoding);
                list.Add(data);
                length += data.Length;
                if (block.Data == null)
                {
                    hasEnd = true;
                    break;
                }
            }
            if (!hasEnd)
            {
                var data = (new HttpChunk()).GetBytes(encoding);
                list.Add(data);
                length += data.Length;
            }
            var tb = Trailer.GetBytes(encoding);
            list.Add(tb);
            length += tb.Length;
            //
            // all parts are prepared; start building bytes to return
            //
            var dst = new byte[length];
            int index = 0;
            foreach (var src in list)
            {
                Buffer.BlockCopy(src, 0, dst, index, src.Length);
                index += src.Length;
            }
            return dst;
        }

        public string GetString(Encoding encoding)
        {
            var sb = new StringBuilder();
            bool hasEnd = false;
            foreach (var block in BlockList)
            {
                sb.Append(block.GetString(encoding));
                if (block.Data == null)
                {
                    hasEnd = true;
                    break;
                }
            }
            if (!hasEnd)
            {
                sb.Append((new HttpChunk()).GetString(encoding));
            }
            Trailer.Format(sb);
            return sb.ToString();
        }

        public bool Contains(byte[] value)
        {
            foreach (var block in BlockList)
            {
                if (block.IndexOf(value, 0) >= 0)
                {
                    return true;
                }
            }
            return false;
        }

        public void Replace(byte[] oldValue, byte[] newValue)
        {
            foreach (var block in BlockList)
            {
                block.Replace(oldValue, newValue);
            }
        }
    }
}
