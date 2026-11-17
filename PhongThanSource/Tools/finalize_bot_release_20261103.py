from pathlib import Path
from extract_client_coordinate_dictionary import encode_tcvn3
r=Path(__file__).resolve().parents[1]/'Sources/Core/Src'
p=r/'PhongThanBotAIgent.inl';d=p.read_bytes();a=b'int command=PTBotReleaseCommand(commandQuery)?PTBC_CANCEL:forcedSemantic?forcedCommand:PTBotCommandMode(commandQuery);'
assert d.count(a)==1
if b'CancelQueuedCompanionChats(from,bot-bots)' not in d:d=d.replace(a,a+b'if(command==PTBC_CANCEL)CancelQueuedCompanionChats(from,bot-bots);');p.write_bytes(d)
p=r/'PhongThanBotCompanionChat.inl';d=p.read_bytes();start=d.index(b'if(mode==PTBC_CANCEL)');at=d.index(b'lstrcpynA(answer,"',start)+len(b'lstrcpynA(answer,"');end=d.index(b'",size)',at)
raw=encode_tcvn3('Ok, tui quay lại làm việc của tui nhé. Cần thì gọi.');replacement=''.join('\\%03o'%c for c in raw).encode('ascii');d=d[:at]+replacement+d[end:];p.write_bytes(d)
p=r/'PhongThanThuongChuTgpt.inl';d=p.read_bytes()
for old,text in [('Vat To Thuong','Vật Tổ Thương'),('Vat To Chu','Vật Tổ Chu'),('Tham Quan Thuong','Thám Quân Thương'),('Tham Quan Chu','Thám Quân Chu'),('Thong Soai Thuong','Thống Soái Thương'),('Thong Soai Chu','Thống Soái Chu'),('Co Thuong','Cờ Thương'),('Co Chu','Cờ Chu'),('Van Trong','Văn Trọng'),('Khuong Thuong','Khương Thượng')]:
 d=d.replace(('"'+old+'"').encode(),('"'+''.join('\\%03o'%c for c in encode_tcvn3(text))+'"').encode())
p.write_bytes(d)
