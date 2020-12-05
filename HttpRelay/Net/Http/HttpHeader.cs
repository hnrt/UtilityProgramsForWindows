namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpHeader
    {
        public string Name { get; }

        public string Value { get; }

        public HttpHeader Next { get; set; }

        public HttpHeader(string name, string value)
        {
            Name = name;
            Value = value;
        }
    }
}
