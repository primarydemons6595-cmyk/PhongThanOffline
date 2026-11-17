using System.Text;

namespace PhongThan.Event.Runtime;
public static class BotNames {
    public static string WireHex(string name){
        if(name!=name.Trim()||name.Any(c=>c<128&&!char.IsAsciiLetterOrDigit(c)&&c is not('_' or '.' or ' ')))throw new InvalidDataException("Tên bot chỉ dùng chữ, số, dấu chấm, gạch dưới và khoảng trắng.");
        string hex=BotChatEncoding.Encode(name.Normalize(NormalizationForm.FormC));
        if(hex.Length is <6 or >48)throw new InvalidDataException("Tên bot phải dài 3–24 byte TCVN3.");
        return hex;
    }
    public static bool Valid(string name){try{WireHex(name);return true;}catch(InvalidDataException){return false;}}
}
