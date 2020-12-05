namespace hnrt.HttpRelay.Controller
{
    internal interface IListener
    {
        void Start();
        void Stop();
        void Wait();
    }
}
