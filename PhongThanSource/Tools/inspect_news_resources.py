from restore_original_world import ROOT,Original
o=Original()
for scheme in ('ui3','ui4'):
 for name in ('新闻消息来了.ini','系统提示.ini','系统消息.ini','消息窗口界面.ini','滚动显示界面.ini','提示.ini','提示2.ini'):
  got=o.get(('ui/'+scheme+'/'+name).encode('gbk'))
  if got:print(scheme,name,got[1]['id'],got[0][:1300].decode('gbk','replace'))
