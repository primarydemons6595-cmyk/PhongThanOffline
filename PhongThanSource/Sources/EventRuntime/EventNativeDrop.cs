using System.Globalization;
using System.Text;

namespace PhongThan.Event.Runtime;

public sealed record EventDropText(string Name, string Text, string Sha256);
public sealed record EventDropDraftRequest(long ExpectedRevision, int EventId, int Slot, string Text);

public static class EventNativeDrop
{
    // Same syntax/count/range checks as the native reader. This does not
    // reinterpret RandRate as weights or change the restored engine algorithm.
    public static byte[] Validate(string text)
    {
        if (text.Length > 2097152 || text.Any(c => c == 0 || c > 255)) throw new InvalidDataException("Bảng native phải giữ byte ANSI/Latin1, tối đa 2 MB.");
        var sections = new Dictionary<string, Dictionary<string, string>>(StringComparer.Ordinal);
        Dictionary<string, string>? current = null;
        foreach (string input in text.Split('\n'))
        {
            string line = input.Trim();if (line.Length == 0 || line[0] is ';' or '#') continue;
            if (line[0] == '[' && line[^1] == ']')
            {
                current = new(StringComparer.Ordinal);
                if (!sections.TryAdd(line[1..^1].Trim(), current)) throw new InvalidDataException("Section trùng.");
            }
            else
            {
                int at = line.IndexOf('=');
                if (current is null || at <= 0 || !current.TryAdd(line[..at].Trim(), line[(at + 1)..].Trim())) throw new InvalidDataException("Dòng lỗi hoặc khóa trùng.");
            }
        }
        int Number(Dictionary<string, string> row, string key)
            => row.TryGetValue(key, out string? value) && int.TryParse(value, NumberStyles.AllowLeadingSign, CultureInfo.InvariantCulture, out int n)
                ? n : throw new InvalidDataException("Thiếu/sai số nguyên: " + key);
        if (!sections.TryGetValue("Main", out var main)) throw new InvalidDataException("Thiếu [Main].");
        int count = Number(main, "Count"), range = Number(main, "RandRange");
        if (count is < 0 or > 4096 || range <= 0 || sections.Count != count + 1) throw new InvalidDataException("Count/RandRange không khớp các section.");
        for (int i = 1; i <= count; i++)
        {
            if (!sections.TryGetValue(i.ToString(CultureInfo.InvariantCulture), out var row)) throw new InvalidDataException("Thiếu section " + i);
            foreach (string key in new[] { "Genre", "Detail", "Particular", "RandRate" })
                if (Number(row, key) < 0) throw new InvalidDataException("Giá trị âm: " + key);
            if (Number(row, "RandRate") > range) throw new InvalidDataException("RandRate vượt RandRange.");
        }
        return Encoding.Latin1.GetBytes(text);
    }
}
