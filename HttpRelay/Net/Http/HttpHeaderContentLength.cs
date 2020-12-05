namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpHeaderContentLength : HttpHeader
    {
        public static readonly string NAME = "Content-Length";

        public int ContentLength { get; }

        public HttpHeaderContentLength(int value)
            : base(NAME, string.Format("{0}", value))
        {
            ContentLength = value;
        }
    }
}
