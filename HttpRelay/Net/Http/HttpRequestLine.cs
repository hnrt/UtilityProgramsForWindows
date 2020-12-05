namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpRequestLine
    {
        public HttpMethod Method { get; set; }

        public string RequestUri { get; set; }

        public HttpVersion HttpVersion { get; }

        public HttpRequestLine()
        {
            Method = HttpMethod.None;
            RequestUri = null;
            HttpVersion = new HttpVersion();
        }

        public void Clear()
        {
            Method = HttpMethod.None;
            RequestUri = null;
            HttpVersion.Clear();
        }

        public override string ToString()
        {
            return string.Format("{0} {1} HTTP/{2}\r\n", HttpMethodHelper.GetString(Method), RequestUri, HttpVersion.ToString());
        }
    }
}
