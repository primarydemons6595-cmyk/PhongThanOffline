"""Generate bounded native commodity rules from verified original table bytes."""
import json
from restore_original_world import ROOT,OUT

folder=OUT/'priority_events'
def rows(eid):return [line.split(b'\t') for line in (folder/(eid+'.txt')).read_bytes().splitlines() if line and line.split(b'\t')[0].isdigit()]
goods=rows('E9627D82');shops=rows('3DB5AAF0');prices=rows('A493650D')
assert len(goods)==27 and len(shops)==9 and len(prices)==243
def number(value):return int(value or b'0')
lines=['// Generated from original client tables; see analysis/restoration_20261016/priority_events/service_tables.json.',
 '#ifndef PHONGTHAN_COMMERCE_DATA_H','#define PHONGTHAN_COMMERCE_DATA_H',
 'struct PhongThanGoodsRule {const char *name;int weight,restock;};',
 'struct PhongThanPriceRule {int reference;int purchase[9],sale[9];};',
 'static const PhongThanGoodsRule pt_goods_rules[27] = {']
for row in goods:
    name=''.join('\\%03o'%c for c in row[1].lstrip(b'#'))
    lines.append('{"%s",%d,%d},'%(name,number(row[2]),number(row[7])))
lines+=['};','static const unsigned long pt_shop_refresh_seconds[9] = {'+','.join(str(number(r[2])) for r in shops)+'};',
 'static const PhongThanPriceRule pt_price_rules[9][27] = {']
for shop in range(1,10):
    lines.append('{')
    records=[r for r in prices if number(r[0])==shop];assert len(records)==27
    for good in range(1,28):
        row=next(r for r in records if number(r[1])==good)
        buy=[number(v) for v in row[3:12]];sale=[number(v) for v in row[12:21]]
        assert 0<=sum(buy[2::3])<=1000 and 0<sum(sale[2::3])<=1000
        for vector in (buy,sale):
            assert all(vector[i]<=vector[i+1] for i in (0,3,6))
        lines.append('{%d,{%s},{%s}},'%(number(row[2]),','.join(map(str,buy)),','.join(map(str,sale))))
    lines.append('},')
lines+=['};','#endif','']
target=ROOT/'PhongThanSource/Headers/PhongThanCommerceData.h';target.write_text('\n'.join(lines),'ascii')
print(json.dumps(dict(goods=len(goods),shops=len(shops),price_rules=len(prices),target=str(target))))
