"""Read the running server's generated catalog; never infer live GUI success."""
import csv,json,collections
from restore_original_world import ROOT,OUT
server=ROOT/'PhongThanRuntime-Loose/Server';batch=OUT/'npc_reward_safety'
ready=server/'native_world_ready.log';catalog=server/'normal_shop_loader_diag.csv'
published=(batch/'publication.json').stat().st_mtime
if min(ready.stat().st_mtime,catalog.stat().st_mtime)<published:
    print('WAITING: current server has not finished loading the world/catalog.');raise SystemExit(2)
rows=list(csv.DictReader(catalog.open()))
goods=[line.split(b'\t') for line in (server/'settings/goods.txt').read_bytes().splitlines()[1:]]
shops=[line.split(b'\t') for line in (server/'settings/buysell.txt').read_bytes().splitlines()[1:]]
errors=[];unsupported=[]
for item in rows:
    shop,slot,row,index=map(int,(item['shop'],item['slot'],item['goods_row'],item['item_index']))
    expected=goods[row-1]
    if int(shops[shop-1][slot])!=row:errors.append(item)
    if index<0:unsupported.append(item)
    elif (int(item['genre']),int(item['detail']))!=(int(expected[0]),int(expected[1])):errors.append(item)
book_shop=[item for item in rows if item['shop']=='14']
assert not errors,errors
assert len(book_shop)==5 and all(int(item['item_index'])>=0 for item in book_shop)
result=dict(status='PASS_SUPPORTED_CATALOG',shops=len({item['shop'] for item in rows}),rows=len(rows),loaded=len(rows)-len(unsupported),
    identity_errors=errors,unsupported=unsupported,unsupported_by_shop=dict(collections.Counter(item['shop'] for item in unsupported)),
    original_book_shop14=book_shop,full_shop_restore=not unsupported,live_gui_acceptance=False)
(batch/'shop_runtime_verification.json').write_text(json.dumps(result,indent=2),'utf8')
print(json.dumps({k:v for k,v in result.items() if k not in ('unsupported','original_book_shop14')}))
