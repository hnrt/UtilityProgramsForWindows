using System.Text;

namespace hnrt.HttpRelay.Net.Http
{
    internal interface IHttpMessageBody
    {
        byte[] GetBytes(Encoding encoding);
        string GetString(Encoding encoding);
        bool Contains(byte[] value);
        void Replace(byte[] oldValue, byte[] newValue);
    }
}
