"""Stage Loose-only merchant and daily acceptance repairs; never publish on import."""
from pathlib import Path
import hashlib,json,sys
from extract_client_coordinate_dictionary import encode_tcvn3,decode_tcvn3
ROOT=Path(__file__).resolve().parents[2]
LIVE=ROOT/'PhongThanRuntime-Loose/Server'
PACKAGE=ROOT/'PhongThanSource/Deploy/ProjectContent/QuestServices20261029'
REPORT=ROOT/'analysis/quest_services_20261029'
def enc(s):
    out=bytearray()
    for c in s:
        try:out.extend(encode_tcvn3(c))
        except KeyError:out.extend(encode_tcvn3(c.lower()))
    return bytes(out)
def build():
    REPORT.mkdir(parents=True,exist_ok=True)
    files=[]
    def write(rel,data):
        p=PACKAGE/'compiled/Server'/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
        files.append(dict(role='Server',relative=rel,before=hashlib.sha256((LIVE/rel).read_bytes()).hexdigest(),sha256=hashlib.sha256(data).hexdigest()))
    rel='script/phongthan/commerce/runtime.lua'
    text=decode_tcvn3((LIVE/rel).read_bytes())
    text=text.replace('function PTTradeNear()','PTTradeContacts={}\nfunction PTTradeNear()')
    text=text.replace('local npc=GetNpcIdx()','local contact=PTTradeContacts[PlayerIndex or 0]\n local npc=contact and contact.npc or 0\n if not contact or contact.owner~=PlayerId or GetNpcID(npc)~=contact.id then return nil end')
    text=text.replace('GetTask(60)==1','GetTask(60)>=1 and GetTask(60)<=9')
    text=text.replace('function main()\n if not PTTradeNear()', '''function main(npc)
 if not npc or npc<=0 or GetNpcTemplateID(npc)~=158 then CloseDialog();return end
 PTTradeContacts[PlayerIndex or 0]={npc=npc,id=GetNpcID(npc),owner=PlayerId}
 if not PTTradeNear()''')
    write(rel,enc(text))
    merchants=json.loads((ROOT/'analysis/bot_aigent_20261029/commerce_manifest.json').read_text('utf8'))['merchants']
    for m in merchants:
        rel=m['script'];data=(LIVE/rel).read_bytes()
        # Outer four originals leave RandomTask (row 5) hidden even for a camel owner.
        old=b'if (GetLevel()>=25) then\r\n\t\t\t\tif (GetTask(333)>0) then'
        if old not in data:old=old.replace(b'\r\n',b'\n')
        assert data.count(old)==1,rel
        if m['shop']>=6:
            data=data.replace(b'tasks[5].show=0',b'tasks[5].show=1')
        write(rel,data)
    rel='script/phongthan/daily/runtime.lua'
    text=decode_tcvn3((LIVE/rel).read_bytes())
    text+='''
-- Shared acceptance feedback: denied callbacks must leave a visible dialog.
function PTDailyNotice(text) Talk(1,"no",text) end
function PTDailyAcceptReady(kind,level,near)
 if not near then PTDailyNotice("Hãy đến gần NPC nhận nhiệm vụ rồi thử lại.");return nil end
 if GetTrade()~=0 or GetLockState()~=0 then PTDailyNotice("Hãy kết thúc giao dịch và mở khóa hành trang trước khi nhận nhiệm vụ.");return nil end
 if GetLevel()<level then PTDailyNotice("Cần đạt cấp "..level.." để nhận nhiệm vụ này.");return nil end
 if PTDailyGet(kind,0)>0 and PTDailyGet(kind,3)<SystemTime() then PTDailyCancel(kind) end
 if PTDailyGet(kind,0)>0 then PTDailyJournal(kind);PTDailyNotice("Bạn đang làm nhiệm vụ này. Hãy hoàn thành hoặc hủy nhiệm vụ trước.");return nil end
 if not PTDailyCanStart(kind,level,4) then PTDailyNotice("Hôm nay bạn đã nhận đủ 4 lượt nhiệm vụ này. Hãy quay lại vào ngày mai.");return nil end
 return 1
end
'''
    write(rel,enc(text))
    rel='script/phongthan/daily/tu_linh.lua';text=decode_tcvn3((LIVE/rel).read_bytes())
    text=text.replace('local phase=PTDailyGet(5,0)','local phase=PTDailyGet(5,0)\n if phase>0 and PTDailyGet(5,3)<SystemTime() then PTDailyCancel(5);phase=0 end')
    text=text.replace('PTDailyNear(1021,214*256,195*512) or PTDailyNear(1020,175*256,188*512)','PTDailyNear(1021,54912,100096) or PTDailyNear(1020,44672,96512)')
    text=text.replace('if not PTDailySeerNear() or not PTDailyCanStart(5,65,4) then return end','''if not PTDailyAcceptReady(5,65,PTDailySeerNear()) then return end
 local fee=50000+floor((GetLevel()-65)/10)*50000
 if HaveNormalItem(3,82,0,0)<1 then PTDailyNotice("Mở Linh Tê môn cần 1 Sơn thạch trong hành trang. Hãy chuẩn bị rồi quay lại.");return end
 if GetCash()<fee then PTDailyNotice("Mở Linh Tê môn cần "..fee.." lượng. Bạn chưa đủ tiền.");return end''')
    text=text.replace('PTDailyJournal(5);pt_daily_tu_go()','PTDailyJournal(5);pt_daily_tu_go()\n else PTDailyNotice("Chưa thể nhận Tứ Tượng Linh Tê. Kiểm tra Sơn thạch trong túi chính, tiền và khóa vật phẩm; chưa trừ lượt nhận.")')
    write(rel,enc(text))
    rel='script/phongthan/daily/thien_cuong.lua';text=decode_tcvn3((LIVE/rel).read_bytes())
    text=text.replace('local phase=PTDailyGet(10,0)','local phase=PTDailyGet(10,0)\n if phase>0 and PTDailyGet(10,3)<SystemTime() then PTDailyCancel(10);phase=0 end')
    text=text.replace('PTDailyNear(1001,184*256,193*512)','PTDailyNear(1001,47216,99056)')
    start=text.index('function pt_daily_cuong_take()');end=text.index('function pt_daily_cuong_claim()')
    text=text[:start]+'''function pt_daily_cuong_take()
 if not PTDailyAcceptReady(10,75,PTDailyCuongNear()) then return end
 local choices={}
 for i=1,getn(PT_DAILY_DATA.targets) do local p=PT_DAILY_DATA.targets[i];if p.map>=1042 and p.map<=1051 then tinsert(choices,i) end end
 if getn(choices)==0 then PTDailyNotice("Chưa có khu vực Thiên Cương khả dụng. Chưa thu phí hay trừ lượt nhận.");return end
 if HaveNormalItem(3,114,0,0)<1 or HaveNormalItem(3,115,0,0)<1 then PTDailyNotice("Nhận Thiên Cương Hồn cần 1 Thu thập và 1 Tứ Tinh trong hành trang. Hãy chuẩn bị đủ vật phẩm rồi quay lại.");return end
 local fee=GetLevel()*1000
 if GetCash()<fee then PTDailyNotice("Nhận Thiên Cương Hồn cần "..fee.." lượng. Bạn chưa đủ tiền.");return end
 if PTDailyStart(10,{{3,114,0,0,0,0,1},{3,115,0,0,0,0,1}},{},fee) then
  PTDailySet(10,6,choices[random(1,getn(choices))]);PTDailyJournal(10)
  PTDailyNotice("Đã nhận Thiên Cương Hồn. Xem sổ nhiệm vụ để đến khu vực được chỉ định; thời hạn 30 phút.")
 else PTDailyNotice("Chưa thể nhận Thiên Cương Hồn. Kiểm tra vật phẩm trong túi chính, tiền và khóa vật phẩm; chưa trừ lượt nhận.") end
end
'''+text[end:]
    write(rel,enc(text))
    (PACKAGE/'manifest.json').write_text(json.dumps(dict(schema=1,files=files),indent=2),'utf8')
    print(json.dumps(dict(files=len(files),package=str(PACKAGE))))
if __name__=='__main__':build()
