"""Read-only checks against actual deployed PT resources, not synthetic INIs."""
from pathlib import Path
import configparser
import re
import struct
ROOT = Path(__file__).resolve().parents[2]
CLIENT = ROOT / "PhongThanRuntime-Loose/Client"
UI = ROOT / "PhongThanSource/Sources/GameClient/Ui/UiCase"
checks = 0
def check(value, label):
    global checks
    checks += 1
    assert value, label
def load(hex_name):
    path = CLIENT / "ui/ui3" / (bytes.fromhex(hex_name).decode("latin1") + ".ini")
    parser = configparser.ConfigParser(strict=False, interpolation=None)
    parser.read_string(path.read_bytes().decode("latin1"))
    return parser
def sprite(ini, section):
    path = CLIENT / ini[section]["image"].lstrip("\\").replace("\\", "/")
    check(path.is_file(), section + " SPR exists")
    data = path.read_bytes()
    check(data[:4] == b"SPR\x00", section + " SPR header")
    w, h = struct.unpack_from("<HH", data, 4)
    check(w > 0 and h > 0, section + " SPR dimensions")
parade = load("BFB4B6D4B7BDCDE6BCD2D7B0B1B8")
slots = re.findall(r'\{UIEP_\w+,\s*"([^"]+)"\}', (UI/"UiParadePtSlots.inl").read_text())
check(len(slots) == len(set(slots)) == 12, "12 unique PT slots")
for section in slots:
    check(parade.has_section(section), section + " resource section")
    check(parade.getint(section,"Width") > 0 and parade.getint(section,"Height") > 0,
          section + " nonzero geometry")
    sprite(parade, section)
for section in ("Main", "ImgBackground", "Close"):
    sprite(parade, section)
check(parade.getint("Main", "Left") == 207 and parade.getint("Main", "Top") == 116,
      "authentic root position")
loader = (UI/"UiParadePtCompat.inl").read_text()
check('Init(ini, "Main")' in loader and '"Male"' not in loader, "PT not JX root")
rps = load("CBABC8CBBBA5B2A9")
for section in ("Main","BtnStone","BtnForfex","BtnCloth","BtnLock","BtnConfirm","BtnClose"):
    sprite(rps,section)
check([rps.getint(s,"Top") for s in ("BtnStone","BtnForfex","BtnCloth")] == [153,185,218],
      "original RPS positions")
print(f"PASS: {checks} deployed PT resource checks")
