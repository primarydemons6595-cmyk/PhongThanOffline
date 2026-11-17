using System.Globalization;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace PhongThan.EventService;

internal sealed record EventNetworkConfiguration(IPAddress Address, int Port)
{
    public static EventNetworkConfiguration Load(string path)
    {
        if (!File.Exists(path)) return new(IPAddress.Loopback, 18979);
        return Parse(File.ReadAllText(path));
    }
    internal static EventNetworkConfiguration Parse(string text)
    {
        if (text.Length > 16384) throw new InvalidDataException("File cfg vượt quá 16 KB.");
        string section = "";var values = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
        foreach (string source in text.Split('\n'))
        {
            string line = source.Trim();if (line.Length == 0 || line.StartsWith(';') || line.StartsWith('#')) continue;
            if (line.StartsWith('[') && line.EndsWith(']'))
            {
                section = line[1..^1];if (!section.Equals("Network", StringComparison.OrdinalIgnoreCase)) throw new InvalidDataException("Chỉ hỗ trợ section [Network].");continue;
            }
            int equal = line.IndexOf('=');
            if (section.Length == 0 || equal < 1) throw new InvalidDataException("Dòng cfg không hợp lệ: " + line);
            string name = line[..equal].Trim(), value = line[(equal + 1)..].Trim();
            if ((!name.Equals("IP", StringComparison.OrdinalIgnoreCase) && !name.Equals("Port", StringComparison.OrdinalIgnoreCase)) || !values.TryAdd(name, value))
                throw new InvalidDataException("Khóa cfg lạ hoặc trùng: " + name);
        }
        if (!values.TryGetValue("IP", out var ip) || !IPAddress.TryParse(ip, out var address) || address.AddressFamily != AddressFamily.InterNetwork || !IPAddress.IsLoopback(address))
            throw new InvalidDataException("Network.IP phải là IPv4 loopback (127.x.x.x) để Studio kết nối nội bộ.");
        if (!values.TryGetValue("Port", out var number) || !int.TryParse(number, NumberStyles.None, CultureInfo.InvariantCulture, out int port) || port is < 1024 or > 65535)
            throw new InvalidDataException("Network.Port phải nằm trong 1024–65535.");
        return new(address, port);
    }
}
