namespace PhongThan.Event.Runtime;

public static class EventRuntimeLayout
{
    public static string Server(string root)
    {
        root=Path.GetFullPath(root);
        if(File.Exists(Path.Combine(root,"Data","release.catalog")))return root;
        if(File.Exists(Path.Combine(root,"Server Base","Data","release.catalog")))return Path.Combine(root,"Server Base");
        return Path.Combine(root,"Server");
    }
    public static bool IsRelease(string root)=>File.Exists(Path.Combine(Server(root),"Data","release.catalog"));
    public static bool Valid(string root)=>IsRelease(root)||(Directory.Exists(Path.Combine(root,"Server"))&&Directory.Exists(Path.Combine(root,"Client")));
}
