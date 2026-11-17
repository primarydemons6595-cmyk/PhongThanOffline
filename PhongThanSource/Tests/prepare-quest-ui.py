from pathlib import Path
root=Path(__file__).resolve().parents[1]
out=root/'Output/Tools'; out.mkdir(parents=True,exist_ok=True)
src=(root/'Sources/GameClient/Ui/UiCase/UiTaskNote.cpp').read_bytes().decode('latin1')
def body(signature):
    start=src.index(signature); pos=src.index('{',start); depth=1; end=pos+1
    while depth:
        depth+=(src[end]=='{')-(src[end]=='}'); end+=1
    return src[start:end]+'\n'
(out/'QuestUiInput.inl').write_text(''.join(body(s) for s in [
    'int KWndQuestTracker::PtInWindow(', 'bool KWndQuestTracker::ScrollAt(',
    'bool KUiTaskNote::ScrollTrackerAt(']))
# Guard the actual installation points, not just the isolated methods.
assert 'm_Tracker.SetPosition(0,top)' in src
assert 'm_TrackerScroll.SetPosition(0,0)' in src
assert 'w->SetPosition(part?26:18,max(0,drawTop))' in src
assert 'PhongThanAnchorNativePanel(ini,false);' in src
dispatch=(root/'Sources/GameClient/Ui/Elem/Wnds.cpp').read_bytes().decode('latin1')
assert '''if (uMsg == WM_MOUSEWHEEL && !s_WndStation.pCaptureMouseWnd &&
            !s_WndStation.pExclusiveWnd[0] &&
            KUiTaskNote::ScrollTrackerAt(x,y,(short)HIWORD(uParam),pActiveWnd))''' in dispatch
print('Prepared production quest input methods and verified wiring')
