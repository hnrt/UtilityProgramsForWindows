namespace hnrt.HttpRelay.View
{
    internal class ViewFactory
    {
        public static ViewBuilder Builder { get; } = new ViewBuilder();

        public static IView CreateView()
        {
            return Builder.Build();
        }
    }
}
