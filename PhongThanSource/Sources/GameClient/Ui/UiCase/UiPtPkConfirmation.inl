// Compatible binding to an existing original PT Main/Info/FirstBtn/SecondBtn
// layout (8DF7BF5A). This is not a recovered original PK dialog implementation.
bool KUiInformation::PreparePtPkConfirmation()
{
    KIniFile ini;
    if (!ini.Load("\\Ui\\ui3\\UiPkConfirmation.ini") && !ini.LoadPakEntry(0x8DF7BF5AUL))
        return false;
    if (!KWndShowAnimate::Init(&ini,"Main") || !m_Information.Init(&ini,"Info")) return false;
    // Legacy pure-text buttons return false even after successful Init.
    // Initialize both, then validate their actual geometry below.
    m_FirstBtn.Init(&ini,"FirstBtn");
    m_SecondBtn.Init(&ini,"SecondBtn");
    int infoW,infoH,firstW,firstH,secondW,secondH;
    m_Information.GetSize(&infoW,&infoH);
    m_FirstBtn.GetSize(&firstW,&firstH); m_SecondBtn.GetSize(&secondW,&secondH);
    if (m_Width<=0 || m_Height<=0 || infoW<=0 || infoH<=0 ||
        firstW<=0 || firstH<=0 || secondW<=0 || secondH<=0) return false;
    m_FirstBtn.GetPosition(&m_nOrigFirstBtnXPos,0);
    m_nCentreBtnXPos=(m_Width-firstW)/2;
    int screenW,screenH; Wnd_GetScreenSize(screenW,screenH);
    SetPosition((screenW-m_Width)/2,(screenH-m_Height)/2);
    return true;
}

void KUiInformation::ShowPtRevival(KWndWindow* caller,unsigned int param){
    if(!PreparePtPkConfirmation())return;
    KIniFile ini;if(!ini.Load("\\Ui\\ui3\\UiPkConfirmation.ini")&&!ini.LoadPakEntry(0x8DF7BF5AUL))return;
    m_ThirdBtn.Init(&ini,"SecondBtn");
    SetSize(433,120);m_Information.SetPosition(27,16);m_Information.SetSize(377,74);
    m_FirstBtn.SetPosition(42,91);m_SecondBtn.SetPosition(180,91);m_ThirdBtn.SetPosition(318,91);
    m_nOrigFirstBtnXPos=42;
    m_Information.SetFontSize(16);
    Show("\102\271\156\040\256\267\040\142\336\040\074\143\157\154\157\162\075\162\145\144\076\164\162\344\156\147\040\164\150\255\254\156\147\074\143\157\154\157\162\076\056\012\103\150\344\156\040\074\143\157\154\157\162\075\147\162\145\145\156\076\126\322\040\164\150\265\156\150\074\143\157\154\157\162\076\040\256\323\040\166\322\040\156\254\151\040\144\255\354\156\147\040\163\370\143\056\012\074\143\157\154\157\162\075\147\162\145\145\156\076\124\150\325\040\155\271\156\147\074\143\157\154\157\162\076\072\040\074\143\157\154\157\162\075\171\145\154\154\157\167\076\061\040\103\150\165\351\143\040\110\345\156\040\103\150\251\165\074\143\157\154\157\162\076\054\040\147\151\367\040\256\151\323\155\040\113\116\056\012\074\143\157\154\157\162\075\147\162\145\145\156\076\120\150\364\143\040\163\151\156\150\074\143\157\154\157\162\076\072\040\074\143\157\154\157\162\075\171\145\154\154\157\167\076\061\040\130\165\074\143\157\154\157\162\076\054\040\163\350\156\147\040\154\271\151\040\164\271\151\040\143\150\347\056","\126\322\040\164\150\265\156\150","\124\150\325\040\155\271\156\147",caller,param,-1,-1);
    m_ThirdBtn.SetText("Phôc sinh");m_ThirdBtn.Show();
    int w,h;Wnd_GetScreenSize(w,h);SetPosition((w-m_Width)/2,(h-m_Height)/2);
}
