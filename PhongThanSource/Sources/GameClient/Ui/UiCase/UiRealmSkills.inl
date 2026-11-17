static int PtRealmSkillState()
{
    PTRoleValues role;memset(&role,0,sizeof(role));
    return g_pCoreShell&&g_pCoreShell->GetGameData(PT_GDI_ROLE_VALUES,(unsigned int)&role,sizeof(role))&&
        PhongThanRealmUnlocked(role.RealmStage,role.RealmSide,role.RealmLevel)?role.RealmSide:0;
}
static void PtPrepareRealmSkillIni(KIniFile& ini)
{
    const int profession=CurrentSkillProfession();if(profession<0||profession>3)return;
    const int base=profession==1?4:profession==3?3:2;
    ini.WriteInteger("Main","PageCount",base+(PtRealmSkillState()?1:0));
    char page[40];sprintf(page,"SkillPage_%d",base);
    ini.WriteInteger(page,"SkillCount",PT_REALM_SKILL_COUNT);
    KIniFile original;if(!original.Load("\\ui\\realm_skills.ini"))return;
    const char* ints[]={"Left","Top","DummyWnd","ImageLeft","ImageTop","ImageWidth","ImageHeight",
        "SkillNameTextLeft","SkillNameTextTop","SkillNameTextWidth","LevelTextLeft","LevelTextTop","LevelTextWidth","LevelTextHeight","Up","Down","Width","Height","CheckBox","CheckOver","Over","OverFrame","DisableFrame"};
    const char* strings[]={"Image","Tip","SkillName"};
    for(int n=-2;n<PT_REALM_SKILL_COUNT;++n){
        char section[64],source[64];
        if(n==-2){sprintf(section,"SkillPage_%d",base);sprintf(source,"Realm_%d_Page",profession);}
        else if(n==-1){sprintf(section,"PageBtn_%d",base);sprintf(source,"Realm_%d_Button",profession);}
        else{sprintf(section,"Page_%d_Skill_%d",base,n);sprintf(source,"Realm_%d_Skill_%d",profession,n);}
        for(int k=0;k<sizeof(ints)/sizeof(ints[0]);++k){int value;if(original.GetInteger(source,ints[k],0,&value))ini.WriteInteger(section,ints[k],value);}
        for(k=0;k<sizeof(strings)/sizeof(strings[0]);++k){char value[256];if(original.GetString(source,strings[k],"",value,sizeof(value)))ini.WriteString(section,strings[k],value);}
    }
    ini.WriteInteger(page,"SkillCount",PT_REALM_SKILL_COUNT);
    // Keep the extra tab clear of four Taoist / three warrior tabs.
    if(PtRealmSkillState()&&(profession==1||profession==3)){
        for(int tab=0;tab<base;++tab){char key[40];sprintf(key,"PageBtn_%d",tab);ini.WriteInteger(key,"Top",34);
            if(profession==1){sprintf(key,"SkillPage_%d",tab);ini.WriteInteger(key,"Top",49);}}
        if(profession==1){ini.WriteInteger(page,"Top",49);for(int skill=0;skill<PT_REALM_SKILL_COUNT;++skill){char key[64];sprintf(key,"Page_%d_Skill_%d",base,skill);ini.WriteInteger(key,"Top",25+(skill/2)*48);}}
    }
    KTabFile skills;if(!skills.Load("\\settings\\Skills.txt"))return;
    for(int slot=0;slot<PT_REALM_SKILL_COUNT;++slot){
        char section[64],name[80];sprintf(section,"Page_%d_Skill_%d",base,slot);
        const int id=ptRealmSkills[profession][slot];ini.WriteInteger(section,"SkillID",id);
        for(int row=2;row<=skills.GetHeight();++row){int candidate;skills.GetInteger(row,"SkillId",0,&candidate);
            if(candidate==id){skills.GetString(row,"SkillName","",name,sizeof(name));ini.WriteString(section,"SkillName",name[0]=='#'?name+1:name);break;}}
    }
}
