using System;
using System.IO;
using System.Text;
using System.Threading;

namespace hnrt.HttpRelay.File
{
    internal class FileWriter : IDisposable
    {
        public string Path { get; }

        public FileMode Mode { get; set; } = FileMode.Append;

        private FileStream fs;
        private StreamWriter sw;

        public FileWriter(string path)
        {
            Path = path;
        }

        public void Dispose()
        {
            Close();
        }

        public void Open()
        {
            fs = new FileStream(Path, Mode, FileAccess.Write, FileShare.Read);
            sw = new StreamWriter(fs, Encoding.UTF8);
        }

        public void Close()
        {
            Interlocked.Exchange(ref sw, null)?.Close();
            Interlocked.Exchange(ref fs, null)?.Close();
        }

        public void WriteLine(string text)
        {
            sw.WriteLine("{0}", text);
        }

        public void Flush()
        {
            sw.Flush();
        }
    }
}
