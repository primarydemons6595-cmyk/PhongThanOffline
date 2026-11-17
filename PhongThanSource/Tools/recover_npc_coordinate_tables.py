"""Fetch published NPC coordinate facts, preserving source/version and conflicts.

Historical Chinese tables are discovery/corroboration, never silently treated
as current VNG server coordinates. Only explicit reviewed facts are published.
"""
import concurrent.futures,html,json,re,time,urllib.request,urllib.parse
from pathlib import Path
from html.parser import HTMLParser
from restore_original_world import ROOT,OUT,path_bytes

SINA='https://games.sina.com.cn/z/fs/2005-03-15/'
PAGES={1002:'212898',1003:'212905',1004:'212909',1020:'212913',1021:'212917',
       'doctors':'213000',1052:'213002',1001:'213004','battle':'213011','other':'213017'}


class Page(HTMLParser):
    def __init__(self,text):
        super().__init__();self.rows=[];self.links=[];self.parts=[];self.row=None;self.cell=None;self.ignore=0
        self.feed(text)
    def handle_starttag(self,tag,attrs):
        attrs=dict(attrs)
        if tag in ('script','style'):self.ignore+=1
        if tag=='a' and 'href' in attrs:self.links.append(attrs['href'])
        if tag=='tr':self.row=[]
        if tag in ('td','th') and self.row is not None:self.cell=[]
    def handle_endtag(self,tag):
        if tag in ('script','style'):self.ignore=max(0,self.ignore-1)
        if tag in ('td','th') and self.cell is not None:
            self.row.append(' '.join(self.cell).strip());self.cell=None
        if tag=='tr' and self.row is not None:self.rows.append(self.row);self.row=None
        if tag in ('p','li','br','div','tr'):self.parts.append(' ')
    def handle_data(self,data):
        if self.ignore:return
        if data.strip():self.parts.append(data.strip())
        if self.cell is not None:self.cell.append(data.strip())


def fetch(url):
    cache=OUT/'web_sources';cache.mkdir(exist_ok=True)
    target=cache/(re.sub(r'[^a-zA-Z0-9._-]','_',url)+'.html')
    if target.exists():return target.read_bytes()
    request=urllib.request.Request(url,headers={'User-Agent':'Mozilla/5.0'})
    data=urllib.request.urlopen(request,timeout=25).read();target.write_bytes(data);return data


def historical():
    facts=[];failures=[]
    def read(item):
        world,page=item;url=SINA+page+'.shtml'
        soup=Page(fetch(url).decode('gb18030','replace'))
        records=[]
        for cells in soup.rows:
            if len(cells)!=4 or not cells[2].isdigit() or not cells[3].isdigit():continue
            x,y=int(cells[2]),int(cells[3])
            if not(80<=x<=500 and 80<=y<=500):continue
            records.append(dict(world=world,role=cells[0],original_name=cells[1],x=x,y=y,
                source=url,source_date='2005-03-15',classification='HISTORICAL_EXPLICIT_COORDINATE'))
        return records
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
        futures={pool.submit(read,item):item for item in PAGES.items()}
        for f,item in futures.items():
            try:facts.extend(f.result())
            except Exception as exc:failures.append(dict(page=item,error=str(exc)))
    catalog=json.loads((ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/coordinates.json').read_text('utf8'))
    comparisons=[]
    for world,items in catalog['maps'].items():
        for row in items:
            if not row[1]:continue
            raw=path_bytes(row[1])
            matches=[f for f in facts if f['world']==int(world) and path_bytes(f['original_name'])==raw]
            if matches:
                comparisons.append(dict(world=int(world),name=row[0],current=[row[2],row[3]],
                    published=[[f['x'],f['y']] for f in matches],sources=sorted({f['source'] for f in matches}),
                    agrees=any([f['x'],f['y']]==[row[2],row[3]] for f in matches)))
    result=dict(facts=facts,comparisons=comparisons,failures=failures,
        policy='Do not replace VNG coordinates with older Chinese coordinates on disagreement')
    (OUT/'historical_npc_coordinates.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),'utf8')
    print('HISTORICAL_COORDINATES',len(facts),'MATCHED_CURRENT',len(comparisons),'CONFLICTS',sum(not c['agrees'] for c in comparisons),flush=True)


def vng():
    root='https://phongthan.zing.vn'
    seeds=[root+'/cam-nang/gioi-thieu/tam-gioi-'+n+'.html' for n in ('giap-si','dao-si','di-nhan')]
    links=set(seeds)
    for url in seeds:
        soup=Page(fetch(url).decode('utf8','replace'))
        links.update(urllib.parse.urljoin(root,link) for link in soup.links
            if '/cam-nang/' in link and link.endswith('.html'))
    facts=[];failures=[]
    def read(url):
        soup=Page(fetch(url).decode('utf8','replace'))
        text=' '.join(soup.parts)
        records=[]
        for m in re.finditer(r'(?<!\d)(\d{3})\s*[,/:.]\s*(\d{3})(?!\d)',text):
            x,y=map(int,m.groups())
            if not(100<=x<=400 and 100<=y<=400):continue
            records.append(dict(x=x,y=y,source=url,context=text[max(0,m.start()-220):m.end()+140],
                classification='VNG_EXPLICIT_DISPLAY_COORDINATE_CONTEXT_REQUIRES_IDENTITY_MATCH'))
        return records
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
        futures={pool.submit(read,url):url for url in sorted(links)}
        for f,url in futures.items():
            try:facts.extend(f.result())
            except Exception as exc:failures.append(dict(source=url,error=str(exc)))
    (OUT/'vng_coordinate_evidence.json').write_text(json.dumps(dict(pages=len(links),facts=facts,failures=failures),ensure_ascii=False,indent=2),'utf8')
    print('VNG_COORDINATE_FACTS',len(facts),'PAGES',len(links),'FAILURES',len(failures),flush=True)


if __name__=='__main__':historical();vng()
