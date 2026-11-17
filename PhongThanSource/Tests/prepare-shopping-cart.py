from pathlib import Path
root=Path(__file__).resolve().parents[1]
out=root/'Output/Tools';out.mkdir(parents=True,exist_ok=True)
def body(path,signature):
    s=(root/path).read_bytes().decode('latin1');start=s.index(signature)
    end=s.index('{',start)+1;depth=1
    while depth:
        depth+=(s[end]=='{')-(s[end]=='}');end+=1
    return s[start:end]+'\n'
src=body('Sources/GameClient/Ui/Elem/AutoLocateWnd.cpp','void ALW_GetWndPosition(')
src+=body('Sources/GameClient/Ui/UiCase/UiSuperShop.cpp','void KUiShoppingCart::Show()')
(out/'ShoppingCartPosition.inl').write_text(src)
