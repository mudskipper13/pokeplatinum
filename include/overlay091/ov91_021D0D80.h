#ifndef POKEPLATINUM_OV91_021D0D80_H
#define POKEPLATINUM_OV91_021D0D80_H

#include "struct_defs/struct_020997B8.h"

#include "applications/pokemon_summary_screen/main.h"

#include "bg_window.h"
#include "message.h"
#include "list_menu.h"
#include "menu.h"
#include "overlay_manager.h"
#include "sprite_system.h"

enum MoveReminderWindows {
    MOVE_REMINDER_WINDOW_0 = 0,
    MOVE_REMINDER_WINDOW_1,
    MOVE_REMINDER_WINDOW_2,
    MOVE_REMINDER_WINDOW_3,
    MOVE_REMINDER_WINDOW_4,
    MOVE_REMINDER_WINDOW_5,
    MOVE_REMINDER_WINDOW_6,
    MOVE_REMINDER_WINDOW_7,
    MOVE_REMINDER_WINDOW_8,
    MOVE_REMINDER_WINDOW_9,
    MOVE_REMINDER_WINDOW_10,
    MOVE_REMINDER_WINDOW_11,
    MOVE_REMINDER_WINDOW_12,
    MOVE_REMINDER_WINDOW_13,
    MOVE_REMINDER_WINDOW_14,

    MOVE_REMINDER_WINDOW_MAX,
};

enum MoveReminderManagedSprites {
    MOVE_REMINDER_SPRITE_0 = 0,
    MOVE_REMINDER_SPRITE_1,
    MOVE_REMINDER_SPRITE_2,
    MOVE_REMINDER_SPRITE_3,
    MOVE_REMINDER_SPRITE_CURSOR,
    MOVE_REMINDER_SPRITE_5,
    MOVE_REMINDER_SPRITE_6,
    MOVE_REMINDER_SPRITE_7,
    MOVE_REMINDER_SPRITE_8,
    MOVE_REMINDER_SPRITE_9,
    MOVE_REMINDER_SPRITE_10,
    MOVE_REMINDER_SPRITE_11,
    MOVE_REMINDER_SPRITE_12,

    MOVE_REMINDER_SPRITE_MAX,
};

typedef struct {
    MoveReminderData *data;
    BgConfig *bgConfig;
    Window window[MOVE_REMINDER_WINDOW_MAX];
    MessageLoader *msgLoader;
    StringTemplate *strTemplate;
    Strbuf *strbuf;
    ListMenu *list;
    StringList *strList;
    Menu *menu;
    SpriteSystem *spriteSys;
    SpriteManager *spriteMan;
    ManagedSprite *managedSprite[MOVE_REMINDER_SPRITE_MAX];
    PokemonSummary summaryScreen;
    OverlayManager *overlayMan;
    int state;
    u8 movesCount;
    u8 txtPrinterID;
    u8 unk_186; // index of sort
} MoveReminder;

typedef int (*GenericFunctionPtr_1)(MoveReminder *);

typedef struct {
    GenericFunctionPtr_1 unk_00;
    GenericFunctionPtr_1 unk_04;
} GameWindowLayout_1;

int MoveReminder_Init(OverlayManager *overlayMan, int *state);
int MoveReminder_Main(OverlayManager *overlayMan, int *state);
int MoveReminder_Exit(OverlayManager *overlayMan, int *state);

#endif // POKEPLATINUM_OV91_021D0D80_H
