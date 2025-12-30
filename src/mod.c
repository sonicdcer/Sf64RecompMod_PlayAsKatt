#include "modding.h"
#include "recomputils.h"
#include "recompconfig.h"

#include "context.h"

RECOMP_HOOK_RETURN("Display_Update") void Display_Update(void) {
    if ((gGameState != GSTATE_PLAY) || (gPlayState <= PLAY_INIT)) {
        return;
    }
    gBombCount[0] = 9;
    gLaserStrength[0] = 2;
}