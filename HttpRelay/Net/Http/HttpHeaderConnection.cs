namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpHeaderConnection : HttpHeader
    {
        public static readonly string NAME = "Connection";

        public HttpConnection Connection { get; }

        public HttpHeaderConnection(string value)
            : base(NAME, value)
        {
            var v = value.ToLowerInvariant();
            if (v == "keep-alive")
            {
                Connection = HttpConnection.KEEP_ALIVE;
            }
            else if (v == "close")
            {
                Connection = HttpConnection.CLOSE;
            }
            else if (v == "upgrade")
            {
                Connection = HttpConnection.UPGRADE;
            }
            else
            {
                Connection = HttpConnection.Other;
            }
        }
    }
}
