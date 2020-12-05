namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpHeaderHost : HttpHeader
    {
        public static readonly string NAME = "Host";

        public HttpHeaderHost(string value)
            : base(NAME, value)
        {
        }

        public HttpHeaderHost(string host, int port, bool isSecureConnection)
            : this(port == HttpConstants.GetDefaultPort(isSecureConnection)
                  ? host
                  : string.Format("{0}:{1}", host, port))
        {
        }
    }
}
