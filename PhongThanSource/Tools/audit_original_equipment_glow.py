"""Read original PAK effects and render inspection sheets without runtime edits."""
import colorsys
import hashlib
import json
import re
import struct
import sys
from collections import Counter
from pathlib import Path

from PIL import Image, ImageDraw, ImageOps
from restore_original_world import Original

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'analysis/original_equipment_glow_20261029'


def frame(data, index):
    magic,w,h,cx,cy,frames,colors = struct.unpack_from('<4s6H',data)
    assert magic == b'SPR\0' and index < frames
    table = 32 + colors*3
    offset,size = struct.unpack_from('<II',data,table+index*8)
    block = data[table+frames*8+offset:table+frames*8+offset+size]
    fw,fh,x,y = struct.unpack_from('<4H',block)
    palette = [tuple(data[32+i*3:35+i*3]) for i in range(colors)]
    pixels = []
    pos = 8
    while len(pixels) < fw*fh:
        run,alpha = block[pos:pos+2]
        pos += 2
        assert run and len(pixels)+run <= fw*fh
        for _ in range(run):
            rgb = palette[block[pos]] if alpha else (0,0,0)
            if alpha: pos += 1
            pixels.append(rgb+(alpha,))
    assert pos == len(block)
    image = Image.new('RGBA',(fw,fh));image.putdata(pixels)
    return image,dict(canvas=[w,h],anchor=[cx,cy],offset=[x,y],frames=frames)


def hues(image):
    result = Counter()
    for r,g,b,a in image.getdata():
        if a < 16: continue
        h,s,v = colorsys.rgb_to_hsv(r/255,g/255,b/255)
        if v < .15: continue
        if s < .2: name='white/gray'
        elif h < .04 or h >= .95: name='red'
        elif h < .12: name='orange/gold'
        elif h < .20: name='yellow'
        elif h < .45: name='green'
        elif h < .56: name='cyan'
        elif h < .72: name='blue'
        else: name='purple/pink'
        result[name] += a
    total = sum(result.values())
    return {k:round(v*100/total,1) for k,v in result.most_common()} if total else {}


def main():
    sys.stdout.reconfigure(encoding='utf-8')
    OUT.mkdir(parents=True,exist_ok=True)
    original=Original()
    table,proof=original.get(b'\\settings\\npcres\\zhuang_tai_tu_xing_dui_zhao_biao.txt')
    (OUT/'original-state-table.txt').write_bytes(table)
    rows=table.splitlines()
    # Discover from all original rows. A hand-picked list omitted orange and
    # ordinary immortal/demon set effects in the first audit.
    ids=[]
    for state,row in enumerate(rows[1:],1):
        text=row.decode('gbk','replace')
        if re.search(r'equipeffect|circle-perfect|weaponeffect|套装|装备',text,re.I):
            ids.append(state)
    sheet=Image.new('RGB',(1100,((len(ids)+3)//4)*270),(25,25,30))
    draw=ImageDraw.Draw(sheet)
    records=[]
    for n,state in enumerate(ids):
        fields=rows[state].split(b'\t');path=fields[1]
        row=dict(state=state,path=path.decode('gbk'),comment=fields[-1].decode('gbk'),table_source=proof)
        found=original.get(path)
        col,line=n%4,n//4;x,y=col*275,line*270
        label=path.rsplit(b'\\',1)[1].decode('gbk')
        caption=label if label.isascii() else 'original set effect'
        draw.text((x+8,y+8),str(state)+' '+caption,fill='white')
        if found:
            data,evidence=found
            (OUT/label).write_bytes(data)
            count=struct.unpack_from('<H',data,12)[0]
            # Pick a visible sample; retain all exact SPR bytes and validate
            # every frame. These previews do not establish activation rules.
            choices=[frame(data,i) for i in range(count)]
            image,info=max(choices,key=lambda pair:sum(pair[0].getchannel('A').getdata()))
            image.save(OUT/(label+'.png'))
            row.update(source=evidence,decoded_frames=count,frame=info,visible_hues_percent=hues(image))
            thumb=ImageOps.contain(image,(255,215))
            sheet.paste(thumb,(x+(275-thumb.width)//2,y+30+(215-thumb.height)//2),thumb)
            draw.text((x+8,y+246),', '.join(list(row['visible_hues_percent'])[:2]),fill=(200,200,200))
        else:
            row['missing_sprite']=True
            draw.text((x+8,y+50),'MISSING in active original chain',fill=(255,150,150))
        records.append(row)
    sheet.save(OUT/'original-glow-sheet.png')
    result=dict(original_game_sha256=hashlib.sha256((ROOT/'Client_Original/game.exe').read_bytes()).hexdigest(),
        table=proof,effects=records,note='Sprite existence/palette proves visual assets, not all server activation rules.')
    (OUT/'audit.json').write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n','utf-8')
    for row in records:
        print(row['state'],row['path'],row.get('visible_hues_percent',{}),'missing' if row.get('missing_sprite') else 'decoded')


if __name__=='__main__':main()
