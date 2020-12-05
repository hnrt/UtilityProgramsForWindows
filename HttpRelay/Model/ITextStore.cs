using System.Text;

namespace hnrt.HttpRelay.Model
{
    internal interface ITextStore : IByteStore
    {
        Encoding Encoding { get; set; }
        int ReadChar();
        void UnreadChar(int c);
        void UnreadChars(int c1, int c2);
        void UnreadString(string s);
    }
}
