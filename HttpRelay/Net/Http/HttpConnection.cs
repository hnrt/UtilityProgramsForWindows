namespace hnrt.HttpRelay.Net.Http
{
    internal enum HttpConnection
    {
        None,
        KEEP_ALIVE,
        CLOSE,
        UPGRADE,
        Other
    }
}
