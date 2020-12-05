namespace hnrt.HttpRelay.Controller
{
    internal interface IController
    {
        void Run(string[] args);
        void ShutDown();
    }
}
