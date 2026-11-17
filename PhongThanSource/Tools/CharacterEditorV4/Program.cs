namespace PhongThanCharacterEditor;
internal static class Program
{
    [STAThread]static int Main(string[] args){ApplicationConfiguration.Initialize();try{
        string root=FindWorkspace(),client=Path.Combine(root,"PhongThanRuntime-Loose","Client"),server=Path.Combine(root,"PhongThanRuntime-Loose","Server");
        if(args.Length>0&&args[0]=="--verify-bundle")return BundleVerification.Run(args[1],args[2]);
        if(args.Length>0&&args[0]=="--verify")return Verification.Run(root);
        if(args.Length>0&&args[0]=="--verify-vusi")return VuSiVerification.Run(root);
        if(args.Length>0&&args[0]=="--upgrade-verify")return UpgradeVerification.Run(root).GetAwaiter().GetResult();
        if(args.Length>0&&args[0]=="--gear-upgrade-verify")return UpgradeVerification.RunGear(root).GetAwaiter().GetResult();
        if(args.Length>0&&args[0]=="--preview"){using var form=new MainForm(client,server,args[1]);form.RenderPreview(args[2],args.Length>3?int.Parse(args[3]):0);return 0;}
        Application.Run(new MainForm(client,server,args.FirstOrDefault()));return 0;
    }catch(Exception ex){if(args.Length>0)File.WriteAllText(Path.Combine(AppContext.BaseDirectory,"editor-error.txt"),ex.ToString());else MessageBox.Show(ex.ToString(),"Character Editor");return 1;}}
    private static string FindWorkspace(){foreach(var start in new[]{Environment.CurrentDirectory,AppContext.BaseDirectory}){var d=new DirectoryInfo(start);while(d!=null){foreach(var candidate in new[]{d.FullName,Path.Combine(d.FullName,"PT")})if(Directory.Exists(Path.Combine(candidate,"PhongThanRuntime-Loose","Client")))return candidate;d=d.Parent;}}throw new DirectoryNotFoundException("Không tìm thấy Client Loose. Đặt editor trong cây thư mục PhongThan hoặc PT.");}
}
