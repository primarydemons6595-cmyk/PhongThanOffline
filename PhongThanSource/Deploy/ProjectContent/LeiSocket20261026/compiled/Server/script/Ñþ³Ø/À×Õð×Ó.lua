-- Reconstructed socket branch from current original PAK tables.
-- Source evidence and limitations: Docs/LEI_SOCKET_RESTORE_20261026.md.
function main()
 SayButtons("Ta cã thÓ gióp ng­¬i ®ôc lç, kh¶m n¹m vµ trang trİ vò khİ, trang bŞ. Ng­¬i cÇn gióp ®iÒu g×?",3,
 "§ôc lç - Trang trİ/socket", "Kh¶m n¹m/inlay", "ThuyÕt minh/help")
end
function socket()
 CloseDialog()
 EnchaseItem(2)
end
function inlay()
 SayButtons("Ng­¬i muèn kh¶m vµo lç nµo? NÕu lç ®· cã ngäc, kh¶m thµnh c«ng sÏ thay thÕ ngäc cò.",4,
 "Lç thø nhÊt/slot1", "Lç thø hai/slot2", "Lç thø ba/slot3", "Lç thø t­/slot4")
end
function slot1() CloseDialog() EnchaseItem(3) end
function slot2() CloseDialog() EnchaseItem(4) end
function slot3() CloseDialog() EnchaseItem(5) end
function slot4() CloseDialog() EnchaseItem(6) end
function help()
 Talk(1,"main","§Æt mét trang bŞ cïng nguyªn liÖu vµo cöa sæ GhĞp. KiÓm tra phİ tr­íc khi thùc hiÖn.\n§ôc lç: dïng Hoµng Thñy Tinh t­¬ng øng víi thø tù lç.\nTrang trİ: dïng Tö Thñy Tinh t­¬ng øng; cã thÓ ®Æt thªm Bİ Ng©n. Ngäc ®· kh¶m ®­îc gi÷ l¹i khi trang trİ lç.\nKh¶m: chän vŞ trİ lç, ®Æt ngäc vµ nguyªn liÖu kh¶m t­¬ng øng.\nSè lç tèi ®a phô thuéc lo¹i, phÈm chÊt vµ ®¼ng cÊp trang bŞ. Trªn vËt phÈm, sè tr­íc dÊu / lµ sè lç ®· kh¶m, sè sau lµ sè lç ®· ®ôc.")
end
