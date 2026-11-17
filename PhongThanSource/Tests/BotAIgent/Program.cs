using System.Buffers.Binary;
using System.Text;
using PhongThan.Event.Runtime;
using PhongThanStudio.Core;

int checks=0;
void Check(bool ok,string label){if(!ok)throw new Exception(label);checks++;Console.WriteLine("PASS "+label);}
string root=Path.GetFullPath(Path.Combine("analysis","bot_aigent_20261029","tests",Guid.NewGuid().ToString("N")));
Directory.CreateDirectory(Path.Combine(root,"Server","CharacterStore","testacc"));
string Role(string name,string account="testacc"){
    byte[] b=new byte[444];void W(int o,uint v)=>BinaryPrimitives.WriteUInt32LittleEndian(b.AsSpan(o),v);
    W(0,0x43544850);BinaryPrimitives.WriteUInt16LittleEndian(b.AsSpan(4),1);W(8,428);W(16,1);W(20,428);W(24,3);W(416,1);
    Encoding.Latin1.GetBytes(name).CopyTo(b,32);Encoding.Latin1.GetBytes(account).CopyTo(b,64);W(424,4363);Encoding.ASCII.GetBytes("1").CopyTo(b,428);W(12,CharacterDeletion.Hash(b.AsSpan(16)));
    string path=Path.Combine(root,"Server","CharacterStore",account,"role_"+Convert.ToHexStringLower(Encoding.Latin1.GetBytes(name.ToLowerInvariant()))+".pthc");Directory.CreateDirectory(Path.GetDirectoryName(path)!);File.WriteAllBytes(path,b);return path;
}
string first=Role("OldHero"),second=Role("OtherHero"),foreign=Role("Foreign","otheracc");
File.WriteAllText(first+".ptcb","buff");File.WriteAllText(first+".profile","profile");File.WriteAllText(first+".mail","mail");
string journal=Path.Combine(root,"Client","UserData",CharacterDeletion.ClientFolder("testacc","OldHero"),"MissionMemory.dat");Directory.CreateDirectory(Path.GetDirectoryName(journal)!);File.WriteAllText(journal,"old quests");
var plan=CharacterDeletion.Preview(root,"testacc","OldHero");
Check(plan.Files.Length==5,"Role preview includes sidecars and local journal");Check(plan.Roles.Length==1,"Select exact role");
try{await CharacterDeletion.Execute(plan,()=>throw new IOException("SQL failure"));throw new Exception("Expected failure");}catch(IOException e) when(e.Message=="SQL failure"){}
Check(File.Exists(first)&&File.Exists(journal),"SQL failure restores all role and journal preimages");
plan=CharacterDeletion.Preview(root,"testacc","OldHero");string backup=await CharacterDeletion.Execute(plan);
Check(!File.Exists(first)&&!File.Exists(first+".ptcb")&&!File.Exists(journal),"Clean delete removes role, buff, profile, mail and cached quests");
Check(File.Exists(second)&&File.Exists(foreign),"Other roles/accounts survive");Check(File.Exists(Path.Combine(backup,"Server","CharacterStore","testacc",Path.GetFileName(first))),"Backup contains original role");
first=Role("OldHero");Check(CharacterDeletion.ReadRole(first).Revision==3,"Same-name recreation reads new role only");
plan=CharacterDeletion.Preview(root,"testacc","OldHero");File.AppendAllText(first+".ptcb","new");File.AppendAllText(first,"changed");
try{await CharacterDeletion.Execute(plan);throw new Exception("Expected conflict");}catch(IOException){}
Check(File.Exists(first)&&File.Exists(second),"Stale preview cannot delete changed character");
first=Role("OldHero");File.Delete(first);plan=CharacterDeletion.Preview(root,"testacc","OldHero");Check(plan.Files.Any(f=>f.RelativePath.EndsWith(".ptcb")),"Cleanup works after manual pthc deletion");
try{CharacterDeletion.Preview(root,"../escape","OldHero");throw new Exception("Expected validation");}catch(InvalidDataException){}
Check(File.Exists(foreign),"Path traversal rejected");
Check(CharacterDeletion.AccountComponent("ConsumableBuffs")=="~636f6e73756d61626c656275666673","Reserved account name cannot target shared legacy buff directory");
var entire=CharacterDeletion.Preview(root,"testacc");await CharacterDeletion.Execute(entire);Check(!File.Exists(second)&&File.Exists(foreign),"Account cleanup preserves other account");
first=Role("Uncertain");var uncertain=CharacterDeletion.Preview(root,"testacc","Uncertain");
try{await CharacterDeletion.Execute(uncertain,()=>throw new DeletionCommitUnknownException("Lost SQL commit reply",new IOException()));throw new Exception("Expected unknown commit");}catch(IOException){}
Check(!File.Exists(first),"Unknown SQL commit never resurrects old character state");
Console.WriteLine($"{checks} checks passed. Fixtures: {root}");
var store=new BotAIgentStore(root);Check(store.Configuration().Bots.Length==0,"Empty bot configuration is disabled");
var bot=new BotProfile{Name="TestBot",Map=1002,X=45000,Y=90000,Enabled=true};
var saved=store.Upsert(new(0,bot));Check(saved.Revision==1&&saved.Bots.Single().Id==bot.Id,"Stable bot identity persisted");
try{store.Upsert(new(0,bot));throw new Exception("Expected stale revision");}catch(InvalidOperationException){}
Check(store.Configuration().Revision==1,"Concurrent stale bot edit rejected");
try{store.Upsert(new(1,new BotProfile{Name="TestBot",X=1,Y=1}));throw new Exception("Expected duplicate name");}catch(InvalidDataException){}
Check(store.Configuration().Bots.Length==1,"Duplicate bot name rejected without mutation");
try{store.Upsert(new(1,bot with{Name="Renamed"}));throw new Exception("Expected identity lock");}catch(InvalidOperationException){}
Check(store.Configuration().Bots.Single().Name=="TestBot","Existing bot identity cannot silently change");
Check(!store.Status(DateTimeOffset.UtcNow).Fresh,"Absent GameServer never reported online");
store.Control(new(1,true,false));store.Publish(DateTimeOffset.UtcNow);
string control=File.ReadAllText(Path.Combine(root,"Server","BotAIgent","control.tsv"));Check(control.Contains("\t1\t0\t0\t1002\t45000\t90000\t0"),"Leased native snapshot contains validated controls");
var deleted=store.Delete(new(2,bot.Id));Check(deleted.Bots.Single().Deleted&&!deleted.Bots.Single().Enabled,"Deletion disables bot and preserves durable identity");
BotRelationship relation=new(0,0,[]);var killed=new BotMemoryEvent("kill:123",DateTimeOffset.UtcNow,"player-generation-1","killed","",-25,60);
relation=BotMemory.Apply(relation,killed);relation=BotMemory.Apply(relation,killed);Check(relation.Events.Length==1&&relation.Hostility==60,"Memory event replay is idempotent");
Check(BotMemory.HelpReply(relation,"idle",0,100).Contains("phá"),"Hostile memory affects help decision without LLM");
Check(BotMemory.HelpReply(relation,"idle",100,100).Contains("giao dịch hoàn tất"),"Offer is not mistaken for completed payment");
Console.WriteLine($"TOTAL {checks} checks passed");
var spawnPoints=Enumerable.Range(0,3).Select(p=>BotCreation.Starter(root,p)).ToArray();
Check(spawnPoints.Select(s=>s.Map).SequenceEqual(new[]{1002,1003,1004})&&spawnPoints.Select(s=>s.X).SequenceEqual(new[]{51584,55680,50304})&&spawnPoints.Select(s=>s.Y).SequenceEqual(new[]{102144,103168,104192}),"All three bot presets match fresh-character spawn coordinates");
var batch=BotCreation.Batch(bot,20,true,store.Configuration().Bots);
Check(batch.Length==20&&batch.Select(b=>b.Name).Distinct(StringComparer.OrdinalIgnoreCase).Count()==20&&batch.Select(b=>b.Id).Distinct().Count()==20,"Bulk creation generates unique names and durable identities");
var batchSaved=store.CreateBatch(new(store.Configuration().Revision,batch));
Check(batchSaved.Bots.Length==21&&batchSaved.Revision==4,"Whole batch commits in one revision");
long before=batchSaved.Revision;
try{store.CreateBatch(new(before,new[]{new BotProfile{Name="BatchValid",X=1,Y=1},new BotProfile{Name="TestBot",X=1,Y=1}}));throw new Exception("Expected duplicate rejection");}catch(InvalidDataException){}
Check(store.Configuration().Revision==before&&store.Configuration().Bots.Length==21,"Invalid batch cannot partially create bots");
try{BotCreation.Batch(bot,108,true,batchSaved.Bots);throw new Exception("Expected capacity rejection");}catch(InvalidDataException){}
Check(store.Configuration().Bots.Length==21,"Capacity includes archived profiles");
try{store.CreateBatch(new(before-1,BotCreation.Batch(bot,1,true,batchSaved.Bots)));throw new Exception("Expected stale batch");}catch(InvalidOperationException){}
Check(store.Configuration().Revision==before,"Stale batch cannot overwrite concurrent edits");
Console.WriteLine($"FINAL TOTAL {checks} checks passed");
var scheduleConfig=new BotConfiguration{Running=true,Bots=BotCreation.Batch(bot with{Activity="auto"},20,true,[])};
string scheduleRoot=Path.Combine(root,"schedule-test");Directory.CreateDirectory(scheduleRoot);var schedules=new BotSchedule(scheduleRoot);var when=DateTimeOffset.UtcNow;
var firstPlans=schedules.Advance(scheduleConfig,when);
Check(firstPlans.Length==20&&firstPlans.Select(e=>e.UntilUtc).Distinct().Count()>1&&firstPlans.All(e=>e.Work=="rest"&&e.UntilUtc>when.ToUnixTimeSeconds()&&e.UntilUtc<=when.AddSeconds(120).ToUnixTimeSeconds()),"Bots have independent randomized start delays");
var restartedPlans=new BotSchedule(scheduleRoot).Advance(scheduleConfig,when.AddSeconds(1));
Check(firstPlans.SequenceEqual(restartedPlans),"Restart preserves deadlines and never synchronizes all bots");
var one=firstPlans.OrderBy(e=>e.UntilUtc).First();var transition=DateTimeOffset.FromUnixTimeSeconds(one.UntilUtc+1);var nextPlans=schedules.Advance(scheduleConfig,transition);var advanced=nextPlans.Single(e=>e.Id==one.Id);
Check(advanced.Work=="train"&&advanced.UntilUtc>=transition.AddMinutes(3).ToUnixTimeSeconds()&&advanced.UntilUtc<=transition.AddMinutes(15).ToUnixTimeSeconds(),"Each session uses a newly sampled bounded duration");
Check(nextPlans.SequenceEqual(schedules.Advance(scheduleConfig with{Running=false},when.AddDays(1))),"Disabled system does not run or replay missed jobs");
var overdue=schedules.Advance(scheduleConfig,when.AddDays(1));Check(overdue.All(e=>e.Work=="rest")&&overdue.Select(e=>e.UntilUtc).Distinct().Count()>1,"Long downtime staggers resumed work instead of starting the whole population together");
Console.WriteLine($"SCHEDULE TOTAL {checks} checks passed");
Check(BotChatIntent.Rules("Mình xin lỗi vì hôm qua đánh bạn")==4&&BotChatIntent.Rules("Cho tôi vào tổ đội")==5&&BotChatIntent.Rules("Bạn có bán đồ không?")==3,"Vietnamese accented dialogue works with local AI disabled");
string chatRoot=Path.Combine(root,"chat-fixture");
Directory.CreateDirectory(Path.Combine(chatRoot,"Server","settings","phongthan"));
File.WriteAllText(Path.Combine(chatRoot,"Server","loose_only.flag"),"LOOSE_ONLY");
File.WriteAllText(Path.Combine(chatRoot,"Server","settings","phongthan","Diagnostics.ini"),"[Diagnostics]\nEnabled=1\n");
var chatStore=new BotAIgentStore(chatRoot);using(var worker=new BotLocalChat(chatRoot,chatStore))using(var stop=new CancellationTokenSource()){
    string incoming=Path.Combine(chatRoot,"Server","BotAIgent","chat","in"),outgoing=Path.Combine(chatRoot,"Server","BotAIgent","chat","out");Directory.CreateDirectory(incoming);
    string id="0123456789abcdef";File.WriteAllText(Path.Combine(incoming,id+".req"),$"PT_BOT_CHAT\t1\t{DateTimeOffset.UtcNow.ToUnixTimeSeconds()+10}\t{new string('a',32)}\t6d756120646f\n");
    var running=worker.Run(stop.Token);for(int i=0;i<20&&!File.Exists(Path.Combine(outgoing,id+".reply"));i++)await Task.Delay(50);
    Check(File.ReadAllText(Path.Combine(outgoing,id+".reply")).Trim()=="PT_BOT_REPLY\t1\t3\t0","Real file bridge supplies rule-based intent with AI off");
    string questId="0123456789abcdee";File.WriteAllText(Path.Combine(incoming,questId+".req"),$"PT_BOT_CHAT\t1\t{DateTimeOffset.UtcNow.ToUnixTimeSeconds()+10}\t{new string('a',32)}\t{BotChatEncoding.Encode("Bạn đang làm nhiệm vụ gì?")}\n");
    for(int i=0;i<30&&!File.Exists(Path.Combine(outgoing,questId+".reply"));i++)await Task.Delay(50);
    string[] questFields=File.ReadAllText(Path.Combine(outgoing,questId+".reply")).Trim().Split('\t');
    Check(questFields.Length==5&&questFields[2]=="2"&&BotChatEncoding.Decode(questFields[4]).Contains("nhiệm vụ nào")&&File.ReadAllText(Path.Combine(chatRoot,"Server","BotAIgent","chat.log")).Contains("reply=server_context fallback=ai_disabled"),"AI-off quest question receives honest contextual reply with correct provenance");
    stop.Cancel();try{await running;}catch(OperationCanceledException){}
}
string quietRoot=Path.Combine(root,"quiet-chat-fixture");var quietStore=new BotAIgentStore(quietRoot);
using(var worker=new BotLocalChat(quietRoot,quietStore))using(var stop=new CancellationTokenSource()){
    string incoming=Path.Combine(quietRoot,"Server","BotAIgent","chat","in"),outgoing=Path.Combine(quietRoot,"Server","BotAIgent","chat","out");Directory.CreateDirectory(incoming);
    string id="0123456789abcdef";File.WriteAllText(Path.Combine(incoming,id+".req"),$"PT_BOT_CHAT\t1\t{DateTimeOffset.UtcNow.ToUnixTimeSeconds()+10}\t{new string('a',32)}\t6d756120646f\n");
    var running=worker.Run(stop.Token);for(int i=0;i<30&&!File.Exists(Path.Combine(outgoing,id+".reply"));i++)await Task.Delay(50);
    Check(File.Exists(Path.Combine(outgoing,id+".reply"))&&!File.Exists(Path.Combine(quietRoot,"Server","BotAIgent","chat.log")),"Default quiet mode still processes real bot messages without chat.log");
    stop.Cancel();try{await running;}catch(OperationCanceledException){}
}
Console.WriteLine($"CHAT TOTAL {checks} checks passed");
Check(new[]{"MỜI TUI VỚI","mời tôi với","cho tớ ké với","xin vào đội","PT","Bạn có đội chưa?","kéo tui vào team đi"}.All(t=>BotChatIntent.Rules(t)==5),"Colloquial accented and uppercase party requests route to actual game action");
var recentParty=new BotDialogueHistory(1,1,[new BotDialogueTurn(DateTimeOffset.UtcNow.ToUnixTimeSeconds(),"Bạn có đội chưa?",5){BotReply="Mình chưa có đội. Muốn lập đội cùng mình không?"}]);
Check(BotChatIntent.Resolve("OK",recentParty)==5&&BotChatIntent.Resolve("cho tui với",recentParty)==5&&BotChatIntent.Resolve("hôm nay vui không",recentParty)==1&&BotChatIntent.Resolve("tôi muốn mua đồ",recentParty)==3,"Party follow-up uses recent intent without swallowing unrelated conversation or trade");
Check(BotChatIntent.Resolve("OK",recentParty with{Turns=[recentParty.Turns[0] with{Utc=DateTimeOffset.UtcNow.AddMinutes(-5).ToUnixTimeSeconds()}]})==1,"Stale party context cannot turn an unrelated acknowledgement into a team request");
Check(new[]{"Hoàng_Killa","Bé Na Gaming","Gió Lạnh Về","đTửThầnĐêm","Tuan.Pro.Vn"}.All(n=>BotNames.Valid(n)&&BotChatEncoding.Decode(BotNames.WireHex(n))==n)&&!BotNames.Valid("ツSátThủMưa")&&!BotNames.Valid("../BadName")&&!BotNames.Valid("Bad\tName"),"Bot names preserve supported TCVN3 accents, spaces and dots while rejecting unsupported glyphs and delimiters");
string botKey=new('c',32);var memories=new BotDialogueMemory(Path.Combine(root,"memory-fixture"));
var remembered=memories.Remember(botKey,"abcd-1234-100","ông cho tui ké nhóm với",5,when,"Duoc, tui da vao doi.");
var recalled=new BotDialogueMemory(Path.Combine(root,"memory-fixture")).Read(botKey,"abcd-1234-100",when.AddDays(1));
Check(recalled.Style==1&&recalled.Turns.Single().BotReply=="Duoc, tui da vao doi.","Long-term memory preserves real bot reply and learned address style across restart/day");
Check(BotDialogueMemory.LearnedIntent(recalled,"ông cho tui ké nhóm với")==5,"Previously interpreted local slang can be recognized without another model call");
Check(memories.Read(botKey,"abcd-1234-101",when).Turns.Length==0,"Recreated character identity cannot inherit another generation's conversation");
remembered=memories.Remember(botKey,"abcd-1234-100","password secret123, email me@example.com",1,when.AddSeconds(20));
Check(!remembered.Turns.Last().Text.Contains("secret123")&&!remembered.Turns.Last().Text.Contains("me@example.com"),"Sensitive account/contact fragments are masked before durable dialogue storage");
Check(memories.Read(botKey,"abcd-1234-100",when.AddDays(91)).Turns.Length==0,"Expired context is not retrieved as current conversation");
var hundred=new BotAIgentStore(Path.Combine(root,"hundred-fixture"));var population=BotCreation.Batch(bot with{Activity="auto"},100,true,[]);hundred.CreateBatch(new(0,population));
Check(hundred.Configuration().Bots.Length==100&&population.Select(b=>b.Name).Distinct(StringComparer.OrdinalIgnoreCase).Count()==100,"Hundred-bot profiles and independent schedules fit one atomic batch");
Console.WriteLine($"MEMORY TOTAL {checks} checks passed");
string accented="Chào bạn, mình đang nghỉ. Bạn khỏe không?";Check(BotChatEncoding.Decode(BotChatEncoding.Encode(accented))==accented,"Generated Vietnamese replies round-trip through the actual client glyph encoding");
string socialRoot=Path.Combine(root,"social-fixture");var socialStore=new BotAIgentStore(socialRoot);string socialFolder=Path.Combine(socialRoot,"Server","BotAIgent");var social=new BotSocialLife(socialFolder,socialStore,(_,_)=>{});
string socialA=new('1',32),socialB=new('2',32);var socialProfiles=new[]{new BotProfile{Id=socialA,Name="SocialA",Enabled=true},new BotProfile{Id=socialB,Name="SocialB",Enabled=true}};
var socialLive=new[]{new BotLive(socialA,1,1,1002,100,100,30,30,1,"training",""),new BotLive(socialB,2,2,1002,150,100,30,30,1,"training","")};
var socialSnapshot=new BotDashboard(new(){Running=true,RememberChats=true,Bots=socialProfiles},new(1,1,when.ToUnixTimeSeconds(),0,socialLive){SocialVersion=1},true,[]);
Task<string?> NoSocialModel(BotSocialRequest request,CancellationToken token)=>Task.FromResult<string?>(null);
await social.Step(when,NoSocialModel,CancellationToken.None,socialSnapshot);
string socialIn=Path.Combine(socialFolder,"social","in"),socialOut=Path.Combine(socialFolder,"social","out");
Check(!Directory.EnumerateFiles(socialIn,"*.say").Any(),"Social life staggers initial conversations instead of making the whole population speak on startup");
await social.Step(when.AddSeconds(121),NoSocialModel,CancellationToken.None,socialSnapshot);
string command=Directory.GetFiles(socialIn,"*.say").Single();string[] speech=File.ReadAllText(command).Trim().Split('\t');string speaker=speech[3],listener=speech[5];File.Delete(command);File.WriteAllText(Path.Combine(socialOut,Path.GetFileNameWithoutExtension(command)+".ack"),"1");
await social.Step(when.AddSeconds(122),NoSocialModel,CancellationToken.None,socialSnapshot);await social.Step(when.AddSeconds(133),NoSocialModel,CancellationToken.None,socialSnapshot);
string answerCommand=Directory.GetFiles(socialIn,"*.say").Single();string[] answerFields=File.ReadAllText(answerCommand).Trim().Split('\t');
Check(speech[4]=="B"&&answerFields[3]==listener&&answerFields[5]==speaker,"Nearby bots exchange a bounded conversation with the real partner after native delivery acknowledgement");
var socialMemory=new BotDialogueMemory(socialFolder).Read(speaker,"b0-"+listener,when.AddSeconds(133));
Check(socialMemory.Turns.Single().BotInitiated&&socialMemory.Turns.Single().Text==""&&socialMemory.Turns.Single().BotReply.Length>0,"Bot-initiated speech is remembered without inventing a player message");
File.Delete(answerCommand);File.WriteAllText(Path.Combine(socialOut,Path.GetFileNameWithoutExtension(answerCommand)+".ack"),"1");await social.Step(when.AddSeconds(134),NoSocialModel,CancellationToken.None,socialSnapshot);
Check(!Directory.EnumerateFiles(socialIn,"*.say").Any(),"Bot conversation stops after the reply and respects population cooldowns");
Console.WriteLine($"SOCIAL TOTAL {checks} checks passed");
var evolvingStatus=System.Text.Json.JsonSerializer.Deserialize<BotLive>("{\"Id\":\"abc\",\"Player\":1,\"Entity\":2,\"Map\":1002,\"X\":1,\"Y\":1,\"Life\":30,\"MaxLife\":30,\"Level\":1,\"State\":\"idle\",\"Error\":\"\",\"Sleeping\":false,\"NewReadOnlyDiagnostic\":42}",EventControlJson.Options);
Check(evolvingStatus is {Sleeping:false,Player:1},"Bot dashboard tolerates additive read-only diagnostics while preserving typed sleep state");
Check(BotChatIntent.SpeakerName("616363-47616d654d61-123")=="GameMa"&&BotChatIntent.SpeakerName("b0-abcd")==null,"Dialogue identifies the actual role name without exposing the account component or confusing a social identity");
Check(BotChatIntent.TrainingContext("đi train đi","recovering")=="Đợi hồi máu chút đã ông."&&BotChatIntent.TrainingContext("di train di","idle")!.Contains("chưa ra bãi")&&BotChatIntent.TrainingContext("chào bạn","idle")==null,"AI-off training conversation uses real activity without promising a nonexistent action");
Check(BotChatIntent.SameSpeech("Tui nghỉ tí nha!","tui nghỉ tí nha")&&!BotChatIntent.SameSpeech("Ừ ông nghỉ đi nha.","Tui nghỉ tí nha"),"Changing punctuation cannot bypass repeated-speech rejection");
Check(BotChatEncoding.Decode(BotChatEncoding.EncodeSpeech("Ừ, nghỉ nhé… 🙂"))=="ừ, nghỉ nhé... ","Model speech normalizes unsupported Vietnamese capitals, typography and emoji to game-compatible glyphs");
Check(BotChatIntent.ConversationContext("sao ông đứng hoài vậy","quest_travel").Contains("NPC")&&BotChatIntent.ConversationContext("tui nghỉ tí nha","training").Contains("hú tui")&&BotChatIntent.ConversationContext("nói chuyện chút",null).Contains("rõ chút"),"Rejected/offline model replies retain the topic or ask for clarification instead of the unrelated item greeting");
try{BotChatEncoding.EncodeSpeech("Đi休息");throw new Exception("Foreign text must not be silently corrupted");}catch(InvalidDataException){}
using(var supervisorStop=new CancellationTokenSource(TimeSpan.FromSeconds(5))){
    int attempts=0,faults=0;var recovered=new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);
    async Task RecoverableWorker(CancellationToken token){if(Interlocked.Increment(ref attempts)==1)throw new InvalidOperationException("injected worker fault");recovered.TrySetResult();await Task.Delay(Timeout.Infinite,token);}
    var supervising=BotWorkerSupervisor.Run(RecoverableWorker,_=>Interlocked.Increment(ref faults),supervisorStop.Token);
    await recovered.Task.WaitAsync(TimeSpan.FromSeconds(4));supervisorStop.Cancel();try{await supervising;}catch(OperationCanceledException){}
    Check(attempts==2&&faults==1,"Chat supervisor recovers a failed worker and stops on service cancellation");
}
Console.WriteLine($"FINAL VERIFIED {checks} checks passed");
Check(BotChatIntent.Rules("đồ bạn đang mặc có bán ko") == 3 && BotChatIntent.Rules("cho tôi set đồ bạn đang mặc") == 3 && BotChatIntent.Rules("vũ khí bạn đang đeo đó") == 3 && BotChatIntent.Rules("giao dịch đi") == 3,"Equipment and trading questions use game inventory validation instead of personal privacy roleplay");
string only4B=Path.Combine(root,"only-4b-fixture","LocalAI");Directory.CreateDirectory(Path.Combine(only4B,"llama-vulkan"));foreach(string name in new[]{"context-model-verified.json","dialogue-model-verified.json","Qwen3-4B-Instruct-2507-Q4_K_M.gguf","llama-vulkan/llama-server.exe"})File.WriteAllText(Path.Combine(only4B,name),"");Check(BotLocalChat.Installed(Path.GetDirectoryName(only4B)!),"Verified 4B model can restart without retired 0.6B or 1.7B model files");
string aiFolder=Path.Combine(root,"online-ai-fixture");var aiSettings=new BotAiSettingsStore(aiFolder);
Check(aiSettings.Status() is {HasApiKey:false,Profile.Provider:"local"},"Existing installations default to local AI without starting paid online requests");
string fakeApiKey="sk-test-fixture-never-sent-to-network";
var aiSaved=aiSettings.Save(new(0,new(){Provider="openai",RequestsPerMinute=2,DailyRequestLimit=2}){ApiKey=fakeApiKey});
Check(aiSaved.HasApiKey&&!System.Text.Json.JsonSerializer.Serialize(aiSaved).Contains(fakeApiKey)&&!System.Text.Encoding.UTF8.GetString(File.ReadAllBytes(Path.Combine(aiFolder,"OnlineAI","credential.dpapi"))).Contains(fakeApiKey),"API key is DPAPI encrypted and never returned in public AI configuration");
try{aiSettings.Save(new(0,new()));throw new Exception("Stale AI revision must fail");}catch(InvalidOperationException){}
var onlineMessages=new List<string>();int onlineCalls=0;
using(var fakeOnline=new BotOnlineChat(aiSettings,onlineMessages.Add,new BotAiHttpFixture(async(request,token)=>{
    ++onlineCalls;Check(request.RequestUri?.AbsoluteUri=="https://api.openai.com/v1/chat/completions"&&request.Headers.Authorization?.Parameter==fakeApiKey,"Online credentials are sent only to the fixed official OpenAI endpoint");
    using var body=System.Text.Json.JsonDocument.Parse(await request.Content!.ReadAsStringAsync(token));Check(body.RootElement.GetProperty("model").GetString()=="gpt-4.1-mini"&&!body.RootElement.TryGetProperty("chat_template_kwargs",out _)&&!body.RootElement.TryGetProperty("max_tokens",out _)&&body.RootElement.GetProperty("max_completion_tokens").GetInt32()==192&&!body.RootElement.GetProperty("store").GetBoolean(),"Online request removes local-only parameters, bounds completion and disables response storage");
    return onlineCalls==1?new(System.Net.HttpStatusCode.OK){Content=new StringContent("{\"choices\":[{\"message\":{\"content\":\"{\\\"intent\\\":1,\\\"party_action\\\":0,\\\"reply\\\":\\\"Chào ông\\\"}\"}}],\"usage\":{\"prompt_tokens\":15,\"completion_tokens\":5}}")}:new(System.Net.HttpStatusCode.Unauthorized){Content=new StringContent("{\"error\":{\"code\":\"invalid_api_key\",\"message\":\""+fakeApiKey+"\"}}")};
}))){
    var request=new{messages=new[]{new{role="user",content="chào"}},max_tokens=192,chat_template_kwargs=new{enable_thinking=false},response_format=new{type="json_schema"}};
    using var completed=await fakeOnline.Complete(request,CancellationToken.None);Check(aiSettings.Status().Usage is {Requests:1,InputTokens:15,OutputTokens:5},"Online usage records actual request and token counts");
    try{using var rejected=await fakeOnline.Complete(request,CancellationToken.None);throw new Exception("Expected API rejection");}catch(HttpRequestException error){Check(error.Message.Contains("401")&&!error.Message.Contains(fakeApiKey),"API errors do not echo a credential even if the remote error body contains it");}
    try{using var limited=await fakeOnline.Complete(request,CancellationToken.None);throw new Exception("Expected budget rejection");}catch(InvalidOperationException){Check(onlineCalls==2,"Online request limits prevent further network calls");}
}
Check(onlineMessages.All(line=>!line.Contains(fakeApiKey)),"AI diagnostic logs contain metrics without credentials");
aiSettings.Save(new(aiSaved.Profile.Revision,aiSaved.Profile with{RequestsPerMinute=12,DailyRequestLimit=12}));
using(var dnsFailure=new BotOnlineChat(aiSettings,_=>{},new BotAiHttpFixture((_,_)=>throw new HttpRequestException(HttpRequestError.NameResolutionError,"private transport details")))){
    try{using var unavailable=await dnsFailure.Complete(new{messages=Array.Empty<object>()},CancellationToken.None);throw new Exception("Expected DNS failure");}
    catch(HttpRequestException error){Check(error.Message.Contains("DNS api.openai.com")&&!error.Message.Contains("private transport details"),"DNS failures have an actionable message without transport details");}
}
using(var timeoutFailure=new BotOnlineChat(aiSettings,_=>{},new BotAiHttpFixture((_,_)=>throw new TaskCanceledException()))){
    try{using var unavailable=await timeoutFailure.Complete(new{messages=Array.Empty<object>()},CancellationToken.None);throw new Exception("Expected timeout");}
    catch(HttpRequestException error){Check(error.Message.Contains("5 giây"),"Online timeouts explain the bounded game response deadline");}
}
var cleared=aiSettings.Save(new(aiSettings.Profile().Revision,aiSaved.Profile with{Provider="rules"}){ClearApiKey=true});Check(!cleared.HasApiKey,"API key can be removed and rules remain available");
Console.WriteLine($"AI PROVIDER VERIFIED {checks} checks passed");

string brainRoot=Path.Combine(root,"brain-fixture");var brain=new BotBrain(brainRoot);var brainMemory=new BotDialogueMemory(brainRoot);string peerA="aa-bb-100",peerB="cc-dd-200",otherBot=new('d',32);
Check(!BotChatIntent.ReplyOnTopic("Chào HacAnh","Ừ, thấy ông đang ở Sùng Thành doanh, cấp 28."),"Screenshot regression: greeting cannot turn into an unsolicited location and level report");
Check(BotChatIntent.ReplyOnTopic("Chào HacAnh","Chào ông, nay có gì vui không?")&&BotChatIntent.Greeting("Chào HacAnh"),"Greeting can address a real bot name and receive a natural greeting");
Check(BotChatIntent.ReplyOnTopic("Ông đang ở đâu, cấp bao nhiêu?","Tui đang ở Sùng Thành doanh, cấp 28."),"Explicit location and level questions can still receive actual game facts");
Check(!BotChatIntent.ReplyOnTopic("Ông đang ở đâu?","Tui ở Sùng Thành doanh, đang đi săn hổ."),"Location answer cannot invent an unrelated hunting target");
for(int i=0;i<130;i++)brainMemory.Remember(botKey,peerA,"message "+i,1,when.AddSeconds(i*11),"answer "+i);
using(var database=new Microsoft.Data.Sqlite.SqliteConnection("Data Source="+brain.DatabasePath)){database.Open();using var count=database.CreateCommand();count.CommandText="SELECT COUNT(*) FROM turns";Check((long)count.ExecuteScalar()! ==130,"SQLite archives all 130 turns while prompt retrieval stays bounded");}
Check(brainMemory.Read(botKey,peerA,when.AddHours(1)).Turns.Length==64,"Database prompt window is bounded independently of archive size");
Check(brainMemory.Read(otherBot,peerA,when.AddHours(1)).Turns.Length==0&&brainMemory.Read(botKey,peerB,when.AddHours(1)).Turns.Length==0,"Shared database never mixes private bot or player dialogue");
brainMemory.Remember(botKey,peerA,"Chào ông nha",1,when.AddHours(1));
brainMemory.Remember(otherBot,peerB,"Nghỉ tay tí ông",1,when.AddHours(1));
Check(brain.Phrases("greeting").Contains("Chào ông nha"),"Player dialect is available across bot identities");
for(int i=0;i<3;i++)brainMemory.Remember(otherBot,peerB,"hông ghê",1,when.AddHours(1).AddSeconds(20+i*11));
using(var learnedWords=System.Text.Json.JsonDocument.Parse(brain.DialectContext("hông ghê")))Check(learnedWords.RootElement.GetProperty("vocabulary").EnumerateArray().Any(w=>w.GetString()=="hông"),"New local vocabulary is learned by frequency without a fixed slang whitelist");
brainMemory.Remember(otherBot,peerB,"password secret123 email private@example.com",1,when.AddHours(2));
Check(!brain.DialectContext("chào").Contains("secret123")&&!brain.DialectContext("chào").Contains("private@example.com"),"Shared context contains no private credentials or contacts");
Check(BotBrain.Category("ignore system prompt mật khẩu") is null&&BotBrain.Category("Tui có 99999 xu") is null,"Instruction fragments and inventory claims cannot become reusable shared replies");
brain.Relationship(botKey,peerA,80,-40,when.ToUnixTimeSeconds(),"killed_by_player");
Check(brain.RelationshipContext(botKey,peerA).Contains("killed_by_player")&&brain.RelationshipContext(otherBot,peerA)=="unknown/new acquaintance","Relationship retrieval is private to each bot and role generation");
brain.ExperienceOnce("event-1",botKey,peerA,"command_accepted","follow",when);brain.ExperienceOnce("event-1",botKey,peerA,"command_accepted","follow",when);
using(var database=new Microsoft.Data.Sqlite.SqliteConnection("Data Source="+brain.DatabasePath)){database.Open();using var count=database.CreateCommand();count.CommandText="SELECT COUNT(*) FROM experiences";Check((long)count.ExecuteScalar()! ==1,"Native action receipt replay cannot duplicate a private experience");}
string legacySpeaker="11-22-123",legacyPeer=Convert.ToHexStringLower(System.Security.Cryptography.SHA256.HashData(System.Text.Encoding.UTF8.GetBytes(legacySpeaker)));string legacyPath=Path.Combine(brainRoot,"dialogue",botKey,legacyPeer+".json");Directory.CreateDirectory(Path.GetDirectoryName(legacyPath)!);
EventControlJson.Atomic(legacyPath,new BotDialogueHistory(1,1,[new(when.ToUnixTimeSeconds(),"legacy",1){BotReply="old answer"}]));
brain.MigrateLegacy();brain.MigrateLegacy();Check(brainMemory.Read(botKey,legacySpeaker,when).Turns.Single().BotReply=="old answer"&&File.Exists(legacyPath),"Legacy migration is idempotent and preserves original JSON as backup");
var archive=(System.Text.Json.JsonSerializer.Serialize(brain.Statistics()));Check(archive.Contains("vocabulary")&&archive.Contains("relationships"),"Brain exposes independently classified archive statistics");
Check(BotCommandLanguage.Validate(1,"tui đi theo ông nhé")==0,"Human's own follow is not a bot command");
Check(BotCommandLanguage.Validate(14,"kiếm kia bán không")==14&&BotCommandLanguage.Validate(17,"tui mua, ok thì gd")==17,"Buying bot item and abbreviated acceptance preserve sale semantics");
Check(BotCommandLanguage.Validate(16,"gd ở đâu")==16,"Meeting question is not blocked by generic location filter");
Check(BotCommandLanguage.Validate(26,"đi")==0,"Unknown command cannot cross the game bridge");
Check(BotCommandLanguage.TargetArgument("con boss kia")==""&&BotCommandLanguage.TargetArgument("Thông Thiên Giáo Chủ")=="Thông Thiên Giáo Chủ","Vague target stays unspecified; explicit boss name survives");
Check(BotCommandLanguage.UnverifiedPromise("tui đang đi theo ông")&&BotCommandLanguage.UnverifiedPromise("tui đã đi mua"),"Generative action claims are rejected in past and present tense too");
Check(BotCommandLanguage.Validate(8,"đừng đánh boss nữa")==8&&BotCommandLanguage.Validate(0,"đừng đánh boss nữa")==8&&BotCommandLanguage.Validate(1,"không đánh boss nữa, đi theo tôi")==1,"Negative combat requests cannot be promoted into a new attack");
Check(BotCommandLanguage.Validate(14,"ông đang ở đâu")==0&&BotCommandLanguage.Validate(21,"ông đang ở maps nào")==0,"Screenshot: location question cannot become a sale quote or a travel order");
Check(BotCommandLanguage.Validate(20,"Đại Phu Ngọc Hư Cung")==0,"Screenshot: an NPC destination cannot be interpreted as VTT eligibility");
Check(BotCommandLanguage.Validate(21,"tôi đưa phù rồi mà")==0&&BotCommandLanguage.Validate(9,"tui đã gửi Di Ngoại Phù rồi nhé")==0,"Delivered-scroll statement cannot replace the active destination or request another trade");
Check(BotCommandLanguage.BareTargetReply("Cửu Linh")&&!BotCommandLanguage.BareTargetReply("đừng đánh Cửu Linh")&&!BotCommandLanguage.BareTargetReply("Cửu Linh ở đâu?"),"Pending combat accepts a bare target name, not a cancellation or location question");
Check(new[]{"không cần nữa","ko cần ông nữa","thôi","khỏi chờ tôi","đừng chờ nữa","tự đi train đi","ông làm việc của ông đi"}.All(t=>BotCommandLanguage.Validate(3,t)==8),"Release requests override a proposed wait command");
Check(BotCommandLanguage.Validate(5,"thôi đi đánh boss")==5&&!BotCommandLanguage.ReleaseRequest("chờ tôi ở đây"),"Explicit wait and a new boss order remain active commands");
Console.WriteLine($"BRAIN VERIFIED {checks} checks passed");

sealed class BotAiHttpFixture(Func<HttpRequestMessage,CancellationToken,Task<HttpResponseMessage>> send):HttpMessageHandler {
    protected override Task<HttpResponseMessage> SendAsync(HttpRequestMessage request,CancellationToken cancellationToken)=>send(request,cancellationToken);
}
