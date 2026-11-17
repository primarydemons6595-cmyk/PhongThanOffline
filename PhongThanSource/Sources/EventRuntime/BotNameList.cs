using System.Text;

namespace PhongThan.Event.Runtime;

public static class BotNameList
{
    public static string[] Read(string path)
    {
        if(new FileInfo(path).Length>65536)throw new InvalidDataException("File tên tối đa 64 KB; mỗi dòng một tên, không quá 128 tên.");
        // Strict UTF-8 by default; StreamReader also detects UTF-8/UTF-16 BOM.
        using var reader=new StreamReader(path,new UTF8Encoding(false,true),true);
        try{return Parse(reader.ReadToEnd());}
        catch(DecoderFallbackException){throw new InvalidDataException("Không đọc được bảng mã. Hãy lưu TXT dạng UTF-8 hoặc Unicode (UTF-16).");}
    }
    public static string[] Parse(string text)
    {
        var names=new List<string>();var used=new HashSet<string>(StringComparer.OrdinalIgnoreCase);
        using var reader=new StringReader(text);int line=0;
        while(reader.ReadLine() is {} raw){
            line++;string name=raw.Trim().Normalize(NormalizationForm.FormC);if(name.Length==0)continue;
            try{_ = BotNames.WireHex(name);}catch(InvalidDataException ex){throw new InvalidDataException($"Dòng {line}: {ex.Message}",ex);}
            if(!used.Add(name))throw new InvalidDataException($"Dòng {line}: tên '{name}' bị trùng trong file.");
            names.Add(name);if(names.Count>128)throw new InvalidDataException("Danh sách vượt 128 tên.");
        }
        if(names.Count==0)throw new InvalidDataException("File chưa có tên bot. Mỗi dòng nhập một tên.");
        return names.ToArray();
    }
}
