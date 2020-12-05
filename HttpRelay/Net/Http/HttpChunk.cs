using System;
using System.Collections.Generic;
using System.Text;
using hnrt.HttpRelay.Data;

namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpChunk
    {
        public static readonly byte[] CRLF = { 0x0d, 0x0a };

        public List<(string Name, string Value)> Parameters { get; } = new List<(string Name, string Value)>();

        private byte[] data = null;

        public byte[] Data
        {
            get
            {
                return data;
            }
            set
            {
                if (value != null && value.Length > 0)
                {
                    data = value;
                }
                else
                {
                    data = null;
                }
            }
        }

        public HttpChunk()
        {
        }

        public void AddParameter(string key, string value)
        {
            Parameters.Add((key, value));
        }

        public byte[] GetBytes(Encoding encoding)
        {
            byte[] value;
            var sb = new StringBuilder();
            if (Data != null)
            {
                sb.AppendFormat("{0:x}", Data.Length);
                foreach (var parameter in Parameters)
                {
                    if (parameter.Value != null)
                    {
                        sb.AppendFormat(";{0}={1}", parameter.Name, parameter.Value);
                    }
                    else
                    {
                        sb.AppendFormat(";{0}", parameter.Name);
                    }
                }
                var header = encoding.GetBytes(sb.ToString());
                value = new byte[header.Length + CRLF.Length + Data.Length + CRLF.Length];
                int index = 0;
                Buffer.BlockCopy(header, 0, value, index, header.Length);
                index += header.Length;
                Buffer.BlockCopy(CRLF, 0, value, index, CRLF.Length);
                index += CRLF.Length;
                Buffer.BlockCopy(Data, 0, value, index, Data.Length);
                index += Data.Length;
                Buffer.BlockCopy(CRLF, 0, value, index, CRLF.Length);
            }
            else
            {
                sb.Append("0");
                foreach (var parameter in Parameters)
                {
                    if (parameter.Value != null)
                    {
                        sb.AppendFormat(";{0}={1}", parameter.Name, parameter.Value);
                    }
                    else
                    {
                        sb.AppendFormat(";{0}", parameter.Name);
                    }
                }
                var header = encoding.GetBytes(sb.ToString());
                value = new byte[header.Length + CRLF.Length];
                Buffer.BlockCopy(header, 0, value, 0, header.Length);
                Buffer.BlockCopy(CRLF, 0, value, header.Length, CRLF.Length);
            }
            return value;
        }

        public string GetString(Encoding encoding)
        {
            var sb = new StringBuilder();
            if (Data != null)
            {
                sb.AppendFormat("{0:x}", Data.Length);
                foreach (var parameter in Parameters)
                {
                    if (parameter.Value != null)
                    {
                        sb.AppendFormat(";{0}={1}", parameter.Name, parameter.Value);
                    }
                    else
                    {
                        sb.AppendFormat(";{0}", parameter.Name);
                    }
                }
                sb.Append("\r\n");
                sb.Append(encoding.GetString(Data));
                sb.Append("\r\n");
            }
            else
            {
                sb.Append("0");
                foreach (var parameter in Parameters)
                {
                    if (parameter.Value != null)
                    {
                        sb.AppendFormat(";{0}={1}", parameter.Name, parameter.Value);
                    }
                    else
                    {
                        sb.AppendFormat(";{0}", parameter.Name);
                    }
                }
                sb.Append("\r\n");
            }
            return sb.ToString();
        }

        public int IndexOf(byte[] value, int startIndex)
        {
            return ByteArrayHelper.IndexOf(Data, value, startIndex);
        }

        public void Replace(byte[] oldValue, byte[] newValue)
        {
            Data = ByteArrayHelper.Replace(Data, oldValue, newValue);
        }
    }
}
