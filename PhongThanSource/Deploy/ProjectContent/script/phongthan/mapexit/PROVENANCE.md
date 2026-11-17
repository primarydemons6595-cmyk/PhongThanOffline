# Fix #1C map-exit recovery provenance

Source package: `Client_Original/data/script.pak`

Source package SHA-256: `DEFABFE3EDDE873012D3C2DA54468DFC57F368724E3795BBDE705704F943CB9D`

| Original entry | Original size | Original SHA-256 | Loose destination | Classification |
| --- | ---: | --- | --- | --- |
| `0x381C646E` | 182 | `474582CB1887AD95B39A288B920348116A48AA931AF346D3F7E201D3578F07C2` | `script/phongthan/mapexit/chongcheng_camp_to_outskirts.lua` | ADAPTED |
| `0x5F310810` | 193 | `F16AB10B368EC5E1F1B14310950208FC9CF47DDB5877549210B6C346D2610F8F` | `script/phongthan/mapexit/chongcheng_outskirts_to_camp.lua` | ADAPTED |

The executable Lua statements and authoritative PT map IDs/coordinates are
preserved. The packed-ID binding is adapted to stable ASCII loose paths, and
the original CP936 comments/player messages are replaced by ASCII provenance
and messages. These files are therefore not claimed to be byte-identical.

## Fix #1D exact-ID batch recovery

The same authoritative package was queried only for the 189 unique packed IDs
referenced by Region_S in the 102 mounted worlds. Exact lookup located 186
entries; three IDs were absent from every original package index inspected:
`0x16B22DC2`, `0x72B0ACD7`, and `0xCA147D6C`.

Of the located callbacks, 152 direct fixed-destination callbacks were copied
byte-for-byte to `script/phongthan/mapexit/recovered/` and classified
`BYTE_IDENTICAL`. Their per-entry packed ID, canonical loose path, provenance,
and SHA-256 are recorded in `registry.tsv`, which is the canonical
machine-readable provenance record for the recovered batch. The existing two
Fix #1C callbacks and Fix #1B SuperTrap remain explicitly `ADAPTED`; no
reconstructed callback or invented world link was added.
