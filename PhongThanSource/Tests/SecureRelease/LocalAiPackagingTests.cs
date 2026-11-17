using PhongThanStudio.Core;
using PhongThan.Event.Runtime;
using System.Text.Json;
static class LocalAiPackagingTests {
 public static void Run(string root,Action<bool,string> check){
  string source=Path.Combine(root,"ai-source"),target=Path.Combine(root,"ai-target"),folder=Path.Combine(source,"BotAIgent","LocalAI");
  Directory.CreateDirectory(Path.Combine(folder,"llama"));
  File.WriteAllText(Path.Combine(folder,"Qwen3-0.6B-Q8_0.gguf"),"GGUF fixture");File.WriteAllText(Path.Combine(folder,"llama","llama-server.exe"),"fixture exe");File.WriteAllText(Path.Combine(folder,"llama","llama.dll"),"fixture dependency");
  foreach(string privateFile in new[]{"brain.db","bots.json","ai-key.dpapi"})File.WriteAllText(Path.Combine(source,"BotAIgent",privateFile),"private state");
  File.WriteAllText(Path.Combine(folder,"llama-server.log"),"private prompt");File.WriteAllText(Path.Combine(folder,"llama","llama-bench.exe"),"not needed");
  var included=LocalAiRelease.Export(source,target,true);
  check(included.Included&&included.Files.Count==3&&included.Bytes>0&&BotLocalChat.Installed(Path.Combine(target,"BotAIgent")),"included AI package contains model, server and its DLL and is detected by runtime");
  check(!Directory.EnumerateFiles(target,"*",SearchOption.AllDirectories).Any(f=>new[]{"brain.db","bots.json","ai-key.dpapi","llama-server.log","llama-bench.exe"}.Contains(Path.GetFileName(f))),"AI package excludes private memory, profiles, credentials, logs and unnecessary executables");
  Directory.CreateDirectory(Path.Combine(target,"CharacterStore"));File.WriteAllText(Path.Combine(target,"CharacterStore","preserved.pthc"),"keep");
  var omitted=LocalAiRelease.Export(source,target,false);
  check(!omitted.Included&&omitted.Files.Count==0&&LocalAiRelease.Files(target).Length==0&&File.Exists(Path.Combine(target,"CharacterStore","preserved.pthc")),"unchecking AI removes only staged AI files and preserves unrelated state");
  Directory.CreateDirectory(Path.Combine(target,"BotAIgent","LocalAI","llama"));File.WriteAllText(Path.Combine(target,"BotAIgent","LocalAI","Qwen3-0.6B-Q8_0.gguf"),"old model");File.WriteAllText(Path.Combine(target,"BotAIgent","LocalAI","llama","llama-server.exe"),"old binary");
  check(!BotLocalChat.Installed(Path.Combine(target,"BotAIgent")),"omit manifest disables a leftover model when applying a patch");
  File.WriteAllText(Path.Combine(target,"BotAIgent","local-ai.package.json"),"invalid json");check(!BotLocalAiPackage.Enabled(Path.Combine(target,"BotAIgent")),"corrupt AI package policy cannot silently enable a module");
  LocalAiRelease.Export(source,target,true);check(BotLocalChat.Installed(Path.Combine(target,"BotAIgent")),"including AI again restores model availability");
  bool rejected=false;try{LocalAiRelease.Export(source,source,false);}catch(InvalidDataException){rejected=true;}check(rejected,"packaging cannot remove its live source module");
 }
}
