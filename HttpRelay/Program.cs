using System;
using hnrt.HttpRelay.View;
using hnrt.HttpRelay.Controller;

namespace hnrt.HttpRelay
{
    class Program
    {
        static void Main(string[] args)
        {
            int exitCode = 1;

            var view = ViewFactory.CreateView();

            try
            {
                view.Open();

                var controller = ControllerFactory.CreateController();

                controller.Run(args);

                exitCode = 0;
            }
            catch (Exception e)
            {
                view.Put(e);
            }
            finally
            {
                view.Close();
            }

            Environment.Exit(exitCode);
        }
    }
}
