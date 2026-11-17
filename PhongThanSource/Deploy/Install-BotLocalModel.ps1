param([string]$RuntimeRoot,[ValidateSet('cpu','gpu-context')][string]$Profile='cpu')
$ErrorActionPreference='Stop'
if(!$RuntimeRoot){throw 'Supply -RuntimeRoot'}
$folder=Join-Path ([IO.Path]::GetFullPath($RuntimeRoot)) 'Server/BotAIgent/LocalAI'
[void][IO.Directory]::CreateDirectory($folder)
$files=@(
 @{Name='llama-b11960-bin-win-cpu-x64.zip';Url='https://github.com/ggml-org/llama.cpp/releases/download/b11960/llama-b11960-bin-win-cpu-x64.zip';Sha='73f962f82850d244db47eaacb9d913637027b82399124324298576d58c5f4287'},
 @{Name='Qwen3-0.6B-Q8_0.gguf';Url='https://huggingface.co/Qwen/Qwen3-0.6B-GGUF/resolve/main/Qwen3-0.6B-Q8_0.gguf';Sha='9465e63a22add5354d9bb4b99e90117043c7124007664907259bd16d043bb031'}
)
if($Profile-eq'gpu-context'){$files=@(
 @{Name='llama-b11960-bin-win-vulkan-x64.zip';Url='https://github.com/ggml-org/llama.cpp/releases/download/b11960/llama-b11960-bin-win-vulkan-x64.zip';Sha='bac6914b5e83523eb008c6789eaa12669f93fbc196c3b9e8237b6672323b33e9'},
 @{Name='Qwen3-1.7B-Q8_0.gguf';Url='https://huggingface.co/Qwen/Qwen3-1.7B-GGUF/resolve/main/Qwen3-1.7B-Q8_0.gguf';Sha='061b54daade076b5d3362dac252678d17da8c68f07560be70818cace6590cb1a'}
)}
foreach($file in $files){$target=Join-Path $folder $file.Name;if(!(Test-Path -LiteralPath $target)){
 & curl.exe --fail --location --silent --show-error --retry 2 --output ($target+'.download') $file.Url
 if($LASTEXITCODE-ne0){throw "Download failed: $($file.Name)"}
 if((Get-FileHash -LiteralPath ($target+'.download')).Hash -ine $file.Sha){throw 'Download checksum mismatch'}
 Move-Item -LiteralPath ($target+'.download') -Destination $target
 };if((Get-FileHash -LiteralPath $target).Hash -ine $file.Sha){throw 'Installed model checksum mismatch'}}
$engine='llama';$model='Qwen3-0.6B-Q8_0';if($Profile-eq'gpu-context'){$engine='llama-vulkan';$model='Qwen3-1.7B-Q8_0'}
$enginePath=Join-Path $folder $engine
if(@(Get-Process llama-server -ErrorAction SilentlyContinue|Where-Object Path -eq (Join-Path $enginePath 'llama-server.exe')).Count-gt0){throw 'Stop the local chat worker before reinstalling its engine; downloaded files were verified and no engine files were changed.'}
Expand-Archive -LiteralPath (Join-Path $folder $files[0].Name) -DestinationPath $enginePath -Force
if($Profile-eq'gpu-context'){@{Profile=$Profile;Model=$model;Files=$files}|ConvertTo-Json -Depth 4|Set-Content -LiteralPath (Join-Path $folder 'context-model-verified.json') -Encoding UTF8}else{if(Test-Path -LiteralPath (Join-Path $folder 'context-model-verified.json')){Remove-Item -LiteralPath (Join-Path $folder 'context-model-verified.json')}}
@{Version=1;Profile=$Profile;Model=$model;Endpoint='http://127.0.0.1:18981/v1/chat/completions';Threads=4;Context=2048;TimeoutSeconds=4;Files=$files}|ConvertTo-Json -Depth 4|Set-Content -LiteralPath (Join-Path $folder 'installation.json') -Encoding UTF8
Write-Output 'Local chat model installed and checksum-verified; it remains optional and disabled until explicitly enabled.'
