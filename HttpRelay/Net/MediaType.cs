using System.Collections.Generic;

namespace hnrt.HttpRelay.Net
{
    internal class MediaType
    {
        public static readonly string CHARSET = "charset";

        private static readonly char[] SEP_SLASH = { '/' };
        private static readonly char[] SEP_SEMICOLON = { ';' };
        private static readonly char[] SEP_EQUAL = { '=' };

        public string Type { get; set; }

        public string Subtype { get; set; }

        public List<(string Name, string Value)> Parameters { get; } = new List<(string Name, string Value)>();

        public string CharSet
        {
            get
            {
                foreach (var entry in Parameters)
                {
                    if (entry.Name.ToLowerInvariant() == CHARSET)
                    {
                        return entry.Value;
                    }
                }
                return null;
            }
        }

        private MediaType()
        {
        }

        public static MediaType Parse(string s)
        {
            var mt = new MediaType();
            var ss = s.Trim().Split(SEP_SEMICOLON);
            if (ss.Length > 0)
            {
                mt.Type = ss[0].Trim();
                if (mt.Type.Length > 0)
                {
                    var tt = mt.Type.Split(SEP_SLASH, 2);
                    if (tt.Length == 2)
                    {
                        mt.Type = tt[0].TrimEnd();
                        mt.Subtype = tt[1].TrimStart();
                    }
                }
                for (int i = 1; i < ss.Length; i++)
                {
                    var x = ss[i].Trim();
                    if (x.Length > 0)
                    {
                        var uu = x.Split(SEP_EQUAL, 2);
                        if (uu.Length == 2)
                        {
                            var k = uu[0].TrimEnd();
                            var v = uu[1].TrimStart();
                            if (k.Length > 0)
                            {
                                mt.Parameters.Add((k, v));
                            }
                        }
                        else
                        {
                            mt.Parameters.Add((x, null));
                        }
                    }
                }
            }
            return mt;
        }
    }
}
