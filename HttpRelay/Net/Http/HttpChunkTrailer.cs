using System.Text;

namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpChunkTrailer
    {
        public HttpHeaderList HeaderList { get; } = new HttpHeaderList();

        public HttpChunkTrailer()
        {
        }

        public void Add(string key, string value)
        {
            HeaderList.Add(key, value);
        }

        public StringBuilder Format(StringBuilder sb, params HttpHeader[] rewrites)
        {
            sb = HeaderList.Format(sb, rewrites);
            sb.Append("\r\n");
            return sb;
        }

        public string GetString()
        {
            return Format(new StringBuilder()).ToString();
        }

        public byte[] GetBytes(Encoding encoding)
        {
            return encoding.GetBytes(GetString());
        }
    }
}
