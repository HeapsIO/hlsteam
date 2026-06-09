hlsteam
-------
Simple Haxe wrapper of Steam API for Hashlink.

### Setup

- Download [Steam SDK](https://partner.steamgames.com/downloads/list) (version: `1.64`)
- Put your Steam SDK in the hlsteam/sdk folder
- Define `HASHLINK_SRC` env var to point to your `hashlink` directory
- Compile `steam.hdll`
  - Windows use `hlsteam.sln`
  - Linux/MacOS: run `make`
- Distribute `steam.hdll` with `sdk/redistributable_bin/win64/steam_api64.dll` (or `libsteam_api.so`/`libsteam_api.dylib`)

#### Current Features:

- Achievements & Leaderboards
- Cloud
- Workshop / UGC (User Generated Content)
- Matchmaking / Networking
- Steam Controller Support (not well-tested)
- Stats (not well-tested)

#### Dependencies / Requirements:

- Haxe
- Hashlink

