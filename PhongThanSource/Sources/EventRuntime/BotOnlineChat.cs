using System.Diagnostics;
using System.Net.Http.Headers;
using System.Text;
using System.Text.Json;
using System.Text.Json.Nodes;
using System.Text.RegularExpressions;

namespace PhongThan.Event.Runtime;
public sealed class BotOnlineChat : IDisposable {
    readonly BotAiSettingsStore settings;readonly HttpClient http;readonly Action<string> log;
    public BotOnlineChat(BotAiSettingsStore settings,Action<string> log,HttpMessageHandler? handler=null){this.settings=settings;this.log=log;http=handler is null?new():new(handler);http.Timeout=Timeout.InfiniteTimeSpan;}
    public static JsonObject RequestBody(object localBody,string model){var body=JsonSerializer.SerializeToNode(localBody)!.AsObject();body.Remove("chat_template_kwargs");body.Remove("max_tokens");body["max_completion_tokens"]=192;body["model"]=model;body["store"]=false;return body;}
    public async Task<JsonDocument> Complete(object localBody,CancellationToken stop){
        var profile=settings.Profile();string key=settings.Key();settings.Reserve(DateTimeOffset.UtcNow);
        using var request=new HttpRequestMessage(HttpMethod.Post,"https://api.openai.com/v1/chat/completions");request.Headers.Authorization=new AuthenticationHeaderValue("Bearer",key);request.Content=new StringContent(RequestBody(localBody,profile.Model).ToJsonString(),Encoding.UTF8,"application/json");
        using var timeout=CancellationTokenSource.CreateLinkedTokenSource(stop);timeout.CancelAfter(TimeSpan.FromSeconds(5));var elapsed=Stopwatch.StartNew();
        HttpResponseMessage received;
        try{received=await http.SendAsync(request,timeout.Token);}
        catch(OperationCanceledException)when(!stop.IsCancellationRequested){throw new HttpRequestException("OpenAI: quá thời gian chờ 5 giây. Kiểm tra mạng và DNS tới api.openai.com.");}
        catch(HttpRequestException e){throw new HttpRequestException(e.HttpRequestError==HttpRequestError.NameResolutionError?"OpenAI: không phân giải được DNS api.openai.com.":"OpenAI: không kết nối được máy chủ API. Kiểm tra mạng hoặc proxy.");}
        using var response=received;byte[] bytes=await response.Content.ReadAsByteArrayAsync(timeout.Token);
        if(!response.IsSuccessStatusCode){string code="request_failed";try{using var error=JsonDocument.Parse(bytes);string? value=error.RootElement.GetProperty("error").GetProperty("code").GetString();if(value is not null&&Regex.IsMatch(value,@"\A[A-Za-z0-9_-]{1,64}\z"))code=value;}catch(Exception e)when(e is JsonException or KeyNotFoundException or InvalidOperationException){}throw new HttpRequestException($"OpenAI HTTP {(int)response.StatusCode}: {code}");}
        var json=JsonDocument.Parse(bytes);int input=0,output=0;if(json.RootElement.TryGetProperty("usage",out var usage)){if(usage.TryGetProperty("prompt_tokens",out var p))p.TryGetInt32(out input);if(usage.TryGetProperty("completion_tokens",out var c))c.TryGetInt32(out output);}settings.Tokens(input,output);log($"provider=openai model={profile.Model} milliseconds={elapsed.ElapsedMilliseconds} input_tokens={input} output_tokens={output}");return json;
    }
    public void Dispose()=>http.Dispose();
}
