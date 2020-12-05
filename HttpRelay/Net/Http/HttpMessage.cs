using System;
using System.Text;
using hnrt.HttpRelay.Model;

namespace hnrt.HttpRelay.Net.Http
{
    internal abstract class HttpMessage
    {
        public static readonly string ERROR_MISSING_HTTPVERSION = "Missing HTTP version.";
        public static readonly string ERROR_MALFORMED_HTTPVERSION = "Malformed HTTP version.";
        public static readonly string ERROR_MALFORMED_HEADER = "Malformed {0} header.";
        public static readonly string ERROR_MALFORMED_REQUESTURI = "Malformed request URI.";
        public static readonly string ERROR_MISSING_TOKEN = "Missing token.";
        public static readonly string ERROR_MISSING_SP = "Missing SP.";
        public static readonly string ERROR_MISSING_CRLF = "Missing CRLF.";
        public static readonly string ERROR_MISSING_LF = "Missing LF.";
        public static readonly string ERROR_MALFORMED_QUOTEDSTRING = "Malformed quoated string.";
        public static readonly string ERROR_MALFORMED_QUOTEDPAIR = "Malformed quoated pair.";
        public static readonly string ERROR_UNSUPPORTED_TRANSFER_ENCODING = "Unsupported transfer encoding.";
        public static readonly string ERROR_MALFORMED_CHUNKED = "Malformed chunked message body.";
        public static readonly string ERROR_MISSING_CHUNK_EXT_NAME = "Missing chunk ext name.";
        public static readonly string ERROR_MISSING_CHUNK_EXT_VALUE = "Missing chunk ext value.";

        protected string Name { get; }

        protected ITextStore TextStore { get; }

        protected int c;

        protected StringBuilder sb = new StringBuilder();

        public abstract HttpMethod Method { get; set; }

        public abstract HttpVersion ProtocolVersion { get; }

        public HttpHeaderList HeaderList { get; } = new HttpHeaderList();

        public byte[] Header => GetHeader();

        public IHttpMessageBody MessageBody { get; set; }

        public Action OnReceiveHeader { get; set; }

        public Action OnReceiveBody { get; set; }

        public Action<HttpChunk> OnReceiveChunk { get; set; }

        public Action<HttpChunkTrailer> OnReceiveChunkTrailer { get; set; }

        public Encoding Encoding => TextStore.Encoding;

        public bool IsChunked
        {
            get
            {
                var te = HeaderList.TransferEncoding;
                return te != null && te.IsChunked;
            }
        }

        public int RemainingByteCount => TextStore.RemainingByteCount;

        protected HttpMessage(string name, ITextStore textStore)
        {
            Name = name;
            TextStore = textStore;
        }

        protected void ParseSP()
        {
            if (c == ' ')
            {
                c = TextStore.ReadChar();
            }
            else
            {
                throw new Exception(ERROR_MISSING_SP);
            }
        }

        protected void ParseCRLF()
        {
            ParseCRLF(true);
        }

        protected void ParseCRLF(bool bReadNext)
        {
            if (c == '\r')
            {
                c = TextStore.ReadChar();
            }
            else
            {
                throw new Exception(ERROR_MISSING_CRLF);
            }
            if (c == '\n')
            {
                if (bReadNext)
                {
                    c = TextStore.ReadChar();
                }
            }
            else
            {
                throw new Exception(ERROR_MISSING_LF);
            }
        }

        protected void ParseHttpVersion()
        {
            if (c == 'H')
            {
                c = TextStore.ReadChar();
            }
            else
            {
                throw new Exception(ERROR_MISSING_HTTPVERSION);
            }
            if (c == 'T')
            {
                c = TextStore.ReadChar();
            }
            else
            {
                throw new Exception(ERROR_MISSING_HTTPVERSION);
            }
            if (c == 'T')
            {
                c = TextStore.ReadChar();
            }
            else
            {
                throw new Exception(ERROR_MISSING_HTTPVERSION);
            }
            if (c == 'P')
            {
                c = TextStore.ReadChar();
            }
            else
            {
                throw new Exception(ERROR_MISSING_HTTPVERSION);
            }
            if (c == '/')
            {
                c = TextStore.ReadChar();
            }
            else
            {
                throw new Exception(ERROR_MALFORMED_HTTPVERSION);
            }
            if (ParseNumber())
            {
                ProtocolVersion.Major = int.Parse(sb.ToString());
            }
            else
            {
                throw new Exception(ERROR_MALFORMED_HTTPVERSION);
            }
            if (c == '.')
            {
                c = TextStore.ReadChar();
            }
            else
            {
                throw new Exception(ERROR_MALFORMED_HTTPVERSION);
            }
            if (ParseNumber())
            {
                ProtocolVersion.Minor = int.Parse(sb.ToString());
            }
            else
            {
                throw new Exception(ERROR_MALFORMED_HTTPVERSION);
            }
        }

        protected bool ParseHeader()
        {
            var name = ParseFieldName();
            if (name == null)
            {
                return false;
            }
            if (c == ':')
            {
                c = TextStore.ReadChar();
            }
            else
            {
                throw new Exception(string.Format(ERROR_MALFORMED_HEADER, Name));
            }
            try
            {
                var value = ParseFieldValue();
                ParseCRLF();
                var h = HeaderList.Add(name, value.Trim());
                if (h.Name == HttpHeaderContentType.NAME)
                {
                    CheckEncoding(((HttpHeaderContentType)h).CharSet);
                }
            }
            catch (Exception ex)
            {
                throw new Exception(string.Format(ERROR_MALFORMED_HEADER, Name), ex);
            }
            return true;
        }

        private void CheckEncoding(string cs)
        {
            if (cs != null)
            {
                try
                {
                    TextStore.Encoding = Encoding.GetEncoding(cs);
                }
                catch (Exception)
                {
                }
            }
        }

        protected void ParseMessageBody()
        {
            MessageBody = null;
            if (CanHaveMessageBody())
            {
                HttpHeaderTransferEncoding te = HeaderList.TransferEncoding;
                if (te != null)
                {
                    if (te.IsChunked)
                    {
                        ParseChunkedBody();
                    }
                    else
                    {
                        throw new NotImplementedException(ERROR_UNSUPPORTED_TRANSFER_ENCODING);
                    }
                }
                else
                {
                    int length = HeaderList.ContentLength;
                    if (length == 0)
                    {
                        // no message-body assumed
                    }
                    else if (length > 0)
                    {
                        MessageBody = new HttpSimpleMessageBody(ReadBySize(length));
                    }
                    else
                    {
                        MessageBody = new HttpSimpleMessageBody(ReadToEOF());
                    }
                    OnReceiveBody?.Invoke();
                }
            }
            else
            {
                OnReceiveBody?.Invoke();
            }
        }

        protected abstract bool CanHaveMessageBody();

        private void ParseChunkedBody()
        {
            MessageBody = new HttpChunkedMessageBody();
            c = TextStore.ReadChar();
            while (true)
            {
                var chunkSize = ParseChunkSize();
                ParseChunkExtension();
                ParseChunkCRLF(false);
                if (chunkSize == 0)
                {
                    OnReceiveChunk?.Invoke(((HttpChunkedMessageBody)MessageBody).Current);
                    break;
                }
                ParseChunkData(chunkSize);
                c = TextStore.ReadChar();
                ParseChunkCRLF(true);
                OnReceiveChunk?.Invoke(((HttpChunkedMessageBody)MessageBody).Current);
            }
            c = TextStore.ReadChar();
            ParseChunkTrailer();
            ParseChunkCRLF(false);
            OnReceiveChunkTrailer?.Invoke(((HttpChunkedMessageBody)MessageBody).Trailer);
        }

        private int ParseChunkSize()
        {
            if (ParseHexNumber())
            {
                try
                {
                    var hex = sb.ToString();
                    var chunkSize = int.Parse(hex, System.Globalization.NumberStyles.HexNumber);
                    ((HttpChunkedMessageBody)MessageBody).Add();
                    return chunkSize;
                }
                catch (Exception ex)
                {
                    throw new Exception(ERROR_MALFORMED_CHUNKED, ex);
                }
            }
            else
            {
                throw new Exception(ERROR_MALFORMED_CHUNKED);
            }
        }

        private void ParseChunkExtension()
        {
            while (c == ';')
            {
                c = TextStore.ReadChar();
                string name = ParseChunkExtensionName();
                string value;
                if (c == '=')
                {
                    c = TextStore.ReadChar();
                    value = ParseChunkExtensionValue();
                }
                else
                {
                    value = null;
                }
                ((HttpChunkedMessageBody)MessageBody).Current.AddParameter(name, value);
            }
        }

        protected string ParseChunkExtensionName()
        {
            if (ParseToken())
            {
                return sb.ToString();
            }
            else
            {
                throw new Exception(ERROR_MALFORMED_CHUNKED, new Exception(ERROR_MISSING_CHUNK_EXT_NAME));
            }
        }

        protected string ParseChunkExtensionValue()
        {
            if (ParseQuotedString()
                || ParseToken())
            {
                return sb.ToString();
            }
            else
            {
                throw new Exception(ERROR_MALFORMED_CHUNKED, new Exception(ERROR_MISSING_CHUNK_EXT_VALUE));
            }
        }

        private void ParseChunkCRLF(bool bReadNext)
        {
            ParseCRLF(bReadNext);
        }

        private void ParseChunkData(int chunkSize)
        {
            ((HttpChunkedMessageBody)MessageBody).Current.Data = ReadBySize(chunkSize);
        }

        private void ParseChunkTrailer()
        {
            while (true)
            {
                var name = ParseFieldName();
                if (name == null)
                {
                    return;
                }
                if (c == ':')
                {
                    c = TextStore.ReadChar();
                }
                else
                {
                    throw new Exception(string.Format(ERROR_MALFORMED_HEADER, Name));
                }
                try
                {
                    var value = ParseFieldValue();
                    ParseCRLF();
                    ((HttpChunkedMessageBody)MessageBody).Trailer.Add(name, value.Trim());
                }
                catch (Exception ex)
                {
                    throw new Exception(string.Format(ERROR_MALFORMED_HEADER, Name), ex);
                }
            }
        }

        protected string ParseFieldName()
        {
            if (ParseToken())
            {
                return sb.ToString();
            }
            else
            {
                return null;
            }
        }

        protected string ParseFieldValue()
        {
            sb.Clear();
            while (true)
            {
                if (c == '\"')
                {
                    sb.Append('\"');
                    c = TextStore.ReadChar();
                    while (c != '\"')
                    {
                        if (ParseQuotedPair())
                        {
                            continue;
                        }
                        else if (ParseText())
                        {
                            continue;
                        }
                        else
                        {
                            throw new Exception(ERROR_MALFORMED_QUOTEDSTRING);
                        }
                    }
                    sb.Append('\"');
                    c = TextStore.ReadChar();
                }
                else if (ParseText())
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            return sb.ToString();
        }

        protected bool ParseToken()
        {
            if (HttpChar.IsChar(c) && !HttpChar.IsControl(c) && !HttpChar.IsSeparator(c))
            {
                sb.Clear();
                sb.Append((char)c);
                c = TextStore.ReadChar();
            }
            else
            {
                return false;
            }
            while (HttpChar.IsChar(c) && !HttpChar.IsControl(c) && !HttpChar.IsSeparator(c))
            {
                sb.Append((char)c);
                c = TextStore.ReadChar();
            }
            return true;
        }

        protected bool ParseString()
        {
            if (HttpChar.IsChar(c) && !HttpChar.IsControl(c) && c != HttpChar.SP)
            {
                sb.Clear();
                sb.Append((char)c);
                c = TextStore.ReadChar();
            }
            else
            {
                return false;
            }
            while (HttpChar.IsChar(c) && !HttpChar.IsControl(c) && c != HttpChar.SP)
            {
                sb.Append((char)c);
                c = TextStore.ReadChar();
            }
            return true;
        }

        protected bool ParseNumber()
        {
            if (HttpChar.IsDigit(c))
            {
                sb.Clear();
                sb.Append((char)c);
                c = TextStore.ReadChar();
            }
            else
            {
                return false;
            }
            while (HttpChar.IsDigit(c))
            {
                sb.Append((char)c);
                c = TextStore.ReadChar();
            }
            return true;
        }

        protected bool ParseHexNumber()
        {
            if (HttpChar.IsHexDigit(c))
            {
                sb.Clear();
                sb.Append((char)c);
                c = TextStore.ReadChar();
            }
            else
            {
                return false;
            }
            while (HttpChar.IsHexDigit(c))
            {
                sb.Append((char)c);
                c = TextStore.ReadChar();
            }
            return true;
        }

        protected bool ParseQuotedString()
        {
            if (c == '\"')
            {
                sb.Clear();
                sb.Append('\"');
                c = TextStore.ReadChar();
            }
            else
            {
                return false;
            }
            while (c != '\"')
            {
                if (ParseQuotedPair())
                {
                    continue;
                }
                else if (ParseText())
                {
                    continue;
                }
                else
                {
                    throw new Exception(ERROR_MALFORMED_QUOTEDSTRING);
                }
            }
            sb.Append('\"');
            c = TextStore.ReadChar();
            return true;
        }

        protected bool ParseQuotedPair()
        {
            if (c == '\\')
            {
                c = TextStore.ReadChar();
            }
            else
            {
                return false;
            }
            if (HttpChar.IsChar(c))
            {
                sb.Append('\\');
                sb.Append((char)c);
                c = TextStore.ReadChar();
                return true;
            }
            else
            {
                throw new Exception(ERROR_MALFORMED_QUOTEDPAIR);
            }
        }

        protected bool ParseText()
        {
            if (ParseLinearWhiteSpace())
            {
                return true;
            }
            else if (HttpChar.IsControl(c))
            {
                return false;
            }
            else
            {
                sb.Append((char)c);
                c = TextStore.ReadChar();
                return true;
            }
        }

        protected bool ParseLinearWhiteSpace()
        {
            if (c == HttpChar.SP || c == HttpChar.HT)
            {
                sb.Append((char)c);
                c = TextStore.ReadChar();
                return true;
            }
            else if (c == HttpChar.CR)
            {
                c = TextStore.ReadChar();
                if (c == HttpChar.LF)
                {
                    c = TextStore.ReadChar();
                }
                else
                {
                    TextStore.UnreadChar(c);
                    c = HttpChar.CR;
                    return false;
                }
                if (c == HttpChar.SP || c == HttpChar.HT)
                {
                    sb.Append('\r');
                    sb.Append('\n');
                    sb.Append((char)c);
                    c = TextStore.ReadChar();
                    return true;
                }
                else
                {
                    TextStore.UnreadChars(HttpChar.LF, c);
                    c = HttpChar.CR;
                    return false;
                }
            }
            else
            {
                return false;
            }
        }

        protected byte[] ReadBySize(int size)
        {
            var buf = new byte[size];
            int len = 0;
            while (len < size)
            {
                var delta = TextStore.Read(buf, len, size - len);
                if (delta == -1)
                {
                    Array.Resize<byte>(ref buf, len);
                    return buf;
                }
                len += delta;
            }
            return buf;
        }

        protected byte[] ReadToEOF()
        {
            const int INITIAL_SIZE = 4096;
            const int MIN_SPACE = 1024;
            const int MAX_DELTA = 65536;
            int size = INITIAL_SIZE;
            var buf = new byte[size];
            int len = 0;
            while (true)
            {
                if (len + MIN_SPACE > size)
                {
                    size += size < MAX_DELTA ? size : MAX_DELTA;
                    Array.Resize<byte>(ref buf, size);
                }
                int delta = TextStore.Read(buf, len, size - len);
                if (delta == -1)
                {
                    Array.Resize<byte>(ref buf, len);
                    return buf;
                }
                len += delta;
            }
        }

        public abstract byte[] GetHeader(params HttpHeader[] rewrites);

        public abstract byte[] GetBytes(params HttpHeader[] rewrites);
    }
}
