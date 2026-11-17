import json,re,struct,hashlib,sys
from pathlib import Path
from restore_original_world import ROOT,OUT
from extract_client_coordinate_dictionary import decode_tcvn3

data=(ROOT/'Client_Original/engine.dll').read_bytes();pe=struct.unpack_from('<I',data,60)[0]
sections=struct.unpack_from('<H',data,pe+6)[0];optional=struct.unpack_from('<H',data,pe+20)[0]
base=struct.unpack_from('<I',data,pe+52)[0]
def offset(rva):
    for i in range(sections):
        p=pe+24+optional+40*i;size,va,length,pos=struct.unpack_from('<IIII',data,p+8)
        if va<=rva<va+max(size,length):return pos+rva-va
    raise ValueError(rva)
rva=int(sys.argv[1],16) if len(sys.argv)>1 else 0xc1cb;p=offset(rva)
if data[p]==233:rva+=5+struct.unpack_from('<i',data,p+1)[0]
print('LOCALIZE_FUNCTION',hex(base+rva))
for address in (0x10244d20,0x10244c60):
    try:
        locator=struct.unpack_from('<I',data,offset(address-base)-4)[0]
        descriptor=struct.unpack_from('<I',data,offset(locator-base)+12)[0]
        begin=offset(descriptor-base)+8
        print('HASH_RTTI',hex(address),data[begin:data.index(b'\0',begin)])
    except (ValueError,struct.error):pass
with (OUT/'engine_original_disassembly.txt').open(encoding='utf-16',errors='replace') as stream:
    remaining=0
    for line in stream:
        if re.match(r'\s*'+f'{base+rva:08X}'+':',line):remaining=190
        if remaining:
            print(line.rstrip());remaining-=1
            if not remaining:break
raw=(OUT/'localize_vietnamese.expanded.txt').read_bytes();values={}
for line in raw.splitlines()[1:]:
    if b'\t' in line:
        key,text=line.split(b'\t',1);values[key.decode('ascii','replace')]=decode_tcvn3(text)
for name in ('仓库管理员','铜匠','passerby001','passerby003','商汤'):
    found=[]
    for encoding in ('gbk','utf8','utf-16le'):
        md5=hashlib.md5(name.encode(encoding)).hexdigest().upper()
        for key in (md5[:16],md5[16:],bytes.fromhex(md5[:16])[::-1].hex().upper(),bytes.fromhex(md5[16:])[::-1].hex().upper()):
            if key in values:found.append((encoding,key,values[key]))
        key=hashlib.shake_128(name.encode(encoding)).hexdigest(8).upper()
        if key in values:found.append(('SHAKE128_'+encoding,key,values[key]))
    print('HASH',name,found)
print('WAREHOUSE_KEYS',[(k,v) for k,v in values.items() if v.lower() in ('thủ khố','thợ đồng','thương thang')])
