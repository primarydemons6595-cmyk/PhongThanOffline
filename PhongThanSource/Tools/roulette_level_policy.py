"""Shared authored minimum for the restored roulette NPCs."""
from extract_client_coordinate_dictionary import encode_tcvn3

def apply_level_20(payload):
    data=payload.replace(b'\r\n',b'\n')
    data=data.replace(b'GetLevel()>=40',b'GetLevel()>=20').replace(b'GetLevel()<40',b'GetLevel()<20')
    data=data.replace(b'return 0,40',b'return 0,20').replace(b'return 1,40',b'return 1,20').replace(b'return 3,40',b'return 3,20')
    data=data.replace(encode_tcvn3('cấp 40'),encode_tcvn3('cấp 20'))
    intro=encode_tcvn3('Càn Khôn Luân giúp bạn thử vận may. Đạt <color=green>cấp 20<color> có thể xoay; vòng quay chỉ dừng khi bạn bấm Dừng.')
    data=data.replace(b'SayTask(11079,tasks)',b'SayTask("'+intro+b'",tasks)')
    guard=b'\n    if GetLevel()<20 then Talk(1,"no","'+encode_tcvn3('Cần đạt cấp 20 để xoay Càn Khôn Luân.')+b'");return end'
    for name in (b'yes_2',b'yes_3',b'rolling'):
        marker=b'function '+name+b'()'
        data=data.replace(marker,marker+guard) if marker+guard not in data else data
    return data
