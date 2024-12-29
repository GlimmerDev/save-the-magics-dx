#ifndef MAGICS_ENUM_H
#define MAGICS_ENUM_H

// Establish platform defines
#if defined(__APPLE__) && defined(TARGET_OS_IPHONE)
#define __IPHONE__
#endif

#if defined(__ANDROID__) || defined(__IPHONE__)
#define __MAGICSMOBILE__
#endif

// ********************
// * PLATFORM DEPENDENT
// ********************

// MOBILE
#ifdef __MAGICSMOBILE__
#define CLICK_STR "Tap!"
#define NUM_OPTIONS 1
// DESKTOP
#else
#define CLICK_STR "Click!"
#define NUM_OPTIONS 4
#endif

// ************************
// * NON-PLATFORM DEPENDENT
// ************************

// GENERAL
#define FC_USE_SDL_GPU
#define SDL_RENDER_SCALE_QUALITY 0
#define MAX_PATH_LEN 1024

// APP INFO
#define MAGICS_ORG_STR "GlimmerDev"
#define MAGICS_APP_STR "SaveTheMagicsDX"
#define VERSION_NUMBER "2.0-b1"
#define SAVE_VERSION   200

// SCREEN DIMENSIONS
#define SCREEN_WIDTH_W  1067
#define SCREEN_WIDTH_C  800
#define SCREEN_HEIGHT_W 600
#define SCREEN_HEIGHT_C SCREEN_HEIGHT_W

// TIMING CONSTANTS
#define DEFAULT_MEDI_TIMER    60
#define DEFAULT_MEDI_COOLDOWN 60*5
#define DEFAULT_FPS           60
#define DEFAULT_AUTOSAVE      2
#define DEFAULT_ASPECT        ASPECT_WIDE
#define DEFAULT_POSTSAVE      0
#define MAX_LAG_FRAMES        4

// COLORS
#define NUM_COLORS 52
typedef enum {
    BLACK,
    WHITE,
    RED,
    GREEN,
    OFFWHITE,
    LOCKED,
    BACKGROUND,
    PLANET1,
    PLANET2,
    PLANET3,
    EXPL1,
    EXPL2,
    EXPL3,
    EXPL4,
    SHIP_COL,
    SHIP_GLOW,
    B_PURPLE,
    B_PURPLE_S,
    B_LIGHTPURPLE,
    B_LIGHTPURPLE_S,
    B_DARKPURPLE,
    B_DARKPURPLE_S,
    B_BLUE,
    B_BLUE_S,
    B_LIGHTBLUE,
    B_LIGHTBLUE_S,
    B_GREEN,
    B_GREEN_S,
    B_RED,
    B_RED_S,
    B_ORANGE,
    B_ORANGE_S,
    B_DARK,
    B_DARK_S,
    B_DARKGREEN,
    B_DARKGREEN_S,
	B_PINK,
	B_PINK_S,
    B_MENU,
    B_MENU_S,
	B_SAVE_CLEAR,
	B_SAVE_CLEAR_S,
	B_SAVE_CL,
	B_SAVE_CL_S,
	B_SAVE_CLEAR_CL,
	B_SAVE_CLEAR_CL_S,
    STAR1,
    STAR2,
    STAR3,
    STAR4,
	TRANS_WHITE,
	TRANSPARENT
} E_ColorIndex;
// "virtual" colors
#define B_YELLOW     B_MENU
#define B_YELLOW_S   B_MENU_S
#define B_DARKRED    B_DARK
#define B_DARKRED_S  B_DARK_S

// SOUNDS
#define NUM_SOUNDS 10
typedef enum {
	UPGRADE_SND,
	MENU_BUTTON_SND,
	MEDITATE_SND,
	MEDI_CLICK_SND,
	INCANT_SND,
	INCANT_READY_SND,
	END_EXPLODE_SND,
	END_SHOOT_SND,
	ENGAGE_EVIL_SND,
	EVIL_SHIP_SND
} E_SoundIndex;

// BUTTONS
#define NUM_MISC_BUTTONS   28
#define NUM_MENU_BUTTONS   5
#define MENU_BUTTON_OFFSET NUM_MISC_BUTTONS
#define OPTIONS_OFFSET     OPT_AUTOSAVE_B
#define NUM_BUTTONS        NUM_MISC_BUTTONS+NUM_MENU_BUTTONS

typedef enum {
	MAGIC_B,
	NEXT_B,
	FACE_EVIL_B,
	START_MEDI_B,
	MEDI_B,
	NEW_GAME_B,
	LOAD_GAME_B,
	OPTIONS_B,
	COMPENDIUM_B,
	COMP_LEFT_B,
	COMP_RIGHT_B,
	COMP_EXIT_B,
	SAVE_YES_B,
	SAVE_NO_B,
	SAVE_CANCEL_B,
	QUIT_B,
	MUTE_B,
	END_OK_B,
	SAVE_0_B,
	SAVE_1_B,
	SAVE_2_B,
	SAVE_3_B,
	OPT_AUTOSAVE_B,
	OPT_ASPECT_B,
	OPT_FPS_B,
	OPT_POSTSAVE_B,
	OPT_IMPORT_B,
	OPT_CONFIRM_B,
	MENU_UPGRADES_B,
	MENU_PRINCESS_B,
	MENU_MEDI_B,
	MENU_INCANT_B,
	MENU_EVIL_B
} E_ButtonIndex;

// BUTTON STATE
typedef enum {
	STATE_BTN_IDLE,
	STATE_BTN_CLICKED
} E_ButtonState;

// FONTS
#define NUM_FONTS 1
typedef enum {
	FONT_RPG
} E_FontIndex;
#define MIN_FONT_SIZE 14
#define MAX_FONT_SIZE 82
#define NUM_PER_FONT (MAX_FONT_SIZE-MIN_FONT_SIZE)/2

// UPGRADES
#define NUM_UPGRADES       55
#define NUM_PRINCESSES     11
#define NUM_INCANTATIONS   10
#define NUM_UPGRADE_FIELDS 9

#define PRINCESS_OFFSET      NUM_UPGRADES
#define INCANTATION_OFFSET   PRINCESS_OFFSET+NUM_PRINCESSES
#define UPGRADES_END_OFFSET  INCANTATION_OFFSET+NUM_INCANTATIONS

// COMPENDIUM
# define NUM_COMPENDIUM_ENTRIES 12
# define NUM_COMPENDIUM_FIELDS 5

// SHAPES
#define NUM_BG_SHAPES 15
#define NUM_STARS 200

// ENUMS

// UPGRADE TYPES
typedef enum {
	UPGRADE_TYPE_UPGRADE,
	UPGRADE_TYPE_PRINCESS,
	UPGRADE_TYPE_INCANTATION
} E_UpgradeType;

// SHAPE TYPES
typedef enum {
	SHAPE_TYPE_RECT,
	SHAPE_TYPE_CIRCLE,
	SHAPE_TYPE_TRI,
	SHAPE_TYPE_TREE,
	SHAPE_TYPE_HILL
} E_ShapeType;

// BACKGROUND SHAPES
typedef enum {
	BG_PLANET1,
	BG_PLANET2,
	BG_PLANET3,
	BG_MEDI_SUN1,
	BG_MEDI_SUN2,
	BG_MEDI_SUN3,
	BG_MEDI_TREE1,
	BG_MEDI_TREE2,
	BG_MEDI_TREE3,
	BG_MEDI_HILL1,
	BG_MEDI_HILL2,
	BG_MEDI_HILL3,
	BG_MENU_RECT,
	BG_COMP_BIO,
	BG_COMP_ART
} E_BGShapes;

// UPGRADE EFFECTS
typedef enum {
	EFFECT_INCREASE_MPS,
	EFFECT_MULT_ADD,
	EFFECT_MULT_MULT,
	EFFECT_CLICK_ADD,
	EFFECT_CLICK_MULT,
	EFFECT_TOTAL_MULT,
	EFFECT_REDUCE_MEDI_COOLDOWN,
	EFFECT_MULT_MEDI_TIMER,
	EFFECT_SET_MEDI_MULT,
	EFFECT_INCREASE_MAX_UPGR,
	EFFECT_INCREASE_MAX_PRIN,
	EFFECT_DIV_INCANT_TIMERS
} E_UpgradeEffect;

// SCREENS (GAMESTATES)
typedef enum {
	SCREEN_TITLE,
	SCREEN_OPTIONS,
	SCREEN_SAVE,
	SCREEN_GAME_LOOP,
	SCREEN_ENDING,
	SCREEN_COMPENDIUM
} E_StateScreen;

// MENUS (SUB-STATES)
typedef enum {
	MENU_UPGRADES,
	MENU_PRINCESS,
	MENU_MEDITATE,
	MENU_INCANTATIONS,
	MENU_FACE_EVIL,
	MENU_LD_SLOT,
	MENU_SV_SLOT,
	MENU_SV_CNF_OVERWR,
	MENU_SV_CNF_QUIT,
	MENU_SV_ENDING,
	MENU_END_PROMPT1,
	MENU_END_PROMPT2,
	MENU_END_FAIL,
	MENU_END_WIN1,
	MENU_END_WIN2
} E_StateMenu;

// ERROR CODES
typedef enum {
	ERR_NONE,
	ERR_MALLOC,
	ERR_JSON_LOAD,
	ERR_JSON_SAVE,
	ERR_SAVE_VERSION,
	ERR_MAGIC_MISSILE
} E_ErrorCode;

// ASPECT RATIOS
typedef enum {
	ASPECT_WIDE,
	ASPECT_CLASSIC
} E_AspectType;

// RELOAD STATES
typedef enum {
	RELOAD_STATE_NONE,
	RELOAD_STATE_WRITECFG,
	RELOAD_STATE_REQUESTED,
	RELOAD_STATE_EXECUTE,
	RELOAD_STATE_EXECUTE_SOFT
} E_ReloadState;

#endif
