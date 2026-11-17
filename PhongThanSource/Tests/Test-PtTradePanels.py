"""Read-only PT resource/loader/panel contract regression checks."""
from pathlib import Path
import configparser
import struct

root = Path(__file__).resolve().parents[2]
client = root / "PhongThanRuntime-Loose/Client"
ui = root / "PhongThanSource/Sources/GameClient/Ui"
checks = 0

def check(value, name):
    global checks
    checks += 1
    assert value, name

for scheme in ("ui3", "ui4"):
    for kind, name in (("trade", "CDE6BCD2BCE4BDBBD2D7"), ("rps", "CBABC8CBBBA5B2A9")):
        path = client / "ui" / scheme / (bytes.fromhex(name).decode("latin1") + ".ini")
        ini = configparser.ConfigParser(strict=False, interpolation=None)
        ini.read_string(path.read_bytes().decode("latin1"))
        check(ini.getint("Main", "left") == 0 and ini.getint("Main", "top") == 116,
              kind + " authentic root geometry")
        for section in ("Main", "OtherName", "SelfItemsBox", "OtherItemsBox", "SelfMoney", "OtherMoney"):
            check(ini.has_section(section), kind + " " + section)
        sprite = client / ini["Main"]["image"].lstrip("\\").replace("\\", "/")
        data = sprite.read_bytes()
        check(data[:4] == b"SPR\0" and all(struct.unpack_from("<HH", data, 4)), kind + " actual background")
        check(ini.getint("SelfItemsBox", "hunits") == 5, kind + " five-column offer")

shortcut = (ui / "ShortcutKey.cpp").read_bytes().decode("latin1")
status_case = shortcut[shortcut.index("case 2:"):shortcut.index("case 3:")]
item_case = shortcut[shortcut.index("case 3:"):shortcut.index("case 4:")]
check("KUiItem::CloseWindow" not in status_case and "KUiStatus::CloseWindow" not in item_case,
      "F3 and F4 do not mutually close")
check("!strcmp(ScriptCommand, SCK_SHORTCUT_ITEMS)" in shortcut and
      "!strcmp(ScriptCommand, SCK_SHORTCUT_STATUS)" in shortcut,
      "only inventory/status scripts bypass active-trade shortcut gate")
notify = (ui / "GameSpaceChangedNotify.cpp").read_bytes().decode("latin1")
start = notify[notify.index("case GDCNI_TRADE_START:"):notify.index("case GDCNI_TRADE_DESIRE_ITEM:")]
check("UiCloseWndsInGame" not in start and "KUiItem::OpenWindow()" in start,
      "trade start preserves panels and opens F4")
trade = (ui / "UiCase/UiTrade.cpp").read_bytes().decode("latin1")
check('"UiTrade.ini"' not in trade, "missing JX trade resource no longer requested")
check("m_bTradeFolkGame && bTraded == 2" in trade, "round notification retains OTT panel")
print(f"PASS {checks} deployed PT trade/panel contract checks")
