#define main ExistingEquipmentColumnTestMain
#include "EquipmentColumnTests.cpp"
#undef main
int main(int argc,char** argv){
    assert(argc==3);int count=0;
    for(int t=0;t<2;++t){KTabFile table;assert(table.Load(argv[t+1]));
        for(int row=2;row<=(int)table.rows.size();++row){
            KBASICPROP_EQUIPMENT record;ZeroMemory(&record,sizeof(record));
            PhongThanReadEquipmentColumns(&table,row,0,&record);
            const std::vector<std::string>& raw=table.rows[row-1];const int first=t==0?53:44;
            for(int r=0;r<6;++r){assert(record.m_aryPropReq[r].nType==atoi(raw[first+r*2].c_str()));assert(record.m_aryPropReq[r].nPara==atoi(raw[first+r*2+1].c_str()));}
            assert(record.m_nWeight==atoi(raw[13].c_str()));++count;
        }
    }
    assert(count==431);puts("PASS original 380 instrument / 51 jade pendant production header reader requirements weight flags");return 0;
}
