namespace PhongThan.Event.Runtime;

public static class RuntimeDiagnostics {
    public static bool Enabled(string serverRoot) {
        try {
            if (!File.Exists(Path.Combine(serverRoot,"loose_only.flag"))) return false;
            string path=Path.Combine(serverRoot,"settings","phongthan","Diagnostics.ini");
            if (!File.Exists(path)) return false;
            bool section=false;
            foreach (string raw in File.ReadLines(path)) {
                string line=raw.Trim();
                if (line.StartsWith(';')||line.Length==0) continue;
                if (line.StartsWith('[')) { section=line.Equals("[Diagnostics]",StringComparison.OrdinalIgnoreCase);continue; }
                if (section&&line.Split('=',2) is [var key,var value]&&key.Trim().Equals("Enabled",StringComparison.OrdinalIgnoreCase)) return value.Trim()=="1";
            }
        } catch (IOException) { } catch (UnauthorizedAccessException) { }
        return false;
    }
}
