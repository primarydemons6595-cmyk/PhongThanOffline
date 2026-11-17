from pathlib import Path
root=Path(__file__).resolve().parents[1]
out=root/'Output/Tools';out.mkdir(parents=True,exist_ok=True)
text=(root/'Sources/Core/Src/KNpc.cpp').read_bytes().decode('latin1')
start=text.index('static BOOL CallNpcLevelNumber(')
end=text.index('static int ResolveNpcSkillId(',start)
(out/'SummonNpcStatLoader.inl').write_bytes(text[start:end].encode('latin1'))
start=text.index('BOOL KNpc::CheckHitTarget(');end=text.index('\nvoid KNpc::GetNpcCopyFromTemplate',start)
(out/'SummonHitCheck.inl').write_bytes(text[start:end].replace('KNpc::CheckHitTarget','HitProbe::CheckHitTarget').encode('latin1'))
start=text.index('        if (factionPvp && nType!=damage_magic)')
end=text.index('\n\t\t///end fix TamLTM',start)
(out/'SummonPvpPolicy.inl').write_bytes(text[start:end].encode('latin1'))
client=(root/'Sources/Core/Src/KNpcSet.cpp').read_bytes().decode('latin1')
start=client.index('struct PT_CLIENT_SUMMON_OWNER');end=client.index('int PhongThanSummonRelation(',start)
(out/'SummonClientCache.inl').write_bytes(client[start:end].encode('latin1'))
ui=(root/'Sources/GameClient/Ui/UiCase/UiPtCompanionHud.inl').read_bytes().decode('latin1')
start=ui.index('int KPtCompanionHud::WndProc(')
(out/'SummonHudButton.inl').write_bytes(ui[start:].encode('latin1'))
import re
config=dict(re.findall(r'root.WriteInteger\("Main","([^"]+)",(\d+)\)',ui))
(out/'SummonHudRootConfig.inl').write_text('\n'.join('#define HUD_ROOT_%s %s'%p for p in config.items()))
window=(root/'Sources/GameClient/Ui/Elem/WndWindow.cpp').read_bytes().decode('latin1')
image=(root/'Sources/GameClient/Ui/Elem/WndImage.cpp').read_bytes().decode('latin1')
def body(source, signature):
    start=source.index(signature); pos=source.index('{',start); depth=1; end=pos+1
    while depth:
        depth += (source[end]=='{')-(source[end]=='}'); end+=1
    return source[start:end]+'\n'
(out/'SummonHudHitTesting.inl').write_bytes((body(window,'int KWndWindow::PtInWindow(')+body(image,'int KWndImage::PtInWindow(')+body(window,'KWndWindow *KWndWindow::TopChildFromPoint(')).encode('latin1'))
print('Prepared unmodified production stat/hit function bodies')
