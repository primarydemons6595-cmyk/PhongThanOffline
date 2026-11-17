#include <assert.h>
#include <stdio.h>

struct Scene {
    bool m_bPreprocessEvent, treeReady, requestDuringBuild;
    int builds;
    Scene():m_bPreprocessEvent(false),treeReady(true),requestDuringBuild(false),builds(0){}
    void CrossRegion(){treeReady=false;m_bPreprocessEvent=true;}
    void Preprocess(){treeReady=true;++builds;if(requestDuringBuild){m_bPreprocessEvent=true;requestDuringBuild=false;}}
    void Prepare(){
#include "../../Sources/Core/Src/Scene/PhongThanScenePreparePaint.inl"
    }
};
int Run(int fps,bool fix){
    Scene s;int blanks=0,logic=0;
    for(int frame=0;frame<fps*10;++frame){
        int now=frame*1000/fps;
        while(logic*55<=now){s.Prepare();++logic;}
        // This is the ordering used by CoreShell: sample camera AFTER logic,
        // potentially cross a region, then paint the scene in the same frame.
        if(frame%17==3)s.CrossRegion();
        if(fix)s.Prepare();
        if(!s.treeReady)++blanks;
    }
    return blanks;
}
int main(){
    for(int fps=30;fps<=54;fps+=24){
        int old=Run(fps,false),fixed=Run(fps,true);
        assert(old>0&&fixed==0);
        printf("PASS %dFPS/18Hz: empty scene frames before=%d after=%d\n",fps,old,fixed);
    }
    Scene s;s.Prepare();assert(s.builds==0); // idle does not rebuild
    s.CrossRegion();s.Prepare();s.Prepare();assert(s.builds==1);
    s.CrossRegion();s.requestDuringBuild=true;s.Prepare();assert(s.m_bPreprocessEvent);
    s.Prepare();assert(!s.m_bPreprocessEvent&&s.builds==3);
    puts("PASS: paint boundary consumes pending work, idle avoids duplicate rebuild, new work retained");
    return 0;
}
