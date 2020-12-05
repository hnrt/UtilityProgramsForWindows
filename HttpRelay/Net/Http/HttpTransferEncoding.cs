namespace hnrt.HttpRelay.Net.Http
{
    internal enum HttpTransferEncoding
    {
        CHUNKED,
        IDENTITY,
        GZIP,
        COMPRESS,
        DEFLATE,
        Other
    }
}
