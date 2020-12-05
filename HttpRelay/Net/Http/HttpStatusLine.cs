namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpStatusLine
    {
        public HttpVersion HttpVersion { get; } = new HttpVersion();

        public int StatusCode
        {
            get
            {
                return statusCode;
            }
            set
            {
                statusCode = value;
                ReasonPhrase = HttpReasonPhrases.Get(value);
            }
        }

        public string ReasonPhrase { get; set; }

        private int statusCode;

        public HttpStatusLine()
        {
            Clear();
        }

        public void Clear()
        {
            HttpVersion.Clear();
            StatusCode = 0;
        }

        public override string ToString()
        {
            return string.Format("HTTP/{0} {1} {2}\r\n", HttpVersion.ToString(), StatusCode, ReasonPhrase);
        }
    }
}
