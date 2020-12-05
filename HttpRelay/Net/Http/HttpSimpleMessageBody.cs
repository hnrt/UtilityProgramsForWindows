using System.Text;
using hnrt.HttpRelay.Data;

namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpSimpleMessageBody : IHttpMessageBody
    {
        public byte[] Data { get; private set; }

        public HttpSimpleMessageBody(byte[] data)
        {
            Data = data;
        }

        public byte[] GetBytes(Encoding encoding)
        {
            return Data != null ? Data : new byte[0];
        }

        public string GetString(Encoding encoding)
        {
            return Data != null ? encoding.GetString(Data) : "";
        }

        public bool Contains(byte[] value)
        {
            return ByteArrayHelper.IndexOf(Data, value, 0) >= 0;
        }

        public void Replace(byte[] oldValue, byte[] newValue)
        {
            Data = ByteArrayHelper.Replace(Data, oldValue, newValue);
        }
    }
}
