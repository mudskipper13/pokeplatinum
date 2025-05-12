#ifndef POKEPLATINUM_STRUCT_020997B8_H
#define POKEPLATINUM_STRUCT_020997B8_H

#include "game_options.h"
#include "pokemon.h"
#include "trainer_info.h"

typedef struct {
    Pokemon *mon;
    TrainerInfo *trainerInfo;
    Options *options;
    u16 *movesList;
    u16 cursorPos;
    u16 listPos;
    u8 unk_14;
    u8 unk_15;
    u8 unk_16;
    u8 moveSlot;
} MoveReminderData;

#endif // POKEPLATINUM_STRUCT_020997B8_H
