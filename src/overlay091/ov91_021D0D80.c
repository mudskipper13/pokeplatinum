#include "overlay091/ov91_021D0D80.h"

#include <nitro.h>
#include <string.h>

#include "constants/field/window.h"
#include "constants/heap.h"

#include "struct_defs/struct_02099F80.h"

#include "applications/pokemon_summary_screen/main.h"
#include "overlay007/shop_menu.h"

#include "bg_window.h"
#include "font.h"
#include "game_options.h"
#include "graphics.h"
#include "gx_layers.h"
#include "heap.h"
#include "list_menu.h"
#include "menu.h"
#include "message.h"
#include "move_table.h"
#include "narc.h"
#include "overlay_manager.h"
#include "pokemon.h"
#include "render_text.h"
#include "render_window.h"
#include "sound_playback.h"
#include "sprite.h"
#include "sprite_system.h"
#include "strbuf.h"
#include "string_list.h"
#include "string_template.h"
#include "system.h"
#include "text.h"
#include "unk_0200F174.h"
#include "unk_0207C908.h"
#include "unk_0208C098.h"
#include "unk_02094EDC.h"
#include "vram_transfer.h"

#include "constants/moves.h"
#include "constants/narc.h"
#include "generated/sdat.h"
#include "generated/text_banks.h"
#include "res/text/bank/unk_0645.h"
#include "constdata/const_020F410C.h"

enum XAlignmentModes {
    ALIGN_MODE_LEFT = 0,
    ALIGN_MODE_RIGHT,
    ALIGN_MODE_CENTER
};

enum FormatStrings {
    FMTSTR_TEACH_WHICH_MOVE = 0,
    FMTSTR_TEACH_THE_MOVE,
    FMTSTR_GIVE_UP_TEACHING_MOVE,
    FMTSTR_MON_LEARNED_MOVE,
    FMTSTR_MON_TRYING_TO_LEARN_MOVE,
    FMTSTR_MON_FORGOT_MOVE,
    FMTSTR_MON_LEARNED_MOVE2, // succession after FMTSTR_MON_FORGOT_MOVE
    FMTSTR_MON_STOP_TEACHING_MOVE,
    FMTSTR_MON_DID_NOT_LEARN_MOVE,
    FMTSTR_PLAYER_HANDED_OVER_ITEM,
    FMTSTR_MAKE_MON_FORGOT_MOVE
};

static void MoveReminder_LoadGraphics(MoveReminder *reminder);
static void MoveReminder_Free(MoveReminder *reminder);
static void MoveReminder_VBlank(void *data);
static void MoveReminder_SetGXLayersBanks(void);
static void MoveReminder_SetBGLayers(BgConfig *bgConfig);
static void MoveReminder_FreeBgConfig(BgConfig *bgConfig);
static void MoveReminder_LoadGraphicsFromNARC(MoveReminder *reminder, NARC *narc);
static void MoveReminder_LoadWindows(MoveReminder *reminder);
static void MoveReminder_FreeWindows(MoveReminder *reminder);
static void MoveReminder_LoadStrings(MoveReminder *reminder);
static void MoveReminder_FreeStrings(MoveReminder *reminder);
static int MoveReminder_WaitScreenTransition(MoveReminder *reminder);
static int MoveReminder_Select(MoveReminder *reminder);
static int MoveReminder_ScreenTransitionOut(MoveReminder *reminder);
static int MoveReminder_HasTextPrinterFinished(MoveReminder *reminder);
static int MoveReminder_MakeYesNoChoice(MoveReminder *reminder);
static int MoveReminder_SelectYesNo(MoveReminder *reminder);
static int MoveReminder_SetMoveSlotToSelectedMove(MoveReminder *reminder);
static int MoveReminder_PrintMonLearnedMove2(MoveReminder *reminder);
static int MoveReminder_PrintForgotOrStopTeachingMove(MoveReminder *reminder);
static int MoveReminder_ScreenTransitionIn(MoveReminder *reminder);
static int MoveReminder_OpenSummaryScreenApp(MoveReminder *reminder);
static int MoveReminder_ReloadApp(MoveReminder *reminder);
static void MoveReminder_PrintStrbuf(MoveReminder *reminder, u32 windowID, u32 fontID, TextColor color, enum XAlignmentModes xAlignMode);
static void MoveReminder_LoadSelectedMoveInfo(MoveReminder *reminder);
static void MoveReminder_PrintStaticInfo(MoveReminder *reminder);
static void MoveReminder_InitListMenu(MoveReminder *reminder);
static void MoveReminder_FreeListMenu(MoveReminder *reminder);
static void MoveReminder_MenuCursorCallback(ListMenu *listMenu, u32 index, u8 onInit);
static void MoveReminder_MenuPrintCallback(ListMenu *listMenu, u32 index, u8 yOffset);
static void MoveReminder_PrintBattleInfo(MoveReminder *reminder, u32 index);
static void MoveReminder_PrintContestInfo(MoveReminder *reminder, u32 index);
static void MoveReminder_PrintContestHeartsInfo(MoveReminder *reminder, u16 index);
static void MoveReminder_FormatStrbuf(MoveReminder *reminder, enum FormatStrings fmt);
static void MoveReminder_FormatAndPrintStrbuf(MoveReminder *reminder, enum FormatStrings fmt);
static BOOL MoveReminder_TextPrinterCallback(TextPrinterTemplate *template, u16 param);
static u16 MoveReminder_SelectedMove(MoveReminder *reminder);
static u16 MoveReminder_MoveIndex(MoveReminder *reminder);
static u8 MoveReminder_CalculateTotalMoves(MoveReminder *reminder);
static int MoveReminder_PrintMonLearnedMove(MoveReminder *reminder); // TODO:
static int ov91_021D1EE8(MoveReminder *reminder);
static int ov91_021D1F38(MoveReminder *reminder);
static int ov91_021D1F44(MoveReminder *reminder);
static int ov91_021D1F94(MoveReminder *reminder);
static int ov91_021D1FA0(MoveReminder *reminder);
static int ov91_021D1FBC(MoveReminder *reminder);
static int ov91_021D1FD8(MoveReminder *reminder);
static int ov91_021D1FF4(MoveReminder *reminder);
static int ov91_021D2008(MoveReminder *reminder);
static void MoveReminder_LoadSprites(MoveReminder *reminder, NARC *narc);
static void MoveReminder_FreeSprites(MoveReminder *reminder);
static void MoveReminder_UpdateSpritesAnim(MoveReminder *reminder);
static void MoveReminder_UpdateTypeSprites(MoveReminder *reminder);
static void MoveReminder_MoveTypeSprites(MoveReminder *reminder, u16 prevPos, u16 currPos);
static void MoveReminder_UpdateClassSprite(MoveReminder *reminder, u16 move);
static void MoveReminder_UpdateCursor(MoveReminder *reminder, u8 index, u8 palette);
static void ov91_021D2574(MoveReminder *reminder, u8 draw);
static void ov91_021D2594(MoveReminder *reminder);
static void ov91_021D25E4(MoveReminder *reminder);

static const WindowTemplate sMoveReminder_WindowTemplates[] = {
    [MOVE_REMINDER_WINDOW_0] =
    {
        .bgLayer = BG_LAYER_MAIN_1,
        .tilemapLeft = 19,
        .tilemapTop = 0,
        .width = 11,
        .height = 2,
        .palette = 15,
        .baseTile = 0x28
    },
    [MOVE_REMINDER_WINDOW_1] =
    {
        .bgLayer = BG_LAYER_MAIN_1,
        .tilemapLeft = 19,
        .tilemapTop = 0,
        .width = 11,
        .height = 2,
        .palette = 15,
        .baseTile = 0x3E
    },
    [MOVE_REMINDER_WINDOW_2] =
    {
        .bgLayer = BG_LAYER_MAIN_1,
        .tilemapLeft = 1,
        .tilemapTop = 0,
        .width = 6,
        .height = 2,
        .palette = 15,
        .baseTile = 0x54
    },
    [MOVE_REMINDER_WINDOW_3] =
    {
        .bgLayer = BG_LAYER_MAIN_1,
        .tilemapLeft = 1,
        .tilemapTop = 2,
        .width = 6,
        .height = 2,
        .palette = 15,
        .baseTile = 0x60
    },
    [MOVE_REMINDER_WINDOW_4] =
    {
        .bgLayer = BG_LAYER_MAIN_1,
        .tilemapLeft = 1,
        .tilemapTop = 4,
        .width = 8,
        .height = 2,
        .palette = 15,
        .baseTile = 0x6C
    },
    [MOVE_REMINDER_WINDOW_5] =
    {
        .bgLayer = BG_LAYER_MAIN_1,
        .tilemapLeft = 1,
        .tilemapTop = 6,
        .width = 3,
        .height = 2,
        .palette = 15,
        .baseTile = 0x7C
    },
    [MOVE_REMINDER_WINDOW_6] =
    {
        .bgLayer = BG_LAYER_MAIN_1,
        .tilemapLeft = 2,
        .tilemapTop = 2,
        .width = 12,
        .height = 2,
        .palette = 15,
        .baseTile = 0x82
    },
    [MOVE_REMINDER_WINDOW_7] =
    {
        .bgLayer = BG_LAYER_MAIN_1,
        .tilemapLeft = 13,
        .tilemapTop = 2,
        .width = 3,
        .height = 2,
        .palette = 15,
        .baseTile = 0x9A
    },
    [MOVE_REMINDER_WINDOW_8] =
    {
        .bgLayer = BG_LAYER_MAIN_1,
        .tilemapLeft = 13,
        .tilemapTop = 4,
        .width = 3,
        .height = 2,
        .palette = 15,
        .baseTile = 0xA0
    },
    [MOVE_REMINDER_WINDOW_9] =
    {
        .bgLayer = BG_LAYER_MAIN_1,
        .tilemapLeft = 11,
        .tilemapTop = 6,
        .width = 5,
        .height = 2,
        .palette = 15,
        .baseTile = 0xA6
    },
    [MOVE_REMINDER_WINDOW_10] =
    {
        .bgLayer = BG_LAYER_MAIN_1,
        .tilemapLeft = 1,
        .tilemapTop = 8,
        .width = 15,
        .height = 10,
        .palette = 15,
        .baseTile = 0xB0
    },
    [MOVE_REMINDER_WINDOW_11] =
    {
        .bgLayer = BG_LAYER_MAIN_1,
        .tilemapLeft = 1,
        .tilemapTop = 9,
        .width = 15,
        .height = 6,
        .palette = 15,
        .baseTile = 0x146
    },
    [MOVE_REMINDER_WINDOW_12] =
    {
        .bgLayer = BG_LAYER_MAIN_0,
        .tilemapLeft = 2,
        .tilemapTop = 19,
        .width = 27,
        .height = 4,
        .palette = 14,
        .baseTile = 0x1A0
    },
    [MOVE_REMINDER_WINDOW_13] =
    {
        .bgLayer = BG_LAYER_MAIN_1,
        .tilemapLeft = 21,
        .tilemapTop = 3,
        .width = 11,
        .height = 14,
        .palette = 15,
        .baseTile = 0x20C
    },
    [MOVE_REMINDER_WINDOW_14] =
    {
        .bgLayer = BG_LAYER_MAIN_0,
        .tilemapLeft = 23,
        .tilemapTop = 13,
        .width = 7,
        .height = 4,
        .palette = 14,
        .baseTile = 0x2A6
    }
};

static const ListMenuTemplate sMoveReminder_ListMenuTemplate = {
    .choices = NULL,
    .cursorCallback = MoveReminder_MenuCursorCallback,
    .printCallback = MoveReminder_MenuPrintCallback,
    .window = NULL,
    .count = 0,
    .maxDisplay = MAX_ITEM_SHOWN,
    .headerXOffset = 0,
    .textXOffset = 2,
    .cursorXOffset = 0,
    .yOffset = 0,
    .textColorFg = 1,
    .textColorBg = 0,
    .textColorShadow = 2,
    .letterSpacing = 0,
    .lineSpacing = 0,
    .pagerMode = PAGER_MODE_NONE,
    .fontID = FONT_SYSTEM,
    .cursorType = 1,
    .parent = NULL
};

static const u32 sMoveReminder_BankEntryIDs[2][11] = {
    {
        pl_msg_00000645_00000,
        pl_msg_00000645_00001,
        pl_msg_00000645_00002,
        pl_msg_00000645_00003,
        pl_msg_00000645_00004,
        pl_msg_00000645_00005,
        pl_msg_00000645_00006,
        pl_msg_00000645_00007,
        pl_msg_00000645_00008,
        pl_msg_00000645_00009,
        pl_msg_00000645_00010
    },
    {
        pl_msg_00000645_00011,
        pl_msg_00000645_00012,
        pl_msg_00000645_00013,
        pl_msg_00000645_00014,
        pl_msg_00000645_00015,
        pl_msg_00000645_00016,
        pl_msg_00000645_00017,
        pl_msg_00000645_00018,
        pl_msg_00000645_00019,
        pl_msg_00000645_00020,
        pl_msg_00000645_00021
    }
};

static const GameWindowLayout_1 Unk_ov91_021D27DC[] = {
    { MoveReminder_PrintMonLearnedMove, ov91_021D1EE8 },
    { ov91_021D1F38, ov91_021D1F44 },
    { ov91_021D1F94, ov91_021D1FA0 },
    { ov91_021D1FBC, ov91_021D1FD8 },
    { ov91_021D1FF4, ov91_021D2008 }
};

static const SpriteTemplate sMoveReminder_SpriteTemplates[] = {
    [MOVE_REMINDER_SPRITE_0] =
    {
        .x = 146,
        .y = 8,
        .z = 0,
        .animIdx = 0,
        .priority = 0,
        .plttIdx = 0,
        .vramType = NNS_G2D_VRAM_TYPE_2DMAIN,
        .resources = { 0xB8A8, 0xB8A8, 0xB8A8, 0xB8A8, 0x0, 0x0 },
        .bgPriority = 2,
        .vramTransfer = FALSE,
    },
    [MOVE_REMINDER_SPRITE_1] =
    {
        .x = 246,
        .y = 8,
        .z = 0,
        .animIdx = 1,
        .priority = 0,
        .plttIdx = 0,
        .vramType = NNS_G2D_VRAM_TYPE_2DMAIN,
        .resources = { 0xB8A8, 0xB8A8, 0xB8A8, 0xB8A8, 0x0, 0x0 },
        .bgPriority = 2,
        .vramTransfer = FALSE,
    },
    [MOVE_REMINDER_SPRITE_2] =
    {
        .x = 196,
        .y = 20,
        .z = 0,
        .animIdx = 0,
        .priority = 0,
        .plttIdx = 0,
        .vramType = NNS_G2D_VRAM_TYPE_2DMAIN,
        .resources = { 0xB8A9, 0xB8A8, 0xB8A9, 0xB8A9, 0x0, 0x0 },
        .bgPriority = 2,
        .vramTransfer = FALSE,
    },
    [MOVE_REMINDER_SPRITE_3] =
    {
        .x = 196,
        .y = 140,
        .z = 0,
        .animIdx = 1,
        .priority = 0,
        .plttIdx = 0,
        .vramType = NNS_G2D_VRAM_TYPE_2DMAIN,
        .resources = { 0xB8A9, 0xB8A8, 0xB8A9, 0xB8A9, 0x0, 0x0 },
        .bgPriority = 2,
        .vramTransfer = FALSE,
    },
    [MOVE_REMINDER_SPRITE_CURSOR] =
    {
        .x = 196,
        .y = 32,
        .z = 0,
        .animIdx = 0,
        .priority = 0,
        .plttIdx = 0,
        .vramType = NNS_G2D_VRAM_TYPE_2DMAIN,
        .resources = { 0xB8AA, 0xB8A8, 0xB8AA, 0xB8AA, 0x0, 0x0 },
        .bgPriority = 1,
        .vramTransfer = FALSE,
    },
    [MOVE_REMINDER_SPRITE_5] =
    {
        .x = 116,
        .y = 8,
        .z = 0,
        .animIdx = 0,
        .priority = 0,
        .plttIdx = 2,
        .vramType = NNS_G2D_VRAM_TYPE_2DMAIN,
        .resources = { 0xB8AB, 0xB8A8, 0xB8AB, 0xB8AB, 0x0, 0x0 },
        .bgPriority = 2,
        .vramTransfer = FALSE,
    },
    [MOVE_REMINDER_SPRITE_6] =
    {
        .x = 152,
        .y = 32,
        .z = 0,
        .animIdx = 0,
        .priority = 1,
        .plttIdx = 2,
        .vramType = NNS_G2D_VRAM_TYPE_2DMAIN,
        .resources = { 0xB8AC, 0xB8A8, 0xB8AB, 0xB8AB, 0x0, 0x0 },
        .bgPriority = 2,
        .vramTransfer = FALSE,
    },
    [MOVE_REMINDER_SPRITE_7] =
    {
        .x = 152,
        .y = 48,
        .z = 0,
        .animIdx = 0,
        .priority = 1,
        .plttIdx = 2,
        .vramType = NNS_G2D_VRAM_TYPE_2DMAIN,
        .resources = { 0xB8AD, 0xB8A8, 0xB8AB, 0xB8AB, 0x0, 0x0 },
        .bgPriority = 2,
        .vramTransfer = FALSE,
    },
    [MOVE_REMINDER_SPRITE_8] =
    {
        .x = 152,
        .y = 64,
        .z = 0,
        .animIdx = 0,
        .priority = 1,
        .plttIdx = 2,
        .vramType = NNS_G2D_VRAM_TYPE_2DMAIN,
        .resources = { 0xB8AE, 0xB8A8, 0xB8AB, 0xB8AB, 0x0, 0x0 },
        .bgPriority = 2,
        .vramTransfer = FALSE,
    },
    [MOVE_REMINDER_SPRITE_9] =
    {
        .x = 152,
        .y = 80,
        .z = 0,
        .animIdx = 0,
        .priority = 1,
        .plttIdx = 2,
        .vramType = NNS_G2D_VRAM_TYPE_2DMAIN,
        .resources = { 0xB8AF, 0xB8A8, 0xB8AB, 0xB8AB, 0x0, 0x0 },
        .bgPriority = 2,
        .vramTransfer = FALSE,
    },
    [MOVE_REMINDER_SPRITE_10] =
    {
        .x = 152,
        .y = 96,
        .z = 0,
        .animIdx = 0,
        .priority = 1,
        .plttIdx = 2,
        .vramType = NNS_G2D_VRAM_TYPE_2DMAIN,
        .resources = { 0xB8B0, 0xB8A8, 0xB8AB, 0xB8AB, 0x0, 0x0 },
        .bgPriority = 2,
        .vramTransfer = FALSE,
    },
    [MOVE_REMINDER_SPRITE_11] =
    {
        .x = 152,
        .y = 112,
        .z = 0,
        .animIdx = 0,
        .priority = 1,
        .plttIdx = 2,
        .vramType = NNS_G2D_VRAM_TYPE_2DMAIN,
        .resources = { 0xB8B1, 0xB8A8, 0xB8AB, 0xB8AB, 0x0, 0x0 },
        .bgPriority = 2,
        .vramTransfer = FALSE,
    },
    [MOVE_REMINDER_SPRITE_12] =
    {
        .x = 152,
        .y = 128,
        .z = 0,
        .animIdx = 0,
        .priority = 1,
        .plttIdx = 2,
        .vramType = NNS_G2D_VRAM_TYPE_2DMAIN,
        .resources = { 0xB8B2, 0xB8A8, 0xB8AB, 0xB8AB, 0x0, 0x0 },
        .bgPriority = 2,
        .vramTransfer = FALSE,
    },
};

int MoveReminder_Init(OverlayManager *overlayMan, int *state)
{
    MoveReminder *reminder;

    Heap_Create(HEAP_ID_APPLICATION, HEAP_ID_MOVE_REMINDER, HEAP_SIZE_MOVE_REMINDER_APP);

    reminder = OverlayManager_NewData(overlayMan, sizeof(MoveReminder), HEAP_ID_MOVE_REMINDER);
    memset(reminder, 0, sizeof(MoveReminder));
    reminder->data = OverlayManager_Args(overlayMan);

    MoveReminder_LoadGraphics(reminder);
    MoveReminder_UpdateCursor(reminder, reminder->data->cursorPos, 0);
    MoveReminder_FormatStrbuf(reminder, FMTSTR_TEACH_WHICH_MOVE);
    MoveReminder_PrintStrbuf(reminder, MOVE_REMINDER_WINDOW_12, FONT_MESSAGE, TEXT_COLOR(1, 2, 15), ALIGN_MODE_LEFT);

    reminder->state = 1;

    Window_ScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_12]);
    ov91_021D2594(reminder);
    SetVBlankCallback(MoveReminder_VBlank, reminder);

    return TRUE;
}

int MoveReminder_Main(OverlayManager *overlayMan, int *state)
{
    MoveReminder *reminder = OverlayManager_Data(overlayMan);

    switch (*state) {
    case 0:
        *state = MoveReminder_WaitScreenTransition(reminder);
        break;
    case 1:
        *state = MoveReminder_Select(reminder);
        break;
    case 2:
        *state = MoveReminder_HasTextPrinterFinished(reminder);
        break;
    case 3:
        *state = MoveReminder_MakeYesNoChoice(reminder);
        break;
    case 4:
        *state = MoveReminder_SelectYesNo(reminder);
        break;
    case 5:
        *state = MoveReminder_SetMoveSlotToSelectedMove(reminder);
        break;
    case 6:
        *state = MoveReminder_PrintMonLearnedMove2(reminder);
        break;
    case 7:
        *state = MoveReminder_PrintForgotOrStopTeachingMove(reminder);
        break;
    case 8:
        *state = MoveReminder_ScreenTransitionOut(reminder);
        break;
    case 9:
        return TRUE;
    case 10:
        *state = MoveReminder_ScreenTransitionIn(reminder);
        break;
    case 11:
        *state = MoveReminder_OpenSummaryScreenApp(reminder);
        return FALSE;
    case 12:
        *state = MoveReminder_ReloadApp(reminder);
        return FALSE;
    }

    MoveReminder_UpdateSpritesAnim(reminder);
    SpriteSystem_DrawSprites(reminder->spriteMan);

    return FALSE;
}

int MoveReminder_Exit(OverlayManager *overlayMan, int *state)
{
    MoveReminder *reminder = OverlayManager_Data(overlayMan);

    MoveReminder_Free(reminder);
    OverlayManager_FreeData(overlayMan);
    Heap_Destroy(HEAP_ID_MOVE_REMINDER);

    return TRUE;
}

static void MoveReminder_LoadGraphics(MoveReminder *reminder)
{
    SetVBlankCallback(NULL, NULL);
    DisableHBlank();

    GXLayers_DisableEngineALayers();
    GXLayers_DisableEngineBLayers();

    GX_SetVisiblePlane(0);
    GXS_SetVisiblePlane(0);

    reminder->bgConfig = BgConfig_New(HEAP_ID_MOVE_REMINDER);

    sub_0208C120(0, HEAP_ID_MOVE_REMINDER);

    NARC *narc = NARC_ctor(NARC_INDEX_GRAPHIC__WAZA_OSHIE_GRA, HEAP_ID_MOVE_REMINDER);

    MoveReminder_SetGXLayersBanks();
    MoveReminder_SetBGLayers(reminder->bgConfig);
    MoveReminder_LoadGraphicsFromNARC(reminder, narc);
    MoveReminder_LoadSprites(reminder, narc);
    MoveReminder_LoadStrings(reminder);
    MoveReminder_LoadWindows(reminder);
    MoveReminder_InitListMenu(reminder);
    MoveReminder_LoadSelectedMoveInfo(reminder);

    SetVBlankCallback(MoveReminder_VBlank, reminder);
    NARC_dtor(narc);
}

static void MoveReminder_Free(MoveReminder *reminder)
{
    MoveReminder_FreeListMenu(reminder);
    MoveReminder_FreeWindows(reminder);
    MoveReminder_FreeBgConfig(reminder->bgConfig);
    MoveReminder_FreeStrings(reminder);
    MoveReminder_FreeSprites(reminder);

    VramTransfer_Free();
    SetVBlankCallback(NULL, NULL);
}

static void MoveReminder_VBlank(void *data)
{
    MoveReminder *reminder = data;

    Bg_RunScheduledUpdates(reminder->bgConfig);
    VramTransfer_Process();
    SpriteSystem_TransferOam();

    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

static void MoveReminder_SetGXLayersBanks(void)
{
    UnkStruct_02099F80 banks = {
        .unk_00 = GX_VRAM_BG_128_A,
        .unk_04 = GX_VRAM_BGEXTPLTT_NONE,
        .unk_08 = GX_VRAM_SUB_BG_128_C,
        .unk_0C = GX_VRAM_SUB_BGEXTPLTT_NONE,
        .unk_10 = GX_VRAM_OBJ_128_B,
        .unk_14 = GX_VRAM_OBJEXTPLTT_NONE,
        .unk_18 = GX_VRAM_SUB_OBJ_16_I,
        .unk_1C = GX_VRAM_SUB_OBJEXTPLTT_NONE,
        .unk_20 = GX_VRAM_TEX_NONE,
        .unk_24 = GX_VRAM_TEXPLTT_NONE
    };

    GXLayers_SetBanks(&banks);
}

static void MoveReminder_SetBGLayers(BgConfig *bgConfig)
{
    GraphicsModes graphicsModes = {
        .displayMode = GX_DISPMODE_GRAPHICS,
        .mainBgMode = GX_BGMODE_0,
        .subBgMode = GX_BGMODE_0,
        .bg0As2DOr3D = GX_BG0_AS_2D,
    };

    SetAllGraphicsModes(&graphicsModes);

    BgTemplate bgMain0Template = {
        .x = 0,
        .y = 0,
        .bufferSize = 0x800,
        .baseTile = 0,
        .screenSize = 1,
        .colorMode = GX_BG_COLORMODE_16,
        .screenBase = GX_BG_SCRBASE_0xf800,
        .charBase = GX_BG_CHARBASE_0x00000,
        .bgExtPltt = GX_BG_EXTPLTT_01,
        .priority = 0,
        .areaOver = 0,
        .dummy = 0,
        .mosaic = 0
    };

    Bg_InitFromTemplate(bgConfig, BG_LAYER_MAIN_0, &bgMain0Template, BG_TYPE_STATIC);
    Bg_ClearTilemap(bgConfig, BG_LAYER_MAIN_0);

    BgTemplate bgMain1Template = {
        .x = 0,
        .y = 0,
        .bufferSize = 0x800,
        .baseTile = 0,
        .screenSize = 1,
        .colorMode = GX_BG_COLORMODE_16,
        .screenBase = GX_BG_SCRBASE_0xf000,
        .charBase = GX_BG_CHARBASE_0x00000,
        .bgExtPltt = GX_BG_EXTPLTT_01,
        .priority = 1,
        .areaOver = 0,
        .dummy = 0,
        .mosaic = 0
    };

    Bg_InitFromTemplate(bgConfig, BG_LAYER_MAIN_1, &bgMain1Template, BG_TYPE_STATIC);
    Bg_ClearTilemap(bgConfig, BG_LAYER_MAIN_1);

    BgTemplate bgMain2Template = {
        .x = 0,
        .y = 0,
        .bufferSize = 0x1000,
        .baseTile = 0,
        .screenSize = 3,
        .colorMode = GX_BG_COLORMODE_16,
        .screenBase = GX_BG_SCRBASE_0xe000,
        .charBase = GX_BG_CHARBASE_0x10000,
        .bgExtPltt = GX_BG_EXTPLTT_01,
        .priority = 2,
        .areaOver = 0,
        .dummy = 0,
        .mosaic = 0
    };

    Bg_InitFromTemplate(bgConfig, BG_LAYER_MAIN_2, &bgMain2Template, BG_TYPE_STATIC);
    Bg_ClearTilemap(bgConfig, BG_LAYER_MAIN_2);

    Bg_ClearTilesRange(0, 32, 0, HEAP_ID_MOVE_REMINDER);
}

static void MoveReminder_FreeBgConfig(BgConfig *bgConfig)
{
    GXLayers_EngineAToggleLayers(GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_OBJ, FALSE);
    Bg_FreeTilemapBuffer(bgConfig, BG_LAYER_MAIN_2);
    Bg_FreeTilemapBuffer(bgConfig, BG_LAYER_MAIN_1);
    Bg_FreeTilemapBuffer(bgConfig, BG_LAYER_MAIN_0);
    Heap_FreeToHeapExplicit(HEAP_ID_MOVE_REMINDER, bgConfig);
}

static void MoveReminder_LoadGraphicsFromNARC(MoveReminder *reminder, NARC *narc)
{
    Graphics_LoadTilesToBgLayerFromOpenNARC(narc, 10, reminder->bgConfig, BG_LAYER_MAIN_2, 0, 0, FALSE, HEAP_ID_MOVE_REMINDER);
    Graphics_LoadTilemapToBgLayerFromOpenNARC(narc, 11, reminder->bgConfig, BG_LAYER_MAIN_2, 0, 0, FALSE, HEAP_ID_MOVE_REMINDER);
    Graphics_LoadPaletteFromOpenNARC(narc, 12, 0, 0, 0, HEAP_ID_MOVE_REMINDER);
    Font_LoadScreenIndicatorsPalette(0, 14 * 32, HEAP_ID_MOVE_REMINDER);
    LoadStandardWindowGraphics(reminder->bgConfig, BG_LAYER_MAIN_0, 1, 12, 0, HEAP_ID_MOVE_REMINDER);
    LoadMessageBoxGraphics(reminder->bgConfig, BG_LAYER_MAIN_0, 10, FIELD_MESSAGE_PALETTE_INDEX, Options_Frame(reminder->data->options), HEAP_ID_MOVE_REMINDER);
}

static void MoveReminder_LoadWindows(MoveReminder *reminder)
{
    for (u32 i = MOVE_REMINDER_SPRITE_0; i < MOVE_REMINDER_WINDOW_MAX; i++) {
        Window_AddFromTemplate(reminder->bgConfig, &reminder->window[i], &sMoveReminder_WindowTemplates[i]);
        Window_FillTilemap(&reminder->window[i], 0);
    }

    MoveReminder_PrintStaticInfo(reminder);

    Window_FillTilemap(&reminder->window[MOVE_REMINDER_WINDOW_12], 15);
    Window_DrawMessageBoxWithScrollCursor(&reminder->window[MOVE_REMINDER_WINDOW_12], 0, (1 + 9), 13);
}

static void MoveReminder_FreeWindows(MoveReminder *reminder)
{
    for (u32 i = MOVE_REMINDER_SPRITE_0; i < MOVE_REMINDER_WINDOW_MAX; i++) {
        Window_Remove(&reminder->window[i]);
    }
}

static void MoveReminder_LoadStrings(MoveReminder *reminder)
{
    reminder->msgLoader = MessageLoader_Init(MESSAGE_LOADER_BANK_HANDLE, NARC_INDEX_MSGDATA__PL_MSG, TEXT_BANK_UNK_0645, HEAP_ID_MOVE_REMINDER);
    reminder->strTemplate = StringTemplate_Default(HEAP_ID_MOVE_REMINDER);
    reminder->strbuf = Strbuf_Init(256, HEAP_ID_MOVE_REMINDER);
}

static void MoveReminder_FreeStrings(MoveReminder *reminder)
{
    MessageLoader_Free(reminder->msgLoader);
    StringTemplate_Free(reminder->strTemplate);
    Strbuf_Free(reminder->strbuf);
}

static int MoveReminder_WaitScreenTransition(MoveReminder *reminder)
{
    if (IsScreenTransitionDone() == TRUE) {
        return reminder->state;
    }

    return FALSE;
}

static int MoveReminder_Select(MoveReminder *reminder)
{
    u32 input;
    u16 listPos, cursorPos;

    if (JOY_NEW(PAD_KEY_LEFT | PAD_KEY_RIGHT)) {
        Sound_PlayEffect(SEQ_SE_DP_DECIDE);
        reminder->data->unk_14 ^= 1;
        MoveReminder_LoadSelectedMoveInfo(reminder);
        return TRUE;
    }

    ListMenu_GetListAndCursorPos(reminder->list, &listPos, &cursorPos);
    input = ListMenu_ProcessInput(reminder->list);
    ListMenu_GetListAndCursorPos(reminder->list, &reminder->data->listPos, &reminder->data->cursorPos);
    MoveReminder_MoveTypeSprites(reminder, listPos, reminder->data->listPos);

    if (reminder->data->cursorPos != cursorPos) {
        MoveReminder_UpdateCursor(reminder, reminder->data->cursorPos, 0);
    }

    ov91_021D2594(reminder);

    switch (input) {
    case LIST_NOTHING_CHOSEN:
        break;
    case LIST_CANCEL:
        Sound_PlayEffect(SEQ_SE_DP_DECIDE);
        MoveReminder_UpdateCursor(reminder, reminder->data->cursorPos, 1);
        ov91_021D25E4(reminder);
        MoveReminder_FormatAndPrintStrbuf(reminder, FMTSTR_GIVE_UP_TEACHING_MOVE);
        reminder->unk_186 = 1;
        reminder->state = 3;
        return 2;
    default:
        Sound_PlayEffect(SEQ_SE_DP_DECIDE);
        MoveReminder_UpdateCursor(reminder, reminder->data->cursorPos, 1);
        ov91_021D25E4(reminder);

        if (MoveReminder_CalculateTotalMoves(reminder) < 4) {
            MoveReminder_FormatAndPrintStrbuf(reminder, FMTSTR_TEACH_THE_MOVE);
            reminder->unk_186 = 0;
            reminder->state = 3;
        } else {
            MoveReminder_FormatAndPrintStrbuf(reminder, FMTSTR_MON_TRYING_TO_LEARN_MOVE);
            reminder->unk_186 = 2;
            reminder->state = 3;
        }

        return 2;
    }

    return TRUE;
}

static int MoveReminder_HasTextPrinterFinished(MoveReminder *reminder)
{
    if (Text_IsPrinterActive(reminder->txtPrinterID) == FALSE) {
        return reminder->state;
    }

    return 2;
}

static int MoveReminder_MakeYesNoChoice(MoveReminder *reminder)
{
    reminder->menu = Menu_MakeYesNoChoice(reminder->bgConfig, &sMoveReminder_WindowTemplates[14], 1, 12, HEAP_ID_MOVE_REMINDER);
    return 4;
}

static int MoveReminder_SelectYesNo(MoveReminder *reminder)
{
    switch (Menu_ProcessInputAndHandleExit(reminder->menu, HEAP_ID_MOVE_REMINDER)) {
    case 0:
        return Unk_ov91_021D27DC[reminder->unk_186].unk_00(reminder);
    case MENU_CANCELED:
        return Unk_ov91_021D27DC[reminder->unk_186].unk_04(reminder);
    }

    return 4;
}

static int MoveReminder_SetMoveSlotToSelectedMove(MoveReminder *reminder)
{
    u32 val = MoveReminder_SelectedMove(reminder);
    Pokemon_SetValue(reminder->data->mon, MON_DATA_MOVE1 + reminder->data->moveSlot, &val);

    val = 0;
    Pokemon_SetValue(reminder->data->mon, MON_DATA_MOVE1_PP_UPS + reminder->data->moveSlot, &val);

    val = MoveTable_CalcMaxPP(MoveReminder_SelectedMove(reminder), 0);
    Pokemon_SetValue(reminder->data->mon, MON_DATA_MOVE1_CUR_PP + reminder->data->moveSlot, &val);

    reminder->data->unk_16 = FALSE;

    return 8;
}

static int MoveReminder_PrintMonLearnedMove2(MoveReminder *reminder)
{
    MoveReminder_FormatAndPrintStrbuf(reminder, FMTSTR_MON_LEARNED_MOVE2);
    reminder->state = 5;
    return 2;
}

static int MoveReminder_PrintForgotOrStopTeachingMove(MoveReminder *reminder)
{
    if (reminder->data->moveSlot < LEARNED_MOVES_MAX) {
        MoveReminder_FormatAndPrintStrbuf(reminder, FMTSTR_MAKE_MON_FORGOT_MOVE);
        reminder->unk_186 = 4;
    } else {
        MoveReminder_FormatAndPrintStrbuf(reminder, FMTSTR_MON_STOP_TEACHING_MOVE);
        reminder->unk_186 = 3;
    }

    reminder->state = 3;

    return 2;
}

static int MoveReminder_ScreenTransitionOut(MoveReminder *reminder)
{
    sub_0208C120(1, HEAP_ID_MOVE_REMINDER);
    reminder->state = 9;
    return FALSE;
}

static void MoveReminder_LoadSelectedMoveInfo(MoveReminder *reminder)
{
    u16 move = MoveReminder_SelectedMove(reminder);

    if (reminder->data->unk_14 == 0) {
        Bg_ScheduleScroll(reminder->bgConfig, BG_LAYER_MAIN_2, BG_OFFSET_UPDATE_SET_X, 0);
        Window_ClearAndScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_1]);
        Window_ClearAndScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_6]);
        Window_ClearAndScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_11]);
        Window_ScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_0]);

        if (move != (u16)-1) {
            MoveReminder_PrintBattleInfo(reminder, move);
        } else {
            MoveReminder_PrintBattleInfo(reminder, LIST_CANCEL);
        }
    } else {
        Bg_ScheduleScroll(reminder->bgConfig, BG_LAYER_MAIN_2, BG_OFFSET_UPDATE_SET_X, 256);
        Window_ClearAndScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_0]);
        Window_ClearAndScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_2]);
        Window_ClearAndScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_3]);
        Window_ClearAndScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_4]);
        Window_ClearAndScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_5]);
        Window_ClearAndScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_10]);
        Window_ClearAndScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_7]);
        Window_ClearAndScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_8]);
        Window_ClearAndScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_9]);
        Window_ScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_1]);

        if (move != (u16)-1) {
            MoveReminder_PrintContestInfo(reminder, move);
        } else {
            MoveReminder_PrintContestInfo(reminder, LIST_CANCEL);
        }

        ManagedSprite_SetDrawFlag(reminder->managedSprite[MOVE_REMINDER_SPRITE_5], 0);
    }

    MoveReminder_UpdateTypeSprites(reminder);
}

static void MoveReminder_PrintStrbuf(MoveReminder *reminder, u32 windowID, u32 fontID, TextColor color, enum XAlignmentModes xAlignMode)
{
    u8 strWidth, winWidth, xOffset;

    switch (xAlignMode) {
    case ALIGN_MODE_LEFT:
        xOffset = 0;
        break;
    case ALIGN_MODE_RIGHT: // unused
        strWidth = Font_CalcStrbufWidth(FONT_SYSTEM, reminder->strbuf, 0);
        winWidth = Window_GetWidth(&reminder->window[windowID]) * 8;
        xOffset = winWidth - strWidth;
        break;
    case ALIGN_MODE_CENTER:
        strWidth = Font_CalcStrbufWidth(FONT_SYSTEM, reminder->strbuf, 0);
        winWidth = Window_GetWidth(&reminder->window[windowID]) * 8;
        xOffset = (winWidth - strWidth) / 2;
        break;
    }

    Text_AddPrinterWithParamsAndColor(&reminder->window[windowID], fontID, reminder->strbuf, xOffset, 0, TEXT_SPEED_NO_TRANSFER, color, NULL);
}

static void ov91_021D1618(MoveReminder *reminder, u32 entryID, u32 val, u8 maxDigits, u8 paddingMode)
{
    Strbuf *fmtString = MessageLoader_GetNewStrbuf(reminder->msgLoader, entryID);
    StringTemplate_SetNumber(reminder->strTemplate, 0, val, maxDigits, paddingMode, CHARSET_MODE_EN);
    StringTemplate_Format(reminder->strTemplate, reminder->strbuf, fmtString);
    Strbuf_Free(fmtString);
}

static void MoveReminder_PrintStaticInfo(MoveReminder *reminder)
{
    MessageLoader_GetStrbuf(reminder->msgLoader, pl_msg_00000645_00026, reminder->strbuf);
    MoveReminder_PrintStrbuf(reminder, MOVE_REMINDER_WINDOW_0, FONT_SYSTEM, TEXT_COLOR(15, 14, 0), ALIGN_MODE_CENTER);

    MessageLoader_GetStrbuf(reminder->msgLoader, pl_msg_00000645_00028, reminder->strbuf);
    MoveReminder_PrintStrbuf(reminder, MOVE_REMINDER_WINDOW_1, FONT_SYSTEM, TEXT_COLOR(15, 14, 0), ALIGN_MODE_CENTER);

    MessageLoader_GetStrbuf(reminder->msgLoader, pl_msg_00000645_00022, reminder->strbuf);
    MoveReminder_PrintStrbuf(reminder, MOVE_REMINDER_WINDOW_2, FONT_SYSTEM, TEXT_COLOR(15, 14, 0), ALIGN_MODE_LEFT);

    MessageLoader_GetStrbuf(reminder->msgLoader, pl_msg_00000645_00023, reminder->strbuf);
    MoveReminder_PrintStrbuf(reminder, MOVE_REMINDER_WINDOW_3, FONT_SYSTEM, TEXT_COLOR(15, 14, 0), ALIGN_MODE_LEFT);

    MessageLoader_GetStrbuf(reminder->msgLoader, pl_msg_00000645_00024, reminder->strbuf);
    MoveReminder_PrintStrbuf(reminder, MOVE_REMINDER_WINDOW_4, FONT_SYSTEM, TEXT_COLOR(15, 14, 0), ALIGN_MODE_LEFT);

    MessageLoader_GetStrbuf(reminder->msgLoader, pl_msg_00000645_00025, reminder->strbuf);
    MoveReminder_PrintStrbuf(reminder, MOVE_REMINDER_WINDOW_5, FONT_SYSTEM, TEXT_COLOR(1, 2, 0), ALIGN_MODE_LEFT);

    MessageLoader_GetStrbuf(reminder->msgLoader, pl_msg_00000645_00027, reminder->strbuf);
    MoveReminder_PrintStrbuf(reminder, MOVE_REMINDER_WINDOW_6, FONT_SYSTEM, TEXT_COLOR(15, 14, 0), ALIGN_MODE_CENTER);
}

static u32 ov91_021D175C(MoveReminder *reminder)
{
    u32 i;

    for (i = 0; i < 256; i++) {
        if (reminder->data->movesList[i] == (u16)-1) {
            break;
        }
    }

    return i;
}

static void MoveReminder_InitListMenu(MoveReminder *reminder)
{
    MessageLoader *msgLoader;
    ListMenuTemplate template;

    reminder->movesCount = (u8)ov91_021D175C(reminder) + 1;
    reminder->strList = StringList_New(reminder->movesCount, HEAP_ID_MOVE_REMINDER);

    msgLoader = MessageLoader_Init(MESSAGE_LOADER_BANK_HANDLE, NARC_INDEX_MSGDATA__PL_MSG, TEXT_BANK_MOVE_NAMES, HEAP_ID_MOVE_REMINDER);

    for (u32 i = 0; i < reminder->movesCount; i++) {
        if (reminder->data->movesList[i] != (u16)-1) {
            StringList_AddFromMessageBank(reminder->strList, msgLoader, reminder->data->movesList[i], reminder->data->movesList[i]);
        } else {
            StringList_AddFromMessageBank(reminder->strList, reminder->msgLoader, pl_msg_00000645_00032, LIST_CANCEL);
            break;
        }
    }

    MessageLoader_Free(msgLoader);

    template = sMoveReminder_ListMenuTemplate;
    template.choices = reminder->strList;
    template.window = &reminder->window[MOVE_REMINDER_WINDOW_13];
    template.count = reminder->movesCount;
    template.parent = (void *)reminder;

    reminder->list = ListMenu_New(&template, reminder->data->listPos, reminder->data->cursorPos, HEAP_ID_MOVE_REMINDER);

    Window_ScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_13]);
}

static void MoveReminder_FreeListMenu(MoveReminder *reminder)
{
    ListMenu_Free(reminder->list, &reminder->data->listPos, &reminder->data->cursorPos);
    StringList_Free(reminder->strList);
}

static void MoveReminder_MenuCursorCallback(ListMenu *listMenu, u32 index, u8 onInit)
{
    MoveReminder *reminder = (MoveReminder *)ListMenu_GetAttribute(listMenu, 19);

    if (onInit != TRUE) {
        Sound_PlayEffect(SEQ_SE_DP_DECIDE);
    }

    if (reminder->data->unk_14 == 0) {
        MoveReminder_PrintBattleInfo(reminder, index);
    } else {
        MoveReminder_PrintContestInfo(reminder, index);
    }
}

// clever gamefreak, clever.
static void MoveReminder_MenuPrintCallback(ListMenu *listMenu, u32 index, u8 yOffset)
{
    return;
}

static void MoveReminder_PrintBattleInfo(MoveReminder *reminder, u32 index)
{
    Window_FillTilemap(&reminder->window[MOVE_REMINDER_WINDOW_10], 0);
    Window_FillTilemap(&reminder->window[MOVE_REMINDER_WINDOW_7], 0);
    Window_FillTilemap(&reminder->window[MOVE_REMINDER_WINDOW_8], 0);
    Window_FillTilemap(&reminder->window[MOVE_REMINDER_WINDOW_9], 0);
    ManagedSprite_SetDrawFlag(reminder->managedSprite[MOVE_REMINDER_SPRITE_5], FALSE);

    if (index != LIST_CANCEL) {
        MessageLoader *msgLoader;
        u32 moveInfo;

        moveInfo = MoveTable_LoadParam(index, MOVEATTRIBUTE_POWER);

        if (moveInfo <= 1) {
            MessageLoader_GetStrbuf(reminder->msgLoader, pl_msg_00000645_00033, reminder->strbuf);
        } else {
            ov91_021D1618(reminder, 29, moveInfo, 3, 1);
        }

        MoveReminder_PrintStrbuf(reminder, MOVE_REMINDER_WINDOW_7, FONT_SYSTEM, TEXT_COLOR(1, 2, 0), ALIGN_MODE_CENTER);

        moveInfo = MoveTable_LoadParam(index, MOVEATTRIBUTE_ACCURACY);

        if (moveInfo == 0) {
            MessageLoader_GetStrbuf(reminder->msgLoader, pl_msg_00000645_00033, reminder->strbuf);
        } else {
            ov91_021D1618(reminder, 30, moveInfo, 3, 1);
        }

        MoveReminder_PrintStrbuf(reminder, MOVE_REMINDER_WINDOW_8, FONT_SYSTEM, TEXT_COLOR(1, 2, 0), ALIGN_MODE_CENTER);

        moveInfo = MoveTable_CalcMaxPP(index, 0);
        ov91_021D1618(reminder, 31, moveInfo, 2, 0);
        MoveReminder_PrintStrbuf(reminder, MOVE_REMINDER_WINDOW_9, FONT_SYSTEM, TEXT_COLOR(1, 2, 0), ALIGN_MODE_CENTER);

        msgLoader = MessageLoader_Init(MESSAGE_LOADER_NARC_HANDLE, NARC_INDEX_MSGDATA__PL_MSG, TEXT_BANK_MOVE_DESCRIPTIONS, HEAP_ID_MOVE_REMINDER);
        MessageLoader_GetStrbuf(msgLoader, index, reminder->strbuf);
        MoveReminder_PrintStrbuf(reminder, MOVE_REMINDER_WINDOW_10, FONT_SYSTEM, TEXT_COLOR(1, 2, 0), ALIGN_MODE_LEFT);
        MessageLoader_Free(msgLoader);
        MoveReminder_UpdateClassSprite(reminder, index);
        ManagedSprite_SetDrawFlag(reminder->managedSprite[MOVE_REMINDER_SPRITE_5], TRUE);
        Window_ScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_2]);
        Window_ScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_3]);
        Window_ScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_4]);
        Window_ScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_5]);
    } else {
        Window_ClearAndScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_2]);
        Window_ClearAndScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_3]);
        Window_ClearAndScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_4]);
        Window_ClearAndScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_5]);
    }

    Window_ScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_10]);
    Window_ScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_7]);
    Window_ScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_8]);
    Window_ScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_9]);
}

static void MoveReminder_PrintContestInfo(MoveReminder *reminder, u32 index)
{
    Window_FillTilemap(&reminder->window[MOVE_REMINDER_WINDOW_11], 0);

    if (index != LIST_CANCEL) {
        MessageLoader *msgLoader;
        u32 moveInfo, entryID;

        moveInfo = MoveTable_LoadParam(index, MOVEATTRIBUTE_CONTEST_EFFECT);
        entryID = sub_0209577C(moveInfo);
        msgLoader = MessageLoader_Init(MESSAGE_LOADER_BANK_HANDLE, NARC_INDEX_MSGDATA__PL_MSG, TEXT_BANK_CONTEST_EFFECTS, HEAP_ID_MOVE_REMINDER);

        MessageLoader_GetStrbuf(msgLoader, entryID, reminder->strbuf);
        Text_AddPrinterWithParamsAndColor(&reminder->window[MOVE_REMINDER_WINDOW_11], FONT_SYSTEM, reminder->strbuf, 0, 0, TEXT_SPEED_NO_TRANSFER, TEXT_COLOR(1, 2, 0), NULL);
        MessageLoader_Free(msgLoader);
        MoveReminder_PrintContestHeartsInfo(reminder, index);
        Window_ScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_6]);
    } else {
        MoveReminder_PrintContestHeartsInfo(reminder, (u16)-1);
        Window_ClearAndScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_6]);
    }

    Window_ScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_11]);
}

static void MoveReminder_PrintContestHeart(MoveReminder *reminder, u16 fillVal, u8 x)
{
    Bg_FillTilemapRect(reminder->bgConfig, BG_LAYER_MAIN_2, fillVal, 34 + x * 2, 5, 1, 1, 16);
    Bg_FillTilemapRect(reminder->bgConfig, BG_LAYER_MAIN_2, fillVal + 1, 34 + x * 2 + 1, 5, 1, 1, 16);
    Bg_FillTilemapRect(reminder->bgConfig, BG_LAYER_MAIN_2, fillVal + 32, 34 + x * 2, 5 + 1, 1, 1, 16);
    Bg_FillTilemapRect(reminder->bgConfig, BG_LAYER_MAIN_2, fillVal + 33, 34 + x * 2 + 1, 5 + 1, 1, 1, 16);
}

static void ov91_021D1B94(MoveReminder *reminder)
{
    for (u16 i = 0; i < 6; i++) {
        MoveReminder_PrintContestHeart(reminder, 18, i);
    }

    Bg_ScheduleTilemapTransfer(reminder->bgConfig, BG_LAYER_MAIN_2);
}

static void MoveReminder_PrintContestHeartsInfo(MoveReminder *reminder, u16 index)
{
    u32 moveEffect;
    s8 v2;

    ov91_021D1B94(reminder);

    if (index != (u16)-1) {
        moveEffect = MoveTable_LoadParam(index, MOVEATTRIBUTE_CONTEST_EFFECT);
        v2 = sub_02095734(moveEffect) / 10;

        for (u16 i = 0; i < v2; i++) {
            MoveReminder_PrintContestHeart(reminder, 14, i);
        }
    }

    Bg_ScheduleTilemapTransfer(reminder->bgConfig, BG_LAYER_MAIN_2);
}

static void MoveReminder_FormatStrbuf(MoveReminder *reminder, enum FormatStrings fmt)
{
    switch (fmt) {
    case FMTSTR_TEACH_WHICH_MOVE:
        StringTemplate_SetNickname(reminder->strTemplate, 0, Pokemon_GetBoxPokemon(reminder->data->mon));
        break;
    case FMTSTR_TEACH_THE_MOVE:
        StringTemplate_SetMoveName(reminder->strTemplate, 1, MoveReminder_SelectedMove(reminder));
        break;
    case FMTSTR_GIVE_UP_TEACHING_MOVE:
        StringTemplate_SetNickname(reminder->strTemplate, 0, Pokemon_GetBoxPokemon(reminder->data->mon));
        break;
    case FMTSTR_MON_LEARNED_MOVE:
        StringTemplate_SetNickname(reminder->strTemplate, 0, Pokemon_GetBoxPokemon(reminder->data->mon));
        StringTemplate_SetMoveName(reminder->strTemplate, 1, MoveReminder_SelectedMove(reminder));
        break;
    case FMTSTR_MON_TRYING_TO_LEARN_MOVE:
        StringTemplate_SetNickname(reminder->strTemplate, 0, Pokemon_GetBoxPokemon(reminder->data->mon));
        StringTemplate_SetMoveName(reminder->strTemplate, 1, MoveReminder_SelectedMove(reminder));
        break;
    case FMTSTR_MON_FORGOT_MOVE:
        StringTemplate_SetNickname(reminder->strTemplate, 0, Pokemon_GetBoxPokemon(reminder->data->mon));
        StringTemplate_SetMoveName(reminder->strTemplate, 1, MoveReminder_MoveIndex(reminder));
        break;
    case FMTSTR_MON_LEARNED_MOVE2:
        StringTemplate_SetNickname(reminder->strTemplate, 0, Pokemon_GetBoxPokemon(reminder->data->mon));
        StringTemplate_SetMoveName(reminder->strTemplate, 1, MoveReminder_SelectedMove(reminder));
        break;
    case FMTSTR_MON_STOP_TEACHING_MOVE:
        StringTemplate_SetMoveName(reminder->strTemplate, 1, MoveReminder_SelectedMove(reminder));
        break;
    case FMTSTR_MON_DID_NOT_LEARN_MOVE:
        StringTemplate_SetNickname(reminder->strTemplate, 0, Pokemon_GetBoxPokemon(reminder->data->mon));
        StringTemplate_SetMoveName(reminder->strTemplate, 1, MoveReminder_SelectedMove(reminder));
        break;
    case FMTSTR_PLAYER_HANDED_OVER_ITEM:
        StringTemplate_SetPlayerName(reminder->strTemplate, 2, reminder->data->trainerInfo);
        break;
    case FMTSTR_MAKE_MON_FORGOT_MOVE:
        StringTemplate_SetMoveName(reminder->strTemplate, 0, MoveReminder_MoveIndex(reminder));
        break;
    }

    Strbuf *fmtString = MessageLoader_GetNewStrbuf(reminder->msgLoader, sMoveReminder_BankEntryIDs[reminder->data->unk_15][fmt]);
    StringTemplate_Format(reminder->strTemplate, reminder->strbuf, fmtString);
    Strbuf_Free(fmtString);
}

static u16 MoveReminder_SelectedMove(MoveReminder *reminder)
{
    return reminder->data->movesList[reminder->data->listPos + reminder->data->cursorPos];
}

static u16 MoveReminder_MoveIndex(MoveReminder *reminder)
{
    return Pokemon_GetValue(reminder->data->mon, MON_DATA_MOVE1 + reminder->data->moveSlot, NULL);
}

static void MoveReminder_FormatAndPrintStrbuf(MoveReminder *reminder, enum FormatStrings fmt)
{
    Window_FillTilemap(&reminder->window[MOVE_REMINDER_WINDOW_12], 15);
    MoveReminder_FormatStrbuf(reminder, fmt);
    RenderControlFlags_SetCanABSpeedUpPrint(TRUE);
    RenderControlFlags_SetAutoScrollFlags(FALSE);

    reminder->txtPrinterID = Text_AddPrinterWithParams(&reminder->window[MOVE_REMINDER_WINDOW_12], FONT_MESSAGE, reminder->strbuf, 0, 0, Options_TextFrameDelay(reminder->data->options), MoveReminder_TextPrinterCallback);
}

static BOOL MoveReminder_TextPrinterCallback(TextPrinterTemplate *template, u16 param)
{
    switch (param) {
    case 1:
        return Sound_IsAnyEffectPlaying();
    case 2:
        return Sound_IsBGMPausedByFanfare();
    case 3:
        Sound_PlayEffect(SEQ_SE_DP_KON);
        break;
    case 4:
        Sound_PlayFanfare(SEQ_FANFA1);
        break;
    case 5:
        return Sound_IsEffectPlaying(SEQ_SE_DP_KON);
    }

    return FALSE;
}

static u8 MoveReminder_CalculateTotalMoves(MoveReminder *reminder)
{
    u8 i;

    for (i = 0; i < LEARNED_MOVES_MAX; i++) {
        if (Pokemon_GetValue(reminder->data->mon, MON_DATA_MOVE1 + i, NULL) == MOVE_NONE) {
            break;
        }
    }

    return i;
}

static int MoveReminder_PrintMonLearnedMove(MoveReminder *reminder)
{
    MoveReminder_FormatAndPrintStrbuf(reminder, FMTSTR_MON_LEARNED_MOVE);

    reminder->data->moveSlot = MoveReminder_CalculateTotalMoves(reminder);
    reminder->state = 5;

    return 2;
}

static int ov91_021D1EE8(MoveReminder *reminder)
{
    Window_FillTilemap(&reminder->window[MOVE_REMINDER_WINDOW_12], 15);

    MoveReminder_FormatStrbuf(reminder, FMTSTR_TEACH_WHICH_MOVE);
    MoveReminder_PrintStrbuf(reminder, MOVE_REMINDER_WINDOW_12, FONT_MESSAGE, TEXT_COLOR(1, 2, 15), ALIGN_MODE_LEFT);

    Window_ScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_12]);

    MoveReminder_UpdateCursor(reminder, reminder->data->cursorPos, 0);
    ov91_021D2574(reminder, TRUE);

    return TRUE;
}

static int ov91_021D1F38(MoveReminder *reminder)
{
    reminder->data->unk_16 = TRUE;
    return 8;
}

static int ov91_021D1F44(MoveReminder *reminder)
{
    Window_FillTilemap(&reminder->window[MOVE_REMINDER_WINDOW_12], 15);

    MoveReminder_FormatStrbuf(reminder, FMTSTR_TEACH_WHICH_MOVE);
    MoveReminder_PrintStrbuf(reminder, MOVE_REMINDER_WINDOW_12, FONT_MESSAGE, TEXT_COLOR(1, 2, 15), ALIGN_MODE_LEFT);

    Window_ScheduleCopyToVRAM(&reminder->window[MOVE_REMINDER_WINDOW_12]);

    MoveReminder_UpdateCursor(reminder, reminder->data->cursorPos, 0);
    ov91_021D2574(reminder, TRUE);

    return TRUE;
}

static int ov91_021D1F94(MoveReminder *reminder)
{
    reminder->state = 10;
    return FALSE;
}

static int ov91_021D1FA0(MoveReminder *reminder)
{
    MoveReminder_FormatAndPrintStrbuf(reminder, FMTSTR_MON_STOP_TEACHING_MOVE);
    reminder->unk_186 = 3;
    reminder->state = 3;
    return 2;
}

static int ov91_021D1FBC(MoveReminder *reminder)
{
    MoveReminder_FormatAndPrintStrbuf(reminder, FMTSTR_MON_DID_NOT_LEARN_MOVE);
    reminder->state = 8;
    reminder->data->unk_16 = TRUE;
    return 2;
}

static int ov91_021D1FD8(MoveReminder *reminder)
{
    MoveReminder_FormatAndPrintStrbuf(reminder, FMTSTR_MON_TRYING_TO_LEARN_MOVE);
    reminder->unk_186 = 2;
    reminder->state = 3;
    return 2;
}

static int ov91_021D1FF4(MoveReminder *reminder)
{
    MoveReminder_FormatAndPrintStrbuf(reminder, FMTSTR_MON_FORGOT_MOVE);
    reminder->state = 6;
    return 2;
}

static int ov91_021D2008(MoveReminder *reminder)
{
    reminder->state = 10;
    return FALSE;
}

static void ov91_021D2014(MoveReminder *reminder)
{
    SpriteResourceCapacities capacities = {
        11, 2, 4, 4, 0, 0
    };

    VramTransfer_New(64, HEAP_ID_MOVE_REMINDER);

    reminder->spriteSys = SpriteSystem_Alloc(HEAP_ID_MOVE_REMINDER);
    reminder->spriteMan = SpriteManager_New(reminder->spriteSys);

    RenderOamTemplate oamTemplate = {
        .mainOamStart = 0,
        .mainOamCount = 128,
        .mainAffineOamStart = 0,
        .mainAffineOamCount = 32,
        .subOamStart = 0,
        .subOamCount = 128,
        .subAffineOamStart = 0,
        .subAffineOamCount = 32,
    };
    CharTransferTemplateWithModes transferTemplate = {
        .maxTasks = 13,
        .sizeMain = 1024 * 128,
        .sizeSub = 1024 * 16,
        .modeMain = GX_OBJVRAMMODE_CHAR_1D_32K,
        .modeSub = GX_OBJVRAMMODE_CHAR_1D_32K
    };

    SpriteSystem_Init(reminder->spriteSys, &oamTemplate, &transferTemplate, 32);
    SpriteSystem_InitSprites(reminder->spriteSys, reminder->spriteMan, 13);
    SpriteSystem_InitManagerWithCapacities(reminder->spriteSys, reminder->spriteMan, &capacities);
    GXLayers_EngineAToggleLayers(GX_PLANEMASK_OBJ, TRUE);
}

static void MoveReminder_FreeSprites(MoveReminder *reminder)
{
    for (u32 i = MOVE_REMINDER_SPRITE_0; i < MOVE_REMINDER_SPRITE_MAX; i++) {
        Sprite_DeleteAndFreeResources(reminder->managedSprite[i]);
    }

    SpriteSystem_FreeResourcesAndManager(reminder->spriteSys, reminder->spriteMan);
    SpriteSystem_Free(reminder->spriteSys);
}

static void MoveReminder_UpdateSpritesAnim(MoveReminder *reminder)
{
    for (u16 i = MOVE_REMINDER_SPRITE_0; i < MOVE_REMINDER_SPRITE_MAX; i++) {
        Sprite_UpdateAnim(reminder->managedSprite[i]->sprite, FX32_ONE);
    }
}

static void MoveReminder_LoadSpriteGraphics(MoveReminder *reminder, NARC *narc)
{
    SpriteSystem_LoadCharResObjFromOpenNarc(reminder->spriteSys, reminder->spriteMan, narc, 5, FALSE, NNS_G2D_VRAM_TYPE_2DMAIN, 47272);
    SpriteSystem_LoadCharResObjFromOpenNarc(reminder->spriteSys, reminder->spriteMan, narc, 8, FALSE, NNS_G2D_VRAM_TYPE_2DMAIN, 47273);
    SpriteSystem_LoadCharResObjFromOpenNarc(reminder->spriteSys, reminder->spriteMan, narc, 2, FALSE, NNS_G2D_VRAM_TYPE_2DMAIN, 47274);

    for (u32 i = 47276; i <= 47282; i++) {
        sub_0207C948(reminder->spriteSys, reminder->spriteMan, NNS_G2D_VRAM_TYPE_2DMAIN, 0, i);
    }

    sub_0207CAC4(reminder->spriteSys, reminder->spriteMan, NNS_G2D_VRAM_TYPE_2DMAIN, 0, 47275);
    SpriteSystem_LoadPlttResObjFromOpenNarc(reminder->spriteSys, reminder->spriteMan, narc, 9, FALSE, 2, NNS_G2D_VRAM_TYPE_2DMAIN, 47272);
    sub_0207C97C(reminder->spriteSys, reminder->spriteMan, NNS_G2D_VRAM_TYPE_2DMAIN, 47273);
    SpriteSystem_LoadCellResObjFromOpenNarc(reminder->spriteSys, reminder->spriteMan, narc, 4, FALSE, 47272);
    SpriteSystem_LoadCellResObjFromOpenNarc(reminder->spriteSys, reminder->spriteMan, narc, 7, FALSE, 47273);
    SpriteSystem_LoadCellResObjFromOpenNarc(reminder->spriteSys, reminder->spriteMan, narc, 1, FALSE, 47274);
    SpriteSystem_LoadAnimResObjFromOpenNarc(reminder->spriteSys, reminder->spriteMan, narc, 3, FALSE, 47272);
    SpriteSystem_LoadAnimResObjFromOpenNarc(reminder->spriteSys, reminder->spriteMan, narc, 6, FALSE, 47273);
    SpriteSystem_LoadAnimResObjFromOpenNarc(reminder->spriteSys, reminder->spriteMan, narc, 0, FALSE, 47274);
    sub_0207C9EC(reminder->spriteSys, reminder->spriteMan, 47275, 47275);
}

static void ov91_021D22A0(MoveReminder *reminder, u16 move, u16 idx)
{
    u32 moveType;

    if (reminder->data->unk_14 == 0) {
        moveType = MoveTable_LoadParam(move, MOVEATTRIBUTE_TYPE);
    } else {
        moveType = MoveTable_LoadParam(move, MOVEATTRIBUTE_CONTEST_TYPE) + 18;
    }

    SpriteSystem_ReplaceCharResObj(reminder->spriteSys, reminder->spriteMan, sub_0207C944(), sub_0207C908(moveType), TRUE, 47276 + idx);
    ManagedSprite_SetExplicitPalette(reminder->managedSprite[MOVE_REMINDER_SPRITE_6 + idx], sub_0207C92C(moveType) + 2);
}

static void MoveReminder_UpdateTypeSprites(MoveReminder *reminder)
{
    for (u32 i = 0; i < MAX_ITEM_SHOWN; i++) {
        ManagedSprite_SetPositionXY(reminder->managedSprite[MOVE_REMINDER_SPRITE_6 + i], 152, 32 + 16 * i);

        if (reminder->data->listPos + i >= reminder->movesCount - 1) {
            ManagedSprite_SetDrawFlag(reminder->managedSprite[MOVE_REMINDER_SPRITE_6 + i], FALSE);
        } else {
            ManagedSprite_SetDrawFlag(reminder->managedSprite[MOVE_REMINDER_SPRITE_6 + i], TRUE);
            ov91_021D22A0(reminder, reminder->data->movesList[reminder->data->listPos + i], i);
        }
    }
}

static void MoveReminder_MoveTypeSprites(MoveReminder *reminder, u16 prevPos, u16 currPos)
{
    u32 i;
    s16 x, y;

    if (prevPos < currPos) {
        for (i = 0; i < MAX_ITEM_SHOWN; i++) {
            ManagedSprite_GetPositionXY(reminder->managedSprite[MOVE_REMINDER_SPRITE_6 + i], &x, &y);

            if (y == 32) {
                y = 32 + (16 * 6);

                if (reminder->data->movesList[currPos + 6] != (u16)-1) {
                    ov91_021D22A0(reminder, reminder->data->movesList[currPos + 6], i);
                }
            } else {
                y -= 16;
            }

            ManagedSprite_SetPositionXY(reminder->managedSprite[MOVE_REMINDER_SPRITE_6 + i], x, y);
        }
    } else if (prevPos > currPos) {
        for (i = 0; i < MAX_ITEM_SHOWN; i++) {
            ManagedSprite_GetPositionXY(reminder->managedSprite[MOVE_REMINDER_SPRITE_6 + i], &x, &y);

            if (y == (32 + (16 * 6))) {
                y = 32;

                if (reminder->data->movesList[currPos] != (u16)-1) {
                    ov91_021D22A0(reminder, reminder->data->movesList[currPos], i);
                }
            } else {
                y += 16;
            }

            ManagedSprite_SetPositionXY(reminder->managedSprite[MOVE_REMINDER_SPRITE_6 + i], x, y);
        }
    } else {
        return;
    }

    for (i = 0; i < MAX_ITEM_SHOWN; i++) {
        ManagedSprite_GetPositionXY(reminder->managedSprite[MOVE_REMINDER_SPRITE_6 + i], &x, &y);
        y = (y - 32) / 16;

        if (currPos + y >= reminder->movesCount - 1) {
            ManagedSprite_SetDrawFlag(reminder->managedSprite[MOVE_REMINDER_SPRITE_6 + i], FALSE);
        } else {
            ManagedSprite_SetDrawFlag(reminder->managedSprite[MOVE_REMINDER_SPRITE_6 + i], TRUE);
        }
    }
}

static void MoveReminder_UpdateClassSprite(MoveReminder *reminder, u16 move)
{
    u32 moveClass = MoveTable_LoadParam(move, MOVEATTRIBUTE_CLASS);

    SpriteSystem_ReplaceCharResObj(reminder->spriteSys, reminder->spriteMan, sub_0207CAC0(), sub_0207CA90(moveClass), TRUE, 47275);
    ManagedSprite_SetExplicitPalette(reminder->managedSprite[MOVE_REMINDER_SPRITE_5], sub_0207CAA8(moveClass) + 2);
}

static void MoveReminder_LoadSprites(MoveReminder *reminder, NARC *narc)
{
    ov91_021D2014(reminder);
    MoveReminder_LoadSpriteGraphics(reminder, narc);

    for (u32 i = MOVE_REMINDER_SPRITE_0; i < MOVE_REMINDER_SPRITE_MAX; i++) {
        reminder->managedSprite[i] = SpriteSystem_NewSprite(reminder->spriteSys, reminder->spriteMan, &sMoveReminder_SpriteTemplates[i]);
    }
}

static void MoveReminder_UpdateCursor(MoveReminder *reminder, u8 index, u8 palette)
{
    ManagedSprite_SetPositionXY(reminder->managedSprite[MOVE_REMINDER_SPRITE_CURSOR], 196, 32 + 16 * index);
    ManagedSprite_SetExplicitPalette(reminder->managedSprite[MOVE_REMINDER_SPRITE_CURSOR], palette);
}

static void ov91_021D2574(MoveReminder *reminder, u8 draw)
{
    ManagedSprite_SetDrawFlag(reminder->managedSprite[MOVE_REMINDER_SPRITE_0], draw);
    ManagedSprite_SetDrawFlag(reminder->managedSprite[MOVE_REMINDER_SPRITE_1], draw);
}

static void ov91_021D2594(MoveReminder *reminder)
{
    if (reminder->data->listPos != 0) {
        ManagedSprite_SetDrawFlag(reminder->managedSprite[MOVE_REMINDER_SPRITE_2], TRUE);
    } else {
        ManagedSprite_SetDrawFlag(reminder->managedSprite[MOVE_REMINDER_SPRITE_2], FALSE);
    }

    if (reminder->data->listPos + 7 < reminder->movesCount) {
        ManagedSprite_SetDrawFlag(reminder->managedSprite[MOVE_REMINDER_SPRITE_3], TRUE);
    } else {
        ManagedSprite_SetDrawFlag(reminder->managedSprite[MOVE_REMINDER_SPRITE_3], FALSE);
    }
}

static void ov91_021D25E4(MoveReminder *reminder)
{
    ManagedSprite_SetDrawFlag(reminder->managedSprite[MOVE_REMINDER_SPRITE_2], FALSE);
    ManagedSprite_SetDrawFlag(reminder->managedSprite[MOVE_REMINDER_SPRITE_3], FALSE);
}

static int MoveReminder_ScreenTransitionIn(MoveReminder *reminder)
{
    sub_0208C120(1, HEAP_ID_MOVE_REMINDER);
    reminder->state = 11;
    return FALSE;
}

static int MoveReminder_OpenSummaryScreenApp(MoveReminder *reminder)
{
    u8 pages[] = {
        SUMMARY_PAGE_BATTLE_MOVES,
        SUMMARY_PAGE_CONTEST_MOVES,
        SUMMARY_PAGE_MAX
    };

    MoveReminder_Free(reminder);

    reminder->summaryScreen.monData = reminder->data->mon;
    reminder->summaryScreen.options = reminder->data->options;
    reminder->summaryScreen.dataType = SUMMARY_DATA_MON;
    reminder->summaryScreen.monIndex = 0;
    reminder->summaryScreen.monMax = 1;
    reminder->summaryScreen.move = MoveReminder_SelectedMove(reminder);
    reminder->summaryScreen.mode = SUMMARY_MODE_SELECT_MOVE;
    reminder->summaryScreen.showContest = TRUE;
    reminder->summaryScreen.chatotCry = NULL;

    PokemonSummaryScreen_FlagVisiblePages(&reminder->summaryScreen, pages);

    reminder->overlayMan = OverlayManager_New(&gPokemonSummaryScreenApp, &reminder->summaryScreen, HEAP_ID_MOVE_REMINDER);
    return 12;
}

static int MoveReminder_ReloadApp(MoveReminder *reminder)
{
    if (OverlayManager_Exec(reminder->overlayMan)) {
        OverlayManager_Free(reminder->overlayMan);
        MoveReminder_LoadGraphics(reminder);
        MoveReminder_UpdateCursor(reminder, reminder->data->cursorPos, 1);
        ov91_021D2574(reminder, FALSE);
        ov91_021D25E4(reminder);
        reminder->data->moveSlot = reminder->summaryScreen.selectedMoveSlot;
        reminder->state = 7;
        return FALSE;
    }

    return 12;
}
