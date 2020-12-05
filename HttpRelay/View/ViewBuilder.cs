using hnrt.HttpRelay.File;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Threading;

namespace hnrt.HttpRelay.View
{
    internal class ViewBuilder
    {
        private string path = null;

        public ViewBuilder()
        {
        }

        public IView Build()
        {
            var view = new ConsoleView();
            if (path != null)
            {
                var f = new FileWriter(Interlocked.Exchange(ref path, null));
                f.Open();
                view.OnStartUp += () =>
                {
                };
                view.OnShutDown += () =>
                {
                };
                view.OnPutLine += (text) =>
                {
                    f.WriteLine(text);
                    f.Flush();
                };
                view.OnError += (e) =>
                {
                    f.WriteLine(string.Format("ERROR: {0}", e.Message));
                    while ((e = e.InnerException) != null)
                    {
                        f.WriteLine(string.Format("    {0}", e.Message));
                    }
                    f.Flush();
                };
            }
            return view;
        }

        private void View_OnStartUp()
        {
            throw new NotImplementedException();
        }

        public ViewBuilder SetPath(string value)
        {
            path = value;
            return this;
        }
    }
}
