namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpVersion
    {
        public int Major { get; set; }

        public int Minor { get; set; }

        public HttpVersion()
        {
            Clear();
        }

        public void Clear()
        {
            Major = 1;
            Minor = 1;
        }

        public void Set(HttpVersion source)
        {
            Major = source.Major;
            Minor = source.Minor;
        }

        public override string ToString()
        {
            return string.Format("{0}.{1}", Major, Minor);
        }
    }
}
