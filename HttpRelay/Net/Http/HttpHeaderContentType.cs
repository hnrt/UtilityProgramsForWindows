namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpHeaderContentType : HttpHeader
    {
        public static readonly string NAME = "Content-Type";
        public static readonly string APP_JSON_UTF8 = "application/json; charset=UTF-8";
        public static readonly string TEXT_HTML_UTF8 = "text/html; charset=UTF-8";
        public static readonly string TEXT_PLAIN_UTF8 = "text/plain; charset=UTF-8";

        public MediaType MediaType { get; }

        public string CharSet => MediaType.CharSet;

        public HttpHeaderContentType(string value)
            : base(NAME, value)
        {
            MediaType = MediaType.Parse(value);
        }
    }
}
