# SuperTrap provenance

- Source: `Client_Original/data/script.pak`; identified by a targeted lookup of packed entry `0xCE741D0E` against each original package.
- Original packed entry ID: `0xCE741D0E`.
- Destination loose path: `Server/script/phongthan/gate/supertrap.lua`.
- Recovery: byte-identical targeted extraction; 7,144 bytes; SHA-256 `73298AB0F9825D4F8C9B6CEAB3ECC7D089F490622993E14430A10BB729A7601A`.
- Binding adaptation: `KNpc::CheckTrap` maps only original trap ID `0xCE741D0E` to this canonical loose path because PAK indexes do not retain the original filename.
