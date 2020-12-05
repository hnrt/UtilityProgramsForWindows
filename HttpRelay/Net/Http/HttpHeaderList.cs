using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpHeaderList : IEnumerable<HttpHeader>
    {
        private List<HttpHeader> list = new List<HttpHeader>();

        private Dictionary<string, HttpHeader> dict = new Dictionary<string, HttpHeader>();

        public HttpHeaderList()
        {
        }

        public void Clear()
        {
            list.Clear();
            dict.Clear();
        }

        public HttpHeader Add(HttpHeader h)
        {
            list.Add(h);
            var k = h.Name.ToLowerInvariant();
            if (dict.TryGetValue(k, out HttpHeader h2))
            {
                while (h2.Next != null)
                {
                    h2 = h2.Next;
                }
                h2.Next = h;
            }
            else
            {
                dict.Add(k, h);
            }
            return h;
        }

        public HttpHeader Add(string key, string value)
        {
            return Add(HttpHeaderFactory.CreateHeader(key, value));
        }

        public IEnumerator<HttpHeader> GetEnumerator()
        {
            return list.GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            throw new System.NotImplementedException();
        }

        public bool TryGetEntry(string key, out HttpHeader found)
        {
            return dict.TryGetValue(key.ToLowerInvariant(), out found);
        }

        public StringBuilder Format(StringBuilder sb, params HttpHeader[] rewrites)
        {
            var rw = rewrites.ToList();
            foreach (HttpHeader h in list)
            {
                var x = h;
                var name = h.Name.ToLowerInvariant();
                foreach (HttpHeader y in rw)
                {
                    if (y.Name.ToLowerInvariant() == name)
                    {
                        x = y;
                        rw.Remove(y);
                        break;
                    }
                }
                sb.AppendFormat("{0}: {1}\r\n", x.Name, x.Value);
            }
            foreach (HttpHeader h in rw)
            {
                sb.AppendFormat("{0}: {1}\r\n", h.Name, h.Value);
            }
            return sb;
        }

        public HttpHeaderTransferEncoding TransferEncoding
        {
            get
            {
                if (TryGetEntry(HttpHeaderTransferEncoding.NAME, out var found))
                {

                    return (HttpHeaderTransferEncoding)found;
                }
                else
                {
                    return null;
                }
            }
        }

        public int ContentLength
        {
            get
            {
                if (TryGetEntry(HttpHeaderContentLength.NAME, out var found))
                {
                    return ((HttpHeaderContentLength)found).ContentLength;
                }
                else
                {
                    return -1;
                }
            }
        }

        public HttpConnection Connection
        {
            get
            {
                if (TryGetEntry(HttpHeaderConnection.NAME, out var found))
                {
                    return ((HttpHeaderConnection)found).Connection;
                }
                else
                {
                    return HttpConnection.None;
                }
            }
        }

        public string Host
        {
            get
            {
                if (TryGetEntry(HttpHeaderHost.NAME, out var found))
                {
                    return found.Value;
                }
                else
                {
                    return null;
                }
            }
        }

        public HttpUpgrade Upgrade
        {
            get
            {
                if (TryGetEntry(HttpHeaderUpgrade.NAME, out var found))
                {
                    return ((HttpHeaderUpgrade)found).Upgrade;
                }
                else
                {
                    return HttpUpgrade.None;
                }
            }
        }
    }
}
