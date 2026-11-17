"""Benchmark real warmed SPR cache accesses; no renderer or game login required."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess


def main():
    root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser()
    parser.add_argument('--before-source', type=Path,
                        help='Optional saved pre-change KImageStore2.cpp')
    parser.add_argument('--runtime', type=Path,
                        default=root.parent / 'PhongThanRuntime-Loose/Client')
    parser.add_argument('--sprites', type=Path, default=root / 'Tests/EquippedSpritePaths.txt')
    args = parser.parse_args()
    vc = Path(os.environ.get('PHONGTHAN_CL',
        r'C:\Program Files (x86)\Microsoft Visual Studio\VC98\Bin\cl.exe')).resolve().parent.parent
    env = dict(os.environ)
    env['INCLUDE'] = str(vc / 'Include')
    env['LIB'] = str(vc / 'Lib')
    env['PATH'] = str(vc / 'Bin') + ';' + str(vc.parent / 'Common/MSDev98/Bin') + ';' + env['PATH']
    output = root / 'Output/Tools/EngineSmoothTests'
    output.mkdir(parents=True, exist_ok=True)
    rep = root / 'Sources/Represent/Represent2'
    flags = [str(vc / 'Bin/cl.exe'), '/nologo', '/MD', '/O2', '/DWIN32', '/DNDEBUG',
             '/I' + str(rep), '/I' + str(root / 'Sources/Engine/Src')]
    sources = [('after', rep / 'KImageStore2.cpp')]
    if args.before_source:
        sources.insert(0, ('before', args.before_source.resolve()))
    shutil.copyfile(args.runtime / 'Engine.dll', output / 'Engine.dll')
    results = []
    for label, source in sources:
        obj = output / (label + 'KImageStore2.obj')
        exe = output / ('SprCache-' + label + '.exe')
        subprocess.run(flags + ['/c', '/Fo' + str(obj), str(source)], env=env, check=True)
        subprocess.run(flags + ['/Fo' + str(output / 'SprCachePerformance.obj'),
            '/Fe' + str(exe), str(root / 'Tests/Native/SprCachePerformance.cpp'), str(obj),
            str(root / 'Sources/Engine/Release/Engine.lib')], env=env, check=True)
        result = subprocess.run([str(exe), str(args.sprites.resolve())],
            cwd=args.runtime.resolve(), capture_output=True, encoding='latin1', check=True)
        results.append(label + ': ' + result.stdout.strip())
    text = '\n'.join(results) + '\n'
    (output / 'real-sprite-benchmark.txt').write_text(text, encoding='utf-8')
    print(text, end='')


if __name__ == '__main__':
    main()
