namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpHeaderUpgrade : HttpHeader
    {
        public static readonly string NAME = "Upgrade";

        public HttpUpgrade Upgrade { get; }

        public HttpHeaderUpgrade(string value)
            : base(NAME, value)
        {
            Upgrade = HttpUpgradeHelper.FromString(value);
        }
    }
}
