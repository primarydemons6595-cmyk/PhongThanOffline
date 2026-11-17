#pragma once
#include "../Elem/WndShadow.h"
#include "../../../Core/Src/CoreShell.h"
#include "../../../../Headers/PhongThanRoleUi.h"

// The original DisableImg_* controls are shadows, not SPR buttons.
class KPtF4LockedRow : public KWndShadow
{
public:
    virtual int PtInWindow(int x, int y)
    {
        return KWndWindow::PtInWindow(x, y);
    }
    virtual int WndProc(unsigned int message, unsigned int param, int data)
    {
        if (message == WM_LBUTTONDOWN)
        {
            if (GetParent())
                GetParent()->WndProc(WND_N_BUTTON_CLICK,
                    (unsigned int)(KWndWindow*)this, 0);
            return 1;
        }
        return KWndWindow::WndProc(message, param, data);
    }
};

struct KPtF4ResourceControls
{
    KWndButton alchemy, arrange, close;
    KWndText256 boundMoney, strength;
    KPtF4LockedRow rows[7];
    bool locked;
    KPtF4ResourceControls() : locked(false) {}
    void AddTo(KWndWindow* parent)
    {
        parent->AddChild(&alchemy);
        parent->AddChild(&arrange);
        parent->AddChild(&close);
        parent->AddChild(&boundMoney);
        parent->AddChild(&strength);
        for (int i = 0; i < 7; ++i)
            parent->AddChild(&rows[i]);
    }
    void Init(KIniFile* ini)
    {
        alchemy.Init(ini, "BtnAlchemy");
        arrange.Init(ini, "ClearStoreBtn");
        close.Init(ini, "CloseBtn2");
        boundMoney.Init(ini, "BindMoney");
        strength.Init(ini, "Weight");
        // Neither authoritative PT value is exposed by this reconstructed
        // core. Do not relabel Xu, JX stamina or a fabricated zero as PT data.
        boundMoney.SetText("--");
        strength.SetText("--/--");
        for (int i = 0; i < 7; ++i)
        {
            char section[32];
            sprintf(section, "DisableImg_%d", i);
            rows[i].Init(ini, section);
            rows[i].Hide();
        }
        locked = false;
    }
    void UpdateValues(iCoreShell* core)
    {
        PTRoleValues values;memset(&values,0,sizeof(values));
        if(core && core->GetGameData(PT_GDI_ROLE_VALUES,(unsigned int)&values,sizeof(values))) {
            boundMoney.Set3IntText(values.BoundMoney);
            strength.Set2IntText(values.CarryWeight,values.CarryCapacity,'/');
            boundMoney.BringToTop();strength.BringToTop();
        }
    }
    void SetRows(int allowed)
    {
        locked=allowed==0;
        for(int i=0;i<7;++i)if(i>=allowed) {rows[i].Show();rows[i].BringToTop();} else rows[i].Hide();
    }
    void SetLocked(bool value)
    {
        locked = value;
        for (int i = 0; i < 7; ++i)
            if (value) rows[i].Show(); else rows[i].Hide();
    }
    bool IsRow(unsigned int window)
    {
        for (int i = 0; i < 7; ++i)
            if (window == (unsigned int)(KWndWindow*)&rows[i]) return true;
        return false;
    }
};
