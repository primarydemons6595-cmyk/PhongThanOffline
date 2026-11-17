using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.Text.RegularExpressions;

namespace PhongThan.Event.Runtime;
public sealed record BotAiProfile {
    public int Version {get;init;}=1;
    public long Revision {get;init;}
    public string Provider {get;init;}="local";
    public string Model {get;init;}="gpt-4.1-mini";
    public int RequestsPerMinute {get;init;}=12;
    public int DailyRequestLimit {get;init;}=500;
    public bool AmbientOnline {get;init;}
}
public sealed record BotAiSaveRequest(long ExpectedRevision,BotAiProfile Profile){public string? ApiKey {get;init;}public bool ClearApiKey {get;init;}}
public sealed record BotAiUsage(string Day,int Requests,long InputTokens,long OutputTokens);
public sealed record BotAiStatus(BotAiProfile Profile,bool HasApiKey,BotAiUsage Usage);
public sealed record BotAiTestRequest(string BotId,string Text);
public sealed record BotAiTestResult(string Provider,string Model,string Reply,int Intent,int Milliseconds,string Error){public int Command {get;init;}public string Argument {get;init;}="";}
public sealed class BotAiSettingsStore {
    readonly object gate=new();readonly string folder,profileFile,keyFile,usageFile;
    long minute=-1;int minuteRequests;
    public BotAiSettingsStore(string botFolder){folder=Path.Combine(botFolder,"OnlineAI");profileFile=Path.Combine(folder,"profile.json");keyFile=Path.Combine(folder,"credential.dpapi");usageFile=Path.Combine(folder,"usage.json");}
    public BotAiProfile Profile(){lock(gate)return File.Exists(profileFile)?EventControlJson.Read<BotAiProfile>(profileFile):new();}
    static string Day(DateTimeOffset now)=>now.ToOffset(TimeSpan.FromHours(7)).ToString("yyyy-MM-dd",System.Globalization.CultureInfo.InvariantCulture);
    BotAiUsage Usage(DateTimeOffset now){var value=File.Exists(usageFile)?EventControlJson.Read<BotAiUsage>(usageFile):new(Day(now),0,0,0);return value.Day==Day(now)?value:new(Day(now),0,0,0);}
    public BotAiStatus Status(){lock(gate)return new(Profile(),File.Exists(keyFile),Usage(DateTimeOffset.UtcNow));}
    public BotAiStatus Save(BotAiSaveRequest request){lock(gate){
        var previous=Profile();if(previous.Revision!=request.ExpectedRevision)throw new InvalidOperationException("Cấu hình AI đã đổi. Hãy tải lại.");
        var profile=request.Profile;
        if(profile.Version!=1||profile.Provider is not("local" or "openai" or "rules")||!Regex.IsMatch(profile.Model,@"\A[A-Za-z0-9._:-]{1,100}\z")||profile.RequestsPerMinute is <1 or >120||profile.DailyRequestLimit is <1 or >100000)throw new InvalidDataException("Nhà cung cấp, model hoặc hạn mức AI không hợp lệ.");
        string? key=request.ApiKey?.Trim();if(!string.IsNullOrEmpty(key)&&(key.Length is <20 or >512||key.Any(c=>c<33||c>126)))throw new InvalidDataException("API key không hợp lệ.");
        if(profile.Provider=="openai"&&(request.ClearApiKey||(!File.Exists(keyFile)&&string.IsNullOrEmpty(key))))throw new InvalidOperationException("Nhập API key trước khi chọn AI Online.");
        Directory.CreateDirectory(folder);
        if(request.ClearApiKey){if(File.Exists(keyFile))File.Delete(keyFile);}
        else if(!string.IsNullOrEmpty(key)){byte[] plain=Encoding.UTF8.GetBytes(key);try{EventControlJson.AtomicBytes(keyFile,BotKeyProtection.Protect(plain));}finally{CryptographicOperations.ZeroMemory(plain);}}
        EventControlJson.Atomic(profileFile,profile with{Revision=checked(previous.Revision+1)});return Status();
    }}
    internal string Key(){lock(gate){if(!File.Exists(keyFile))throw new InvalidOperationException("Chưa nhập OpenAI API key.");byte[] plain=BotKeyProtection.Unprotect(File.ReadAllBytes(keyFile));try{return Encoding.UTF8.GetString(plain);}finally{CryptographicOperations.ZeroMemory(plain);}}}
    internal void Reserve(DateTimeOffset now){lock(gate){var profile=Profile();long window=now.ToUnixTimeSeconds()/60;if(minute!=window){minute=window;minuteRequests=0;}var usage=Usage(now);if(minuteRequests>=profile.RequestsPerMinute||usage.Requests>=profile.DailyRequestLimit)throw new InvalidOperationException("Đã chạm hạn mức gọi AI Online. Tăng hạn mức hoặc chờ lượt sau.");++minuteRequests;Directory.CreateDirectory(folder);EventControlJson.Atomic(usageFile,usage with{Requests=usage.Requests+1});}}
    internal void Tokens(int input,int output){lock(gate){var usage=Usage(DateTimeOffset.UtcNow);EventControlJson.Atomic(usageFile,usage with{InputTokens=usage.InputTokens+Math.Clamp(input,0,1000000),OutputTokens=usage.OutputTokens+Math.Clamp(output,0,1000000)});}}
}
static class BotKeyProtection {
    [StructLayout(LayoutKind.Sequential)]struct Blob {public int Size;public IntPtr Data;}
    [DllImport("crypt32.dll",CharSet=CharSet.Unicode,SetLastError=true)]static extern bool CryptProtectData(ref Blob input,string description,IntPtr entropy,IntPtr reserved,IntPtr prompt,uint flags,out Blob output);
    [DllImport("crypt32.dll",SetLastError=true)]static extern bool CryptUnprotectData(ref Blob input,out IntPtr description,IntPtr entropy,IntPtr reserved,IntPtr prompt,uint flags,out Blob output);
    [DllImport("kernel32.dll")]static extern IntPtr LocalFree(IntPtr memory);
    static byte[] Transform(byte[] source,bool encrypt){
        if(!OperatingSystem.IsWindows())throw new PlatformNotSupportedException("Lưu API key yêu cầu Windows DPAPI.");
        var input=new Blob{Size=source.Length,Data=Marshal.AllocHGlobal(source.Length)};Blob output=default;IntPtr description=IntPtr.Zero;
        try{Marshal.Copy(source,0,input.Data,source.Length);bool ok=encrypt?CryptProtectData(ref input,"PhongThan Bot AI",IntPtr.Zero,IntPtr.Zero,IntPtr.Zero,1,out output):CryptUnprotectData(ref input,out description,IntPtr.Zero,IntPtr.Zero,IntPtr.Zero,1,out output);if(!ok)throw new Win32Exception(Marshal.GetLastWin32Error(),"Không mở được khóa API đã mã hóa cho tài khoản Windows này.");byte[] result=new byte[output.Size];Marshal.Copy(output.Data,result,0,result.Length);return result;}
        finally{for(int i=0;i<input.Size;++i)Marshal.WriteByte(input.Data,i,0);Marshal.FreeHGlobal(input.Data);if(output.Data!=IntPtr.Zero){for(int i=0;i<output.Size;++i)Marshal.WriteByte(output.Data,i,0);LocalFree(output.Data);}if(description!=IntPtr.Zero)LocalFree(description);}
    }
    public static byte[] Protect(byte[] value)=>Transform(value,true);
    public static byte[] Unprotect(byte[] value)=>Transform(value,false);
}
