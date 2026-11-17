"""Restore original marquee layouts and scheduled server notices."""
import json,re
from restore_original_world import ROOT,OUT,Original
from extract_client_coordinate_dictionary import decode_tcvn3
import build_offline_loose as b
original=Original();folder=OUT/'server_news';folder.mkdir(exist_ok=True)
server=ROOT/'PhongThanRuntime-Loose/Server';client=ROOT/'PhongThanRuntime-Loose/Client'
if (folder/'publication.json').exists():raise ValueError('Already applied; preserve the publication journal')
reverse={c:i for i,c in enumerate(decode_tcvn3(bytes(range(256))))}
def encode(s):return bytes(reverse.get(c,ord(c) if ord(c)<128 else 63) for c in s)
def quote(s):return '"'+''.join('\\%03d'%n if n<32 or n>=127 else ('\\'+chr(n) if n in (34,92) else chr(n)) for n in encode(s))+'"'
prepared=json.loads((OUT/'prepared_changes.json').read_text('utf8'))
names={'混沌':'Hỗn Độn','穷奇':'Cùng Kỳ','饕餮':'Thao Thiết','梼杌':'Đào Ngột','九婴':'Cửu Anh','盘古':'Bàn Cổ','大鹏':'Đại Bằng','杨戬':'Dương Tiễn','蛟龙':'Giao Long','螭龙':'Ly Long','不义侯':'Bất Nghĩa Hầu','虬龙':'Cầu Long'}
bossrows=[]
for row in prepared['boss_schedule']:
 name=row['path'].replace('\\','/').split('/')[-1].replace('.lua','')
 bossrows.append('{%d,%s,%d,%d}'%(row['hour']*60+row['minute'],quote(names.get(name,name)),row['flag'],row['content_map']))
source=b'''-- Project server heartbeat: original tips + warnings from the active schedule.
-- UTC+7 is explicit. Dedupe each real UTC minute; do not replay missed warnings.
PT_NEWS_LAST_MINUTE=-1
PT_NEWS_BOSSES={'''+','.join(bossrows).encode('ascii')+b'''}
PT_NEWS_VTT={140,620,800,1100,1280,1400}
'''
tips=(client/'ui/DefaultMessage.ini').read_bytes()
messages=[v.lstrip(b'$') for k,v in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',tips)]
assert len(messages)>=40
# Use the original client text. Server owns the rotation, so all players receive it.
source+=b'PT_NEWS_TIPS={'+b','.join(b'"'+m.replace(b'\\',b'\\\\').replace(b'"',b'\\"')+b'"' for m in messages)+b'}\n'
source+=('PT_NEWS_VTT_WARNING='+quote('[Vạn Tiên Trận] Còn ')+'\nPT_NEWS_VTT_SUFFIX='+quote(' phút mở đăng ký bốn trận Thổ, Thủy, Hỏa, Phong. Hãy đến cổng phù hợp cấp độ; đăng ký trong 5 phút.')+'\nPT_NEWS_BOSS_WARNING='+quote('[Boss] Còn ')+'\nPT_NEWS_BOSS_SUFFIX='+quote(' phút đến lịch xuất hiện. Hãy chuẩn bị tổ đội!')+'\nPT_NEWS_CLOSED='+quote('[Vạn Tiên Trận] Đã hết giờ đăng ký cho người vào mới. Người đã báo danh có thể tiếp tục trận.')+'\n').encode('ascii')
source+=('PT_NEWS_BOSS_TITLE='+quote('Boss Hoàng Kim - ')+'\n').encode('ascii')
source+=b'''
function main()
    local utc=floor(SystemTime()/60)
    if utc<=PT_NEWS_LAST_MINUTE then return end
    PT_NEWS_LAST_MINUTE=utc
    local minute=mod(utc+420,1440)
    local emitted=0
    for i=1,getn(PT_NEWS_VTT) do
        local diff=mod(PT_NEWS_VTT[i]-minute+1440,1440)
        if diff==10 or diff==5 or diff==1 then
            AddNews(PT_NEWS_VTT_WARNING..diff..PT_NEWS_VTT_SUFFIX,1)
            emitted=1
        end
        -- This is phase-specific: do not announce combat after a missed/restarted opening.
        if mod(minute-PT_NEWS_VTT[i]+1440,1440)==5 then
            if GetGlobalValue(1)==2 or GetGlobalValue(2)==2 or GetGlobalValue(3)==2 or GetGlobalValue(4)==2 then
                AddNews(PT_NEWS_CLOSED,1);emitted=1
            end
        end
    end
    for i=1,getn(PT_NEWS_BOSSES) do
        local row=PT_NEWS_BOSSES[i]
        local diff=mod(row[1]-minute+1440,1440)
        if (diff==5 or diff==1) and GetGlobalValue(row[3])==-1 then
            AddNews("<color=yellow>"..PT_NEWS_BOSS_TITLE..row[2].."<color>: "..PT_NEWS_BOSS_WARNING..diff..PT_NEWS_BOSS_SUFFIX,1)
            emitted=1
        end
    end
    if emitted==0 and mod(minute,3)==0 then
        local index=mod(floor(utc/3),getn(PT_NEWS_TIPS))+1
        AddNews(PT_NEWS_TIPS[index],1)
    end
end
'''
changes={'script/servertimer.lua':source}
# Short-lived 1-second messages could expire before one complete marquee pass.
# Keep original source prefixes byte-identical; adjust only the authored lifecycle.
for i in range(1,5):
 rel=f'script/phongthan/van_tien/mission{i:02d}.lua';data=(server/rel).read_bytes()
 marker=b'-- Project lifecycle reconstruction; original payload retained above.'
 prefix,tail=data.split(marker,1);tail=tail.replace(b'AddGlobalCountNews(',b'AddNews(')
 changes[rel]=prefix+marker+tail
files=[]
for relative,data in changes.items():
 p=folder/'staged/Server'/relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
 files.append(dict(relative=relative,sha256=b.sha(data),before_sha256=b.sha((server/relative).read_bytes())))
(folder/'plan.json').write_text(json.dumps(dict(ready=[],blocked=[],files=files),indent=2),'utf8')
clientfiles=[]
for scheme in ('ui3','ui4'):
 got=original.get(('ui/'+scheme+'/新闻消息来了.ini').encode('gbk'))
 assert got
 for n in ('UiNewsMessage.ini','UiNewsMessage2.ini'):
  data=got[0]
  if n.endswith('2.ini'):data=re.sub(rb'(?m)^Top=\d+',b'Top=72',data)
  relative=f'ui/{scheme}/{n}';p=folder/'staged/Client'/relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
  target=client/relative
  clientfiles.append(dict(relative=relative,sha256=b.sha(data),before_sha256=b.sha(target.read_bytes()) if target.exists() else None,source=got[1],adaptation='ASCII resource binding; second news channel shifted to y=72'))
(folder/'client_plan.json').write_text(json.dumps(clientfiles,ensure_ascii=False,indent=2),'utf8')
print('STAGED server_files=5 client_layouts=4 original_tips=%d boss_slots=%d'%(len(messages),len(bossrows)))
