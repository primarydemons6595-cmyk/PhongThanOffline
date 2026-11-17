
#ifndef COREUSENAMEDEF_H
#define COREUSENAMEDEF_H

#define		MAX_PLAYER_IN_ACCOUNT			3

#define		SETTING_PATH					"\\settings"


#define		NPCNAME_KEY						"<npc>"
#define		PLAYERNAME_KEY					"<player>"
#define		SEXNAME_KEY						"<sex>"

#define 	STRANGER_UNITNAME 				"[Ng­êi l¹]"
#define 	FRIEND_INDEX					0
#define 	FRIEND_UNITNAME 				"H¶o h÷u"
#define 	ENEMY_INDEX						1
#define 	ENEMY_UNITNAME	 				"Cõu nh©n"
#define 	ADDENEMY_UNITNAME	 			"Cõu nh©n\n"
#define		BLACKLIST_UNITNAME				"Sæ ®en"
#define		BLACKLIST_INDEX					2
//---------------------------- npc res ???? ------------------------------
#define		RES_INI_FILE_PATH				"\\settings\\npcres"
#define		RES_NPC_STYLE_PATH				"\\spr\\npcres\\style"
#define		RES_SOUND_FILE_PATH				"sound"

#define		NPC_RES_KIND_FILE_NAME			"\\settings\\npcres\\\xC8\xCB\xCE\xEF\xC0\xE0\xD0\xCD.txt"
#define		NPC_NORMAL_RES_FILE				"\\settings\\npcres\\\xC6\xD5\xCD\xA8npc\xD7\xCA\xD4\xB4.txt"
#define		NPC_NORMAL_SPRINFO_FILE			"\\settings\\npcres\\\xC6\xD5\xCD\xA8npc\xD7\xCA\xD4\xB4\xD0\xC5\xCF\xA2.txt"
#define		STATE_MAGIC_TABLE_NAME			"\\settings\\npcres\\\xD7\xB4\xCC\xAC\xCD\xBC\xD0\xCE\xB6\xD4\xD5\xD5\xB1\xED.txt"
#define		PLAYER_RES_SHADOW_FILE			"\\settings\\npcres\\\xD6\xF7\xBD\xC7\xB6\xAF\xD7\xF7\xD2\xF5\xD3\xB0\xB6\xD4\xD3\xA6\xB1\xED.txt"
#define		NPC_RES_SHADOW_FILE				"\xC6\xD5\xCD\xA8npc\xB6\xAF\xD7\xF7\xD2\xF5\xD3\xB0\xB6\xD4\xD3\xA6\xB1\xED.txt"
#define		PLAYER_SOUND_FILE				"\\settings\\npcres\\\xD6\xF7\xBD\xC7\xB6\xAF\xD7\xF7\xC9\xF9\xD2\xF4\xB1\xED.txt"
#define		NPC_SOUND_FILE					"\\settings\\npcres\\npc\xB6\xAF\xD7\xF7\xC9\xF9\xD2\xF4\xB1\xED.txt"

#define		NPC_ACTION_NAME					"npc\xB6\xAF\xD7\xF7\xB1\xED.txt" //settings npcres
#define		ACTION_FILE_NAME				"\xB6\xAF\xD7\xF7\xB1\xE0\xBA\xC5\xB1\xED.txt"

#define		PLAYER_MENU_STATE_RES_FILE		"\\settings\\npcres\\\xBD\xE7\xC3\xE6\xD7\xB4\xCC\xAC\xD3\xEB\xCD\xBC\xD0\xCE\xB6\xD4\xD5\xD5\xB1\xED.txt"
#define		PLAYER_INSTANT_SPECIAL_FILE		"\\settings\\npcres\\\xCB\xB2\xBC\xE4\xCC\xD8\xD0\xA7.txt"

#define		SPR_INFO_NAME					"\xD7\xCA\xD4\xB4"
#define		SPR_EFFECT_NAME					"_effect"
#define		KIND_NAME_SECT					"CharacterType"
#define		KIND_NAME_SPECIAL				"SpecialNpc"
#define		KIND_NAME_NORMAL				"NormalNpc"
#define		KIND_FILE_SECT1					"PartFileName"
#define		KIND_FILE_SECT2					"WeaponActionTab1"
#define		KIND_FILE_SECT3					"WeaponActionTab2"
#define		KIND_FILE_SECT4					"ActionRenderOrderTab"
#define		KIND_FILE_SECT5					"ResFilePath"

//--------------------------- player ???????? ------------------------------
// ?????????????????????????? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ??????????

//--------------------------- player PK???? ------------------------------
#define		defPK_PUNISH_FILE				"\\settings\\npc\\player\\PKPunish.txt"

//--------------------------- player ???????? ------------------------------
#define		defTONG_PARAM_FILE				"\\settings\\tong\\Tong_Setting.ini"
#define		defTONG_LEVEL_FILE				"\\settings\\tong\\Tong_Level.txt"
#define		defTONG_DATA_FILE				"\\settings\\tong\\Tong_Data.ini"

//---------------------------- player ???????? ------------------------------
#define		PLAYER_LEVEL_EXP_FILE			"\\settings\\npc\\player\\level_exp.txt"
#define		PLAYER_LEVEL_ADD_FILE			"\\settings\\npc\\player\\level_add.txt"
#define		PLAYER_LEVEL_LEAD_EXP_FILE		"\\settings\\npc\\player\\level_lead_exp.txt"
#define		PLAYER_MAGIC_LEVEL_EXP_FILE		"\\settings\\npc\\player\\magic_level_exp.txt"
#define		BASE_ATTRIBUTE_FILE_NAME		"\\settings\\npc\\player\\NewPlayerBaseAttribute.ini"
#define		PLAYER_PK_SET_FILE				"\\settings\\npc\\PKSet.ini"
#define		PLAYER_BASE_VALUE				"\\settings\\npc\\player\\BaseValue.ini"
#define		PLAYER_STAMINA_FILE_NAME		"\\settings\\npc\\player\\stamina.ini"

// 00???? 01???? 02???? 03???? 04???? 05???? 06???? 07???? 08???? 09????
#define		NEW_PLAYER_INI_FILE_NAME		"\\settings\\npc\\player\\NewPlayerIni%02d.ini"

//---------------------------- player ???????? ------------------------------
#define		BUYSELL_FILE					"\\settings\\buysell.txt"
#define		GOODS_FILE						"\\settings\\goods.txt"

//----------------------------- ???????? ------------------------------
#define		PHONGTHAN_APPEARANCE_MELEE_FILE			"\\settings\\item\\VNG_WeaponPart.txt"
#define		PHONGTHAN_APPEARANCE_RANGE_FILE			"\\settings\\item\\VNG_WeaponPart.txt"
#define		PHONGTHAN_APPEARANCE_ARMOR_FILE			"\\settings\\item\\VNG_ArmorPart.txt"
#define		PHONGTHAN_APPEARANCE_HELM_FILE				"\\settings\\item\\VNG_ArmorPart.txt"
#define		PHONGTHAN_APPEARANCE_HORSE_FILE			"\\settings\\item\\VNG_HorsePart.txt"
#define		PHONGTHAN_APPEARANCE_MASK_FILE				"\\settings\\item\\MaskRes.txt"
#define		ITEM_ABRADE_FILE				"\\settings\\item\\AbradeRate.ini"

//--------------- npc skill missles ?????????????????????? -------------
#define		SKILL_SETTING_FILE				"\\settings\\Skills.txt"
#define		MISSLES_SETTING_FILE			"\\settings\\Missles.txt"
#define		NPC_SETTING_FILE				"\\settings\\phongthan\\Npcs.txt"
#define		NPC_NAME_FILE					"\\settings\\NpcName.txt"

//---------------------------- object ???? ------------------------------
#define		OBJ_DATA_FILE_NAME		"\\settings\\obj\\ObjData.txt"
#define		MONEY_OBJ_FILE_NAME		"\\settings\\obj\\MoneyObj.txt"
#define		OBJ_NAME_COLOR_FILE		"\\settings\\obj\\ObjNameColor.ini"

//---------------------------- ?????????? -------------------------------
#define		defLITTLE_MAP_SET_FILE	"\\Ui\\Default\\??????????.ini"

//----------------------------- ???????? --------------------------------
#define		defINSTANT_SOUND_FILE	"\\settings\\SoundList.txt"
#define		defMUSIC_SET_FILE		"\\settings\\music\\MusicSet.txt"
#define		defMUSIC_FIGHT_SET_FILE	"\\settings\\music\\MusicFightSet.ini"

//------------------------------------------------------------------------
#define NPC_TEMPLATE_BINFILEPATH		"\\settings"
#define NPC_TEMPLATE_BINFILE			"NpcTemplate.Bin"
#define WEAPON_PHYSICSSKILLFILE			"\\settings\\ClientWeaponSkill.txt"
#define WEAPON_PARTICULARTYPE			"ParticularType"
#define	WEAPON_DETAILTYPE				"DetailType"
#define	WEAPON_SKILLID					"PhysicsSkillID"
//-----------------------------------------------------------------------
#define	WORLD_WAYPOINT_TABFILE			"\\settings\\WayPoint.txt"
#define WORLD_STATION_TABFILE			"\\settings\\Station.txt"
#define WORLD_STATIONPRICE_TABFILE		"\\settings\\StationPrice.txt"
#define WORLD_WAYPOINTPRICE_TABFILE		"\\settings\\WayPointPrice.txt"
#define WORLD_DOCK_TABFILE				"\\settings\\Wharf.txt"
#define WORLD_DOCKPRICE_TABFILE			"\\settings\\WharfPrice.txt"
#define STRINGRESOURSE_TABFILE			"\\settings\\StringResource.txt"
#define PLAYER_RANK_SETTING_TABFILE		"\\settings\\RankSetting.txt"
//---------------------------------????--------------------------
#define		MAGICATTRIB_LEVEL_FILE		"\\settings\\item\\magicattriblevel.txt"
#define		MAGICATTRIB_LVINDEX_FILE	"\\settings\\item\\magicattriblevel_index.txt"

#define		MAPLIST_SETTING_FILE		"\\settings\\WorldSet.ini"

#define		PLATINA_EQUIP_FILE			"\\settings\\item\\platinaequip.txt"
#define		GOLD_EQUIP_FILE				"\\settings\\item\\goldequip.txt"
#define		GOLD_EQUIP_MAGIC_FILE		"\\settings\\item\\magicattrib_ge.txt"

#define		GM_SCRIPT					"\\script\\gmscript.lua"
#define		SERVERTIMER_SCRIPT			"\\script\\servertimer.lua"
#define		SERVERSTARTUP_SCRIPT		"\\script\\serverscript.lua"

#define		LOGIN_SCRIPT				"\\script\\player\\playerlogin.lua"
#define		LEVELUP_SCRIPT				"\\script\\player\\playerlevelup.lua"
#define		LOGOUT_SCRIPT				"\\script\\player\\playerlogout.lua"
#define		SCRIPT_PROTOCOL_FILE		"\\script\\global\\script_protocol.lua"

#define		MINIMAP_SETTING_FILE_INI	"\\settings\\MapTraffic.ini"
#define 	GAME_DATA_FILE_INI			"\\settings\\GameData.ini" // TamLTM note Luu du lieu player trong game
#define 	GAME_SETTING_FILE_INI		"\\settings\\GameSetting.ini"

#define		UNKNOWNITEM_SPR				"\\spr\\item\\unknownitem.spr"
#define		UNKNOWNITEM_SPR36			"\\spr\\item\\unknownitem36.spr"
#define		RESIZEITEM_SPR				"\\spr\\item\\twzhuanyun\\zhuanyunbao_big.spr"

#define		TREMBLE_ITEM_SCRIPT			"\\script\\item\\khamxanh\\tremble.lua" //TamLTM kham nam xanh
#define		BUILD_ITEM_SCRIPT			"\\script\\item\\khamxanh\\builditem.lua" //TamLTM kham nam xanh

#define		SUPPERSHOP_CARD_SCRIPT		"\\script\\cardshop\\card.lua" //TamLTM suppershop card

#define		NORMAL_UNCLEAR_WORD			"Ch­a râ"
#define		NORMAL_FUNCTION_NAME		"main"
//---------------------------- ???????? ------------------------------
// ????????????????????????????????32?????????? %d %s ????????????????????????
#define		MSG_GET_EXP						"B¹n nhËn ®­îc <color=green>%d <color>®iÓm kinh nghiÖm"
#define		MSG_DEC_EXP						"B¹n tæn thÊt <color=green>%d <color>®iÓm kinh nghiÖm"
#define		MSG_LEVEL_UP					"§¼ng cÊp ®¹t %d cÊp"
#define		MSG_LEADER_LEVEL_UP				"§¼ng cÊp thèng so¸i cña b¹n ®¹t cÊp %d"
#define		MSG_GET_ATTRIBUTE_POINT			"B¹n cã %d ®iÓm tiÒm n¨ng"
#define		MSG_GET_SKILL_POINT				"B¹n cã %d ®iÓm kü n¨ng"
#define		MSG_GET_ATTRIBUTE_SKILL_POINT	"B¹n cã %d ®iÓm tiÒm n¨ng %d ®iÓm kü n¨ng"

#define		MSG_TEAM_SEND_INVITE			"B¹n göi ®Õn %s lêi mêi gia nhËp tæ ®éi"
#define		MSG_TEAM_GET_INVITE				"§éi tr­ëng %s göi lêi mêi gia nhËp tæ ®éi ®Õn b¹n"
#define		MSG_TEAM_REFUSE_INVITE			"Ng­êi ch¬i %s tõ chèi lêi gia nhËp tæ ®éi cña nhãm b¹n"
#define		MSG_TEAM_APPLY_INVITE_FAIL		"Nhãm mµ b¹n võa ®ång ý gia nhËp kh«ng tån t¹i"
#define		MSG_TEAM_APPLY_INVITE_FAIL2		"Nhãm mµ b¹n võa ®ång ý gia nhËp ®· ®ãng chøc n¨ng chiªu mé"
#define		MSG_TEAM_CREATE					"B¹n s¸ng lËp nªn mét nhãm míi"
#define		MSG_TEAM_CREATE_FAIL			"S¸ng lËp nhãm thÊt b¹i."
#define		MSG_TEAM_CREATE_FAIL1			"PK m« h×nh bang chiÕn kh«ng thÓ s¸ng lËp nhãm míi!"
#define		MSG_TEAM_CANNOT_CREATE			"B¹n hiÖn giê kh«ng thÓ lËp nhãm!"
#define		MSG_TEAM_TARGET_CANNOT_ADD_TEAM	"§èi ph­¬ng hiÖn giê kh«ng thÓ lËp nhãm!"
#define		MSG_TEAM_IS_FULL				"Nhãm mµ b¹n xin phÐp gia nhËp ®· ®ñ sè l­îng"
#define		MSG_TEAM_OPEN					"Nhãm cña b¹n hiÖn t¹i cã thÓ tiÕp nhËn thµnh viªn míi"
#define		MSG_TEAM_CLOSE					"Nhãm cña b¹n hiÖn t¹i kh«ng thÓ tiÕp nhËn thµnh viªn míi"
#define		MSG_TEAM_MODEPICK_SELF			"Tù nhÆt"
#define		MSG_TEAM_MODEPICK_FREE			"Tù do nhÆt"
#define		MSG_TEAM_MODEPICK_CAPTAIN		"§éi tr­ëng nhÆt"
#define		MSG_TEAM_MODEPICK_ALTERNATE		"Lu©n phiªn nhÆt"
#define		MSG_TEAM_PKFOLLOW_OPEN			"B¹n më chÕ ®é PK theo nhãm"
#define		MSG_TEAM_PKFOLLOW_CLOSE			"B¹n ®ãng chÕ ®é PK theo nhãm"
#define		MSG_TEAM_ADD_MEMBER				"%s trë thµnh b»ng h÷u cña nhãm B¹n."
#define		MSG_TEAM_SELF_ADD				"B¹n ®· tham gia nhãm cña %s"
#define		MSG_TEAM_ADDMEMBER_NOTVALID		"B¹n kh«ng thÓ tham gia nhãm cña ng­êi ch¬i míi"
#define		MSG_TEAM_DISMISS_CAPTAIN		"B¹n ®· gi¶i t¸n nhãm cña m×nh"
#define		MSG_TEAM_DISMISS_MEMBER			"%s gi¶i t¸n nhãm!"
#define		MSG_TEAM_KICK_ONE				"%s bÞ khai trõ khái nhãm"
#define		MSG_TEAM_BE_KICKEN				"B¹n bÞ khai trõ khái nhãm"
#define		MSG_TEAM_APPLY_ADD				"%s xin phÐp gia nhËp nhãm!"
#define		MSG_TEAM_APPLY_ADD_SELF_MSG		"B¹n göi lêi xin phÐp gia nhËp tæ ®éi cña %s"
#define		MSG_TEAM_LEAVE					"%s rêi khái nhãm."
#define		MSG_TEAM_LEAVE_SELF_MSG			"B¹n ®· rêi khái nhãm cña %s"
#define		MSG_TEAM_CHANGE_CAPTAIN_FAIL1	"§¶m nhËn nhãm tr­ëng thÊt b¹i!"
#define		MSG_TEAM_CHANGE_CAPTAIN_FAIL2	"%s kh«ng ®ñ søc thèng so¸i!"
#define		MSG_TEAM_CHANGE_CAPTAIN_FAIL3	"Kh«ng thÓ giao chøc vô ®éi tr­ëng cho ng­êi míi ch¬i"
#define		MSG_TEAM_CHANGE_CAPTAIN			"%s ®­îc ®¶m nhËn lµ nhãm tr­ëng"
#define		MSG_TEAM_CHANGE_CAPTAIN_SELF	"B¹n ®­îc %s ®¶m nhËn lµ nhãm tr­ëng"
#define		MSG_TEAM_AUTO_CAPTAIN			"%s ®­îc hÖ thèng chØ ®Þnh lµ nhãm tr­ëng"
#define		MSG_TEAM_AUTO_CAPTAIN_SELF		"B¹n ®­îc hÖ thèng chØ ®Þnh lµ nhãm tr­ëng"


#define		MSG_CHATROOM_CREATE				"B¹n t¹o phßng míi %s"
#define		MSG_CHATROOM_CREATE_ERROR_SAMENAME	"Tªn nµy ®· tån t¹i"
#define		MSG_CHATROOM_CREATE_ERROR_SAMEHOST	"B¹n ®· t¹o mét phßng råi"
#define		MSG_CHATROOM_CREATE_ERROR_NAME	"Tªn kh«ng hîp lÖ"
#define		MSG_CHATROOM_CREATE_ERROR_PASS	"MËt m· phßng kh«ng hîp lÖ"
#define		MSG_CHATROOM_CREATE_ERROR_ROOMFULL	"Sè phßng t¸n gÉu cña m¸y chñ nµy ®· ®ñ, kh«ng thÓ t¹o thªm"
#define		MSG_CHATROOM_SELF_JOIN			"B¹n vµo phßng %s"
#define		MSG_CHATROOM_JOIN				"%s ®· vµo phßng"
#define		MSG_CHATROOM_JOIN_FAIL1			"B¹n nhËp sai mËt m· phßng %s"
#define		MSG_CHATROOM_JOIN_FAIL2			"B¹n bÞ liÖt vµo sæ ®en kh«ng thÓ ®¨ng nhËp vµo phßng %s"
#define		MSG_CHATROOM_JOIN_FAIL3			"Phßng %s ®· ®Çy"
#define		MSG_CHATROOM_SELF_LEAVE			"B¹n rêi khái phßng %s"
#define		MSG_CHATROOM_LEAVE				"%s rêi khái phßng"
#define		MSG_CHATROOM_SELF_DELETE		"B¹n ®· hñy phßng %s"
#define		MSG_CHATROOM_SELF_FORCELEAVE	"B¹n bÞ ®Èy khái phßng %s"
#define		MSG_CHATROOM_SELF_BEKICK		"B¹n bÞ khai trõ khái phßng %s"
#define		MSG_CHATROOM_BEKICK				"%s bÞ khai trõ khái phßng"
#define		MSG_CHATROOM_ADDBLACKLIST		"Thªm %s vµo sæ ®en"
#define		MSG_CHATROOM_ADDBLACKLIST_FAIL	"Thªm %s vµo sæ ®en thÊt b¹i"
#define		MSG_CHATROOM_REMOVEBLACKLIST	"L­îc bá %s khái sæ ®en"
#define		MSG_CHATROOM_SELD_CHANGEPW		"B¹n ®· ®æi mËt m· phßng %s"
#define		MSG_CHATROOM_OPENGAME			"L­ît ch¬i míi ®­îc më, lÇn nµy ®¹t tèi ®a %d ®iÓm"
#define		MSG_CHATROOM_REVERSE			"L­ît ch¬i cña phßng %s ®· kÕt thóc"

#define		MSG_CHAT_APPLY_ADD_FRIEND		"B¹n ®Õn %s xin phÐp ®­îc kÕt giao."
#define		MSG_CHAT_FRIEND_HAD_IN			"%s ®· ®­îc nhËn lµm b»ng h÷u."
#define		MSG_CHAT_GET_FRIEND_APPLY		"%s xin phÐp gia nhËp b»ng h÷u"
#define		MSG_CHAT_REFUSE_FRIEND			"%s tõ chèi yªu cÇu kÕt b¹n"
#define		MSG_CHAT_ADD_FRIEND_FAIL		"Thªm %s vµo danh s¸ch b»ng h÷u thÊt b¹i"
#define		MSG_CHAT_ADD_FRIEND_SUCCESS		"Thªm %s vµo danh s¸ch b»ng h÷u thµnh c«ng"
#define		MSG_CHAT_FRIEND_ONLINE			"H¶o h÷u %s lªn m¹ng råi!"
#define		MSG_CHAT_FRIEND_OFFLINE			"H¶o h÷u %s rêi m¹ng råi!"
#define		MSG_CHAT_ENEMY_ONLINE			"Cõu nh©n %s lªn m¹ng råi!"
#define		MSG_CHAT_ENEMY_OFFLINE			"Cõu nh©n %s rêi m¹ng råi!"
#define		MSG_CHAT_DELETE_FRIEND			"B¹n cïng %s hñy bá quan hÖ"
#define		MSG_CHAT_DELETED_FRIEND			"%s cïng B¹n hñy bá quan hÖ"

#define		MSG_SHOP_NO_ROOM				"Kho¶ng trèng trong tói kh«ng ®ñ"
#define		MSG_SHOP_NO_MONEY				"B¹n kh«ng cã ®ñ ng©n l­îng"
#define		MSG_SHOP_NO_EXTPOINT			"B¹n kh«ng cã ®ñ tiÒn ®ång"
#define		MSG_SHOP_NO_FUYUAN				"B¹n kh«ng cã ®ñ phóc duyªn"
#define		MSG_SHOP_NO_REPUTE				"B¹n kh«ng cã ®ñ danh väng"
#define		MSG_SHOP_NO_ACCUM				"B¹n kh«ng cã ®ñ tÝch lòy"
#define		MSG_SHOP_NO_HONOR				"B¹n kh«ng cã ®ñ vinh dù"
#define		MSG_SHOP_NO_RESPECT				"B¹n kh«ng cã ®ñ uy danh"

#define		MSG_NPC_NO_MANA					"Néi lùc kh«ng ®ñ!"
#define		MSG_NPC_NO_STAMINA				"ThÓ lùc kh«ng ®ñ!"
#define		MSG_NPC_NO_LIFE					"Sinh lùc kh«ng ®ñ!"

#define		MSG_NPC_NOT_USE_TOWNPORTAL		"Kh«ng thÓ sö dông thæ ®Þa phï ë ®©y"
#define		MSG_NPC_NOT_PAY_FOR_SPEECH		"B¹n bÞ cÊm ph¸t ng«n ®Õn %H:%M - %d/%m/%Y"
#define		MSG_NPC_NOT_USE_SKILL_SILENT	"B¹n kh«ng thÓ thi triÓn vâ c«ng lóc nµy"
#define		MSG_NPC_NOT_USE_SKILL_DISTANCE	"B¹n ®øng c¸ch tiªu ®iÓm qu¸ xa, tr­íc m¾t sö dông vâ c«ng nµy sÏ kh«ng ®ñ hiÖu nghiÖm"
#define		MSG_NPC_NOT_USE_SKILL_WEAPON	"Vò khÝ mµ b¹n ®ang sö dông kh«ng phï hîp víi lo¹i vâ c«ng mµ b¹n hiÖn ®ang sö dông"
#define		MSG_NPC_NOT_USE_SKILL_HORSE1	"Vâ c«ng mµ b¹n sö dông hiÖn giê kh«ng thÓ c­ìi ngùa"
#define		MSG_NPC_NOT_USE_SKILL_HORSE2	"Vâ c«ng mµ b¹n sö dông hiÖn giê cÇn ph¶i c­ìi ngùa"
#define		MSG_NPC_RENASCENCE_SOMEONE		"B¹n bÞ %s h¹ träng th­¬ng"
#define		MSG_NPC_RENASCENCE				"B¹n ®· bÞ träng th­¬ng"
#define		MSG_NPC_DEATH					"%s ®· chÕt !"

#define		MSG_OBJ_CANNOT_PICKUP			"B¹n kh«ng thÓ lÊy vËt phÈm cña ng­êi kh¸c"
#define		MSG_MONEY_CANNOT_PICKUP			"B¹n kh«ng thÓ lÊy tiÒn cña ng­êi kh¸c"
#define		MSG_OBJ_TOO_FAR					"VËt phÈm qu¸ xa kh«ng thÓ lÊy"
#define		MSG_DEC_MONEY					"B¹n bÞ mÊt <color=yellow>%d <color>l­îng b¹c"
#define		MSG_EARN_MONEY					"B¹n nhËn ®­îc <color=yellow>%d <color>l­îng b¹c"
#define		MSG_DEATH_LOSE_ITEM				"B¹n ®· bÞ mÊt <color=blue>%s<color>"
#define		MSG_ADD_ITEM					"B¹n nhËn ®­îc mét c¸i <color=blue>%s<color>"
#define		MSG_ADD_ITEM_STACK				"B¹n nhËn ®­îc %d c¸i <color=blue>%s<color>"

#define		MSG_ITEM_SAME_IN_IMMEDIATE		"HiÖn ®· cã vËt phÈm ®ång lo¹i råi"
#define		MSG_ITEM_PUT_IN_IMMEDIATE		"Thµnh phÈm tèt kh«ng thÓ ®Æt vµo vËt phÈm nµy!"

#define		MSG_ITEM_DAMAGED				"Trang bÞ %s ®· bÞ háng råi"
#define		MSG_ITEM_NEARLY_DAMAGED			"Trang bÞ %s s¾p háng h·y ®Õn thî rÌn ®Ó söa ch÷a l¹i"
#define		MSG_ITEM_REPAIRED				"Tiªu tèn %d l­îng b¹c ®Ó söa ch÷a %s"

#define		MSG_LOCK_NOT_THROW				"Nh»m b¶o vÖ tµi s¶n cho b¹n VËt phÈm nµy kh«ng ®­îc tïy ý vøt bá"
#define		MSG_LOCK_NOT_TRADE				"Nh»m b¶o vÖ tµi s¶n cho b¹n VËt phÈm nµy kh«ng thÓ giao dÞch"
#define		MSG_LOCK_NOT_SETPRICE			"Nh»m b¶o vÖ tµi s¶n cho b¹n VËt phÈm nµy kh«ng thÓ ®Þnh gi¸"

#define		MSG_BROKEN_CANTREPAIR			"Trang bÞ ®· h­ háng hoµn toµn kh«ng thÓ söa ch÷a b»ng ph­¬ng ph¸p b×nh th­êng"

#define		MSG_TRADE_STATE_OPEN			"B¹n hiÖn giê cã thÓ giao dÞch"
#define		MSG_TRADE_STATE_CLOSE			"B¹n hiÖn giê kh«ng thÓ giao dÞch"
#define		MSG_TRADE_SELF_LOCK				"B¹n ®· khãa chøc n¨ng giao dÞch vËt phÈm, chê ®èi ph­¬ng tr¶ lêi"
#define		MSG_TRADE_DEST_LOCK				"Ng­êi ch¬i %s ®· khãa chøc n¨ng giao dÞch vËt phÈm"
#define		MSG_TRADE_SUCCESS				"B¹n cïng víi %s giao dÞch thµnh c«ng"
#define		MSG_TRADE_FAIL					"B¹n cïng víi %s giao dÞch thÊt b¹i"
#define		MSG_TRADE_SELF_ROOM_FULL		"Kho¶ng trèng trong tói cña B¹n kh«ng ®ñ!"
#define		MSG_TRADE_DEST_ROOM_FULL		"%s kho¶ng trèng trong tói kh«ng ®ñ!"
#define		MSG_TRADE_SEND_APPLY			"B¹n gÆp %s xin phÐp giao dÞch!"
#define		MSG_TRADE_GET_APPLY				"%s xin phÐp cïng víi B¹n giao dÞch!"
#define		MSG_TRADE_REFUSE_APPLY			"%s cù tuyÖt giao dÞch víi B¹n!"
#define		MSG_TRADE_INPUT_MONEY			"§èi ph­¬ng ®Æt vµo sè tiÒn %d l­îng"

#define		MSG_SPAR_SEND_APPLY				"B?n g?i l?i m?i t? th? v? ngh? ??n %s"
#define		MSG_SPAR_GET_APPLY				"Ng??i ch?i %s g?i l?i m?i t? th? v? ngh? ??n v?i b?n"
#define		MSG_SPAR_REFUSE_APPLY			"Ng??i ch?i %s t? ch?i l?i m?i t? v? c?a b?n"
#define		MSG_SPAR_VICTORY				"B?n ?? chi?n th?ng trong l?n t? th? v? ngh? n?y"
#define		MSG_SPAR_DEFEAT					"T? v? th?t b?i, xin ??ng ng? l?ng"

#define		MSG_TRADE_FOLKGAME_SEND_APPLY	"B¹n göi lêi mêi ®Êu ®è thuËt ®Õn %s"
#define		MSG_TRADE_FOLKGAME_GET_APPLY	"Ng­êi ch¬i %s göi lêi mêi ®Êu ®è thuËt ®Õn víi b¹n"
#define		MSG_TRADEFOLKGAME_SELF_ROOM_FULL	"Hµnh trang cña b¹n kh«ng ®ñ %dx%d « trèng"
#define		MSG_TRADEFOLKGAME_DEST_ROOM_FULL	"Hµnh trang cña %s kh«ng ®ñ %dx%d « trèng"

#define		MSG_PK_NORMAL_FLAG_OPEN			"Tr¹ng th¸i PK ë d¹ng chiÕn ®Êu"
#define		MSG_PK_NORMAL_FLAG_CLOSE		"Tr¹ng th¸i PK ë d¹ng luyÖn c«ng"
#define		MSG_PK_NORMAL_FLAG_DS			"Tr¹ng th¸i PK ë d¹ng thøc giÕt ng­êi"
#define		MSG_PK_NORMAL_FLAG_TONGWAR		"Tr¹ng th¸i PK ë d¹ng thøc m« h×nh bang chiÕn"
#define		MSG_PK_NORMAL_FLAG_UNKNOWN		"Tr¹ng th¸i PK ë d¹ng thøc ch­a râ"
#define		MSG_PK_HIGHT_LOCK_NOT_SWITCH	"HiÖn giê PK cña b¹n >= %d, tr­íc khi PK xin h·y hñy bá tr¹ng th¸i khãa b¶o hiÓm."
#define		MSG_PKSTATE_NOT_OPEN			"Tr¹ng th¸i PK nµy ch­a më"
#define		MSG_PK_VALUE					"TrÞ sè PK hiÖn t¹i lµ %d"
#define		MSG_PK_ERROR_1					"B¹n hiÖn giê ®ang ë trong tr¹ng th¸i phi chiÕn ®Êu!"
#define		MSG_PK_ERROR_2					"Ng­êi míi kh«ng thÓ giÕt thó!"
#define		MSG_PK_ERROR_3					"B¹n ®ang cïng víi ng­êi kh¸c trao ®æi ý kiÕn!"
#define		MSG_PK_ERROR_4					"B¹n ®ang cïng víi ng­êi kh¸c giÕt thó!"
#define		MSG_PK_ERROR_5					"B¹n kh«ng thÓ giÕt ng­êi míi!"
#define		MSG_PK_ERROR_6					"§èi ph­¬ng ®ang ë trong tr¹ng th¸i phi chiÕn ®Êu!"
#define		MSG_PK_ERROR_7					"§èi ph­¬ng ®ang cïng víi ng­êi kh¸c giÕt thó!"
#define		MSG_PK_ERROR_8					"B¹n hiÖn giê kh«ng thÓ cõu s¸t!"
#define		MSG_PK_ERROR_9					"Ng­êi nµy ®ang trong tr¹ng th¸i ®ang cõu s¸t"
#define		MSG_PK_ENMITY_SUCCESS_1			"%s cïng víi B¹n tiÕn hµnh cõu s¸t"
#define		MSG_PK_ENMITY_SUCCESS_2			"Sau 10 gi©y b¾t ®Çu tiÕn hµnh cõu s¸t"
#define		MSG_PK_SPAR_ENMITY_SUCCESS_1	"%s cïng víi B¹n tiÕn hµnh tû vâ"
#define		MSG_PK_SPAR_ENMITY_SUCCESS_2	"Sau 5 gi©y b¾t ®Çu tiÕn hµnh tû vâ"
#define		MSG_PK_ENMITY_CLOSE				"Tr¹ng th¸i cõu s¸t kÕt thóc"
#define		MSG_PK_SPAR_ENMITY_CLOSE		"Tr¹ng th¸i tû vâ kÕt thóc"
#define		MSG_PK_ENMITY_OPEN				"B¾t ®Çu tiÕn hµnh cõu s¸t"
#define		MSG_PK_SPAR_ENMITY_OPEN			"B¾t ®Çu tiÕn hµnh tû vâ"
#define		MGS_PK_IS_LOCK					"PK tõ phi chiÕn ®Êu chuyÓn sang luyÖn c«ng ph¶i mÊt %d phót"
#define		MGS_PK_IS_LOCK1					"Tr¹ng th¸i ®Æc biÖt kh«ng thÓ chuyÓn kiÓu chiÕn ®Êu"
#define		MGS_PK_IS_LOCK2					"Thµnh viªn bang héi míi cã thÓ më chiÕn ®Êu bang héi"
#define		MGS_PK_IS_LOCK3					"ChÕ ®é PK nhãm kh«ng thÓ tù ý chuyÓn tr¹ng th¸i chiÕn ®Êu"
#define		MGS_PK_IS_LOCK4					"Trong tæ ®éi kh«ng thÓ chuyÓn PK m« h×nh bang chiÕn"
#define		MGS_PK_IS_LOCK5					"§iÓm PK cña b¹n qu¸ nhiÒu, kh«ng thÓ h¹ thñ ®­îc!"
#define		MGS_PK_IS_LOCK6					"§iÓm kinh nghiÖm ©m cña b¹n qu¸ nhiÒu, kh«ng thÓ h¹ thñ ®­îc!"
#define		MGS_PK_IS_LOCK7					"PK chuyÓn gi÷a c¸c tr¹ng th¸i chiÕn ®Êu ph¶i mÊt %d gi©y"

#define		MSG_TONG_CREATE_ERROR01			"B?n nh?p v?o t?n bang h?i kh?ng h?p l?"
#define		MSG_TONG_CREATE_ERROR02			"Phe ph?i trong bang h?i kh?ng h?p l?"
#define		MSG_TONG_CREATE_ERROR03			"Th?nh vi?n bang h?i kh?ng th? kh?ng th? l?p m?t bang h?i kh?c!"
#define		MSG_TONG_CREATE_ERROR04			"B?n kh?ng ph?i l? s?t th? kh?ng th? l?p bang!"
#define		MSG_TONG_CREATE_ERROR05			"B?n kh?ng ?? %d c?p kh?ng th? l?p bang!"
#define		MSG_TONG_CREATE_ERROR06			"B?n kh?ng ?? %d ??ng c?p th?ng so?i!"
#define		MSG_TONG_CREATE_ERROR07			"B?n kh?ng ?? %d ng?n l??ng b?c ?? l?p bang!"
#define		MSG_TONG_CREATE_ERROR08			"B?n ?ang trong ??i ng?, kh?ng th? l?p bang!"
#define		MSG_TONG_CREATE_ERROR09			"B?n kh?ng c? Nh?c v??ng ki?m!"
#define		MSG_TONG_CREATE_ERROR10			"B?n ?ang b?n ho?c ?ang giao d?ch kh?ng th? l?p bang"
#define		MSG_TONG_CREATE_ERROR11			"B?n kh?ng ?? %d danh v?ng"
#define		MSG_TONG_CREATE_ERROR12			"B?n kh?ng ?? %d ph?c duy?n"
#define		MSG_TONG_CREATE_ERROR13			"S? bang h?i c?a server ?? ??t gi?i h?n kh?ng th? t?o th?m"
#define		MSG_TONG_CREATE_ERROR14			"T?n bang h?i ho?c t?n bang ch? b? l?i kh?ng th? t?o"
#define		MSG_TONG_CREATE_ERROR15			"Bang h?i ?? t?n t?i ho?c b?n ?ang l? bang ch? c?a m?t bang h?i kh?c"
#define		MSG_TONG_APPLY_CREATE			"B?n g?i y?u c?u th?nh l?p bang h?i"
#define		MSG_TONG_CREATE_SUCCESS			"Th?nh l?p bang h?i th?nh c?ng"
#define		MSG_TONG_SEND_APPLY				"B?n g?i l?i m?i gia nh?p bang h?i ??n %s"
#define		MSG_TONG_REV_APPLY				"%s g?i l?i m?i gia nh?p bang h?i ??n b?n"
#define		MSG_TONG_APPLY_ADD				"B?n g?i y?u c?u xin gia nh?p bang h?i!"
#define		MSG_TONG_APPLY_ADD_SELF_MSG		"Ng??i ch?i %s xin ph?p gia nh?p bang h?i"
#define		MSG_TONG_APPLY_ADD_ERROR		"Ng??i ch?i %s kh?ng ?? ?i?u ki?n gia nh?p bang h?i"
#define		MSG_TONG_APPLY_ADD_ERROR1		"Th?nh vi?n bang h?i kh?ng th? gia nh?p bang h?i kh?c"
#define		MSG_TONG_APPLY_ADD_ERROR2		"B?n ph?i l? s?t th? m?i c? th? gia nh?p bang h?i"
#define		MSG_TONG_APPLY_ADD_ERROR3		"B?n ph?i r?i kh?i t? ??i m?i c? th? gia nh?p bang h?i"
#define		MSG_TONG_APPLY_ADD_ERROR4		"B?n v?a ch? ??ng r?i kh?i bang h?i, ?ang trong th?i gian th? th?ch kh?ng th? g?i y?u c?u"
#define		MSG_TONG_APPLY_ADD_ERROR5		"Bang h?i m? b?n v?a xin gia nh?p ?? ??ng ch?c n?ng chi?u m? ?? t?"
#define		MSG_TONG_APPLY_ADD_ERROR6		"Bang h?i ?? ??ng ch?c n?ng chi?u m? ?? t?"
#define		MSG_TONG_APPLY_ADD_ERROR7		"Ng??i n?y v?a r?i kh?i bang h?i ch?a th? gia nh?p b?y gi?"
#define		MSG_TONG_REFUSE_ADD				"Ng??i ch?i %s t? ch?i l?i m?i gia nh?p bang h?i"
#define		MSG_TONG_ADD_SUCCESS			"B?n ?? gia nh?p bang h?i th?nh c?ng"
#define		MSG_TONG_CANNOT_LEAVE1			"Th?nh vi?n ch?nh th?c kh?ng th? r?i kh?i bang"
#define		MSG_TONG_CANNOT_LEAVE2			"Tr??ng l?o kh?ng th? r?i kh?i bang"
#define		MSG_TONG_BE_KICKED				"B?n b? tr?c xu?t ra kh?i bang"
#define		MSG_TONG_KICK_ERROR1			"Thao t?c phi ph?p, b?n kh?ng thu?c b?t k? bang h?i n?o"
#define		MSG_TONG_KICK_ERROR2			"Thao t?c th?t b?i, ch?c Tr??ng l?o tr? l?n m?i c? th? tr?c xu?t th?nh vi?n"
#define		MSG_TONG_KICK_ERROR3			"Tr??ng l?o kh?ng th? tr?c xu?t tr?c ti?p"
#define		MSG_TONG_LEAVE_SUCCESS			"B?n ?? r?i kh?i bang h?i th?nh c?ng, c? th? xin gia nh?p bang h?i kh?c sau %d ph?t"
#define		MSG_TONG_LEAVE_FAIL				"R?i kh?i bang h?i th?t b?i"
#define		MSG_TONG_NO_MONEY				"B?n kh?ng c? ?? ng?n l??ng kh?ng th? t? ? r?i kh?i"
#define		MSG_TONG_CHANGE_AS_MASTER		"B?n b?y gi? ??m nh?n ch?c v? Bang ch?"
#define		MSG_TONG_CHANGE_MASTER_ERROR1	"Chuy?n v? th?t b?i, ??i ph??ng kh?ng tr?c tuy?n!"
#define		MSG_TONG_CHANGE_MASTER_ERROR2	"Chuy?n v? th?t b?i, ??i ph??ng n?ng l?c kh?ng ??!"
#define		MSG_TONG_CHANGE_MASTER_ERROR3	"Chuy?n v? th?t b?i, trong bang kh?ng c? ng??i n?y!"
#define		MSG_TONG_CHANGE_AS_MEMBER		"B?y gi? b?n tr? th?nh th?nh vi?n b?nh th??ng"
#define		MSG_TONG_CHANGE_AGNAME_ERROR1	"Ng??i ch?i n?y hi?n kh?ng tr?c tuy?n, thao t?c th?t b?i"
#define		MSG_TONG_CHANGE_AGNAME_ERROR2	"Ng??i ch?i n?y kh?ng ?? t?i l?nh ??o, thao t?c th?t b?i"
#define		MSG_TONG_CHANGE_AGNAME_ERROR3	"Ng??i ch?i n?y kh?ng t?n t?i, thao t?c th?t b?i"
#define		MSG_TONG_CHANGE_CAMP			"Ti?n h?nh thay ??i phe ph?i b?n bang"
#define		MSG_TONG_CHANGE_CAMP_ERROR1		"ChØ cã bang chñ míi sö dông ®­îc chøc n¨ng nµy"
#define		MSG_TONG_CHANGE_CAMP_ERROR2		"Vui lßng chän phe ph¸i thay ®æi ®Õn"
#define		MSG_TONG_CHANGE_CAMP_ERROR3		"Ng©n khè kh«ng cã ®ñ %d l­îng, kh«ng thÓ thay ®æ phe ph¸i bæn bang"
#define		MSG_TONG_CHANGE_SAVEJIYU_ERROR	"Ng©n khè kh«ng cã ®ñ %d l­îng, kh«ng thÓ b¶o l­u"

#define		MSG_CANT_SIT_RIDE				"Lóc c­ìi ngùa kh«ng thÓ thiÒn täa"
#define		MSG_CANT_SWITCH_HORSE_SIT		"B¹n ®ang thiÒn täa kh«ng thÓ lªn ngùa"
#define		MSG_CANT_SWITCH_HORSE			"B¹n qu¸ mÖt mái kh«ng thÓ liªn tôc lªn hoÆc xuèng ngùa"
#define		MSG_CANT_TRADE_BUZY				"Trong lóc c­ìi ngùa hoÆc ®ang giao dÞch víi tæ ®éi kh«ng thÓ bµy b¸n!"

#define		MESSAGE_BROADCAST_ANNOUCE_HEAD	"Th«ng B¸o"
#define		MESSAGE_SYSTEM_ANNOUCE_HEAD		"HÖ Thèng"
#define		MESSAGE_SYSTEM_ANNOUCE_SKILL	"HÖ thèng kü n¨ng"
#define		MESSAGE_SYSTEM_ANNOUCE_PROTECT	"HÖ thèng b¶o mËt"
#define		MESSAGE_TEAM_ANNOUCE_HEAD		"Tin ®éi"
#define		MESSAGE_TONG_ANNOUCE_HEAD		"Tin bang"
#define		MESSAGE_ROOM_ANNOUCE_HEAD		"Tin phßng"

#define		defTONG_VICEROY_AGNAME			"[Th¸i thó]"
#define		defTONG_MASTER_AGNAME			"Bang chñ"
#define		defTONG_DIRECTOR_AGNAME			"Tr­ëng l·o"
#define		defTONG_MANAGER_AGNAME			"§éi tr­ëng"
#define		defTONG_MEMBER_AGNAME			"§Ö tö"

#define		MSG_TRADE_COUNT					"B¹n ch­a ®Þnh gi¸ cho bÊt kú vËt phÈm nµo"
#define		MSG_TRADE_SELLED				"VËt phÈm nµy ®· ®­îc b¸n ra råi!"
#define		MSG_TRADE_CLOSED				"Ng­êi bµy b¸n ®· cÊt ®i råi"

#define		MSG_FIGHT_MODE_ERROR1			"Tr¹ng th¸i kh«ng ®­îc b¶o vÖ kh«ng thÓ thùc hiÖn"
#define		MSG_FIGHT_MODE_ERROR2			"B¹n trong tr¹ng th¸i kh«ng ®­îc b¶o vÖ kh«ng thÓ thùc hiÖn"
#define		MSG_FIGHT_MODE_ERROR3			"%s trong tr¹ng th¸i kh«ng ®­îc b¶o vÖ kh«ng thÓ thùc hiÖn"

#define		MSG_SUBWORLD_NOT_OPEN			"§­êng ®i phÝa tr­íc kh«ng th«ng"

#define		MSG_PASSWORD_ERROR1				"MËt khÈu ph¶i cã ®é dµi tõ 4 ®Õn 16 ký tù"
#define		MSG_PASSWORD_ERROR2				"MËt khÈu x¸c nhËn kh«ng trïng khíp. Xin mêi nhËp l¹i"
#define		MSG_PASSWORD_ERROR3				"MËt khÈu míi nhËp vµo kh«ng thÓ trïng víi mËt khÈu hiÖn t¹i"

#define		MSG_FUNCTION_FOBIDDENED			"Xin lçi, c«ng n¨ng nµy t¹m thêi ®· bÞ ®ãng!"
#define		MSG_FUNCTION_LOCKED				"Thao t¸c ®· bÞ khãa! vui lßng më khãa b¶o vÖ tr­íc khi thùc hiÖn."


enum enumMSG_ID
{
	enumMSG_ID_NONE = 0,
	enumMSG_ID_TEAM_KICK_ONE,
	enumMSG_ID_TEAM_DISMISS,
	enumMSG_ID_TEAM_LEAVE,
	enumMSG_ID_TEAM_REFUSE_INVITE,
	enumMSG_ID_TEAM_APPLY_INVITE_FAIL,
	enumMSG_ID_TEAM_APPLY_INVITE_FAIL2,
	enumMSG_ID_TEAM_SELF_ADD,
	enumMSG_ID_TEAM_ADDMEMBER_NOTVALID,
	enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL,
	enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL2,
	enumMSG_ID_OBJ_CANNOT_PICKUP,
	enumMSG_ID_OBJ_TOO_FAR,
	enumMSG_ID_DEC_MONEY,
	enumMSG_ID_SPAR_VICTORY,
	enumMSG_ID_SPAR_DEFEAT,
	enumMSG_ID_TRADE_SELF_ROOM_FULL,
	enumMSG_ID_TRADE_DEST_ROOM_FULL,
	enumMSG_ID_TRADE_REFUSE_APPLY,
	enumMSG_ID_TRADEFOLKGAME_SELF_ROOM_FULL,
	enumMSG_ID_TRADEFOLKGAME_DEST_ROOM_FULL,
	enumMSG_ID_SPAR_REFUSE_APPLY,
	enumMSG_ID_GET_ITEM,
	enumMSG_ID_MONEY_CANNOT_PICKUP,
	enumMSG_ID_CANNOT_ADD_TEAM,
	enumMSG_ID_TEAM_IS_FULL,
	enumMSG_ID_CANNOT_INVITE_TEAM,
	enumMSG_ID_TARGET_CANNOT_ADD_TEAM,
	enumMSG_ID_GET_INVITE_TEAM_REPLY,
	enumMSG_ID_PK_NORMAL_NOT_OPEN,
	enumMSG_ID_PK_HIGHT_LOCK_NOT_SWITCH,
	enumMSG_ID_PKSTATE_NOT_OPEN,
	enumMSG_ID_PK_ERROR_1,
	enumMSG_ID_PK_ERROR_2,
	enumMSG_ID_PK_ERROR_3,
	enumMSG_ID_PK_ERROR_4,
	enumMSG_ID_PK_ERROR_5,
	enumMSG_ID_PK_ERROR_6,
	enumMSG_ID_PK_ERROR_7,
	enumMSG_ID_PK_ERROR_8,
	enumMSG_ID_PK_ERROR_9,
	enumMSG_ID_PK_IS_LOCK,
	enumMSG_ID_PK_IS_LOCK1,
	enumMSG_ID_PK_IS_LOCK2,
	enumMSG_ID_PK_IS_LOCK3,
	enumMSG_ID_PK_IS_LOCK4,
	enumMSG_ID_PK_IS_LOCK5,
	enumMSG_ID_PK_IS_LOCK6,
	enumMSG_ID_PK_IS_LOCK7,
	enumMSG_ID_TRADE_BUZY,
	enumMSG_ID_TRADE_SELLED,
	enumMSG_ID_TRADE_CLOSED,
	enumMSG_ID_DEATH_LOSE_ITEM,
	enumMSG_ID_TONG_REFUSE_ADD,
	enumMSG_ID_TONG_BE_KICK,
	enumMSG_ID_TONG_LEAVE_SUCCESS,
	enumMSG_ID_TONG_LEAVE_FAIL,
	enumMSG_ID_TONG_NO_MONEY,
	enumMSG_ID_TONG_CHANGE_AS_MASTER,
	enumMSG_ID_TONG_CHANGE_AS_MEMBER,
	enumMSG_ID_TONG_APPLY_ADD_ERROR,
	enumMSG_ID_TONG_APPLY_ADD_ERROR1,
	enumMSG_ID_TONG_APPLY_ADD_ERROR2,
	enumMSG_ID_TONG_APPLY_ADD_ERROR3,
	enumMSG_ID_TONG_APPLY_ADD_ERROR4,
	enumMSG_ID_TONG_APPLY_ADD_ERROR5,
	enumMSG_ID_TONG_APPLY_ADD_ERROR6,
	enumMSG_ID_TONG_APPLY_ADD_ERROR7,
	enumMSG_ID_FIGHT_MODE_ERROR1,
	enumMSG_ID_FIGHT_MODE_ERROR2,
	enumMSG_ID_FIGHT_MODE_ERROR3,
	enumMSG_ID_SHOP_NO_ROOM,
	enumMSG_ID_SHOP_NO_MONEY,
	enumMSG_ID_SHOP_NO_EXTPOINT,
	enumMSG_ID_SHOP_NO_FUYUAN,
	enumMSG_ID_SHOP_NO_REPUTE,
	enumMSG_ID_SHOP_NO_ACCUM,
	enumMSG_ID_SHOP_NO_HONOR,
	enumMSG_ID_SHOP_NO_RESPECT,
	enumMSG_ID_FUNCTION_CHAT_FORBIDDENED,
	enumMSG_ID_FUNCTION_FOBIDDENED,
	enumMSG_ID_FUNCTION_LOCKED,
	enumMSG_ID_LOCK_NOT_TRADE,
	enumMSG_ID_LOCK_NOT_SETPRICE,
	enumMSG_ID_LOCK_NOT_THROW,
	enumMSG_ID_ITEM_TOWNPORTAL,
	enumMSG_ID_ITEM_CANTREPAIR,
	enumMSG_ID_ITEM_SAME_IMMEDIATE,
	enumMSG_ID_ITEM_CANTPUT_IMMEDIATE,
	enumMSG_ID_HORSE_CANT_SWITCH1,
	enumMSG_ID_HORSE_CANT_SWITCH2,
	enumMSG_ID_CHATROOM_JOIN,
	enumMSG_ID_CHATROOM_LEAVE,
	enumMSG_ID_CHATROOM_BEKICK,
	enumMSG_ID_CHATROOM_OPENGAME,
	enumMSG_ID_CHATROOM_REVERSE,
	enumMSG_ID_NPC_RENASCENCE_SOMEONE,
	enumMSG_ID_NPC_RENASCENCE,
	enumMSG_ID_NUM,
};

//---------------------------- ???????????? ------------------------------
#define		MOUSE_CURSOR_NORMAL				0
#define		MOUSE_CURSOR_FIGHT				1
#define		MOUSE_CURSOR_DIALOG				2
#define		MOUSE_CURSOR_PICK				3
#define		MOUSE_CURSOR_USE				8
#define		MOUSE_CURSOR_TRADE				11
#define		MOUSE_CURSOR_FINGER				12
#define		MOUSE_CURSOR_HELP				13
#endif

