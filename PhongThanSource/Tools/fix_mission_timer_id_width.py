from restore_original_world import ROOT
p=ROOT/'PhongThanSource/Sources/Core/Src/KMission.h';data=p.read_bytes()
for name in ('StartTimer','StopTimer','GetTimerRestTimer'):
 old=f'{name}(unsigned char ucTimerId'.encode();new=f'{name}(unsigned short ucTimerId'.encode()
 assert old in data or new in data,name
 data=data.replace(old,new)
p.write_bytes(data)
print('Mission timer IDs preserve all 16 bits, including 3001..3009.')
