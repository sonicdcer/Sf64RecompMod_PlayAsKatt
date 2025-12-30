#include "modding.h"
#include "recomputils.h"
#include "recompconfig.h"

#include "context.h"
#include "sfx.h"

extern Vec3f D_display_801613E0[4];
extern bool sDrawCockpit;
extern Gfx aAwCockpitViewDL[];
extern Gfx D_versus_300EE80[];
extern Gfx D_versus_3010A90[];
extern Gfx D_versus_3011470[];
extern Gfx D_versus_300D550[];
extern s32 D_display_800CA22C;
extern s16 gReflectY;
extern Gfx aKattShipDL_data[];
extern Gfx aBallDL[];
extern f32 sPlayerShadowing;
extern Gfx aArwingShadowDL[];
extern Gfx aLandmasterShadowDL[];
extern Gfx aLandmasterShadowDL_data[];
extern Gfx D_versus_301E570[];
extern s32 sPlayWingSplashSfx;

void Display_Arwing_Skel(ArwingInfo *arwing);
void Effect_WaterSpray_Spawn(f32 xPos, f32 yPos, f32 zPos, f32 scale2, f32 scale1, f32 yRot);

RECOMP_PATCH void Display_Arwing(Player* player, s32 reflectY) {
    Vec3f sp4C;
    f32 sp48;

    if ((gPlayerNum == player->num) && (reflectY == 0)) {
        sp48 = 0.0f;
        if (gChargeTimers[player->num] >= 20) {
            sp48 = (s32) (gGameFrameCount % 8U) * 80.0f;
        }
        sp4C.x = 0.0f;
        sp4C.y = 0.0f;
        sp4C.z = 1200.0f;
        Matrix_MultVec3f(gGfxMatrix, &sp4C, &D_display_801613E0[0]);
        sp4C.z = 2400.0f + sp48;
        Matrix_MultVec3f(gGfxMatrix, &sp4C, &D_display_801613E0[1]);
    }

    if (player->alternateView && (gLevelMode == LEVELMODE_ON_RAILS) &&
        (fabsf(player->trueZpos + gPathProgress - player->cam.eye.z) < 10.0f)) {
        if (reflectY == 0) {
            sDrawCockpit = true;
            Matrix_Push(&gGfxMatrix);
            Matrix_Translate(gGfxMatrix, 0.0f, -8.0f, 5.0f, MTXF_APPLY);
            Matrix_RotateY(gGfxMatrix, M_PI, MTXF_APPLY);
            Matrix_SetGfxMtx(&gMasterDisp);
            gSPDisplayList(gMasterDisp++, aAwCockpitViewDL);
            Matrix_Pop(&gGfxMatrix);
        }
    } else {
        if (gVersusMode) {
            if ((player->arwing.rightWingState == WINGSTATE_INTACT) &&
                (player->arwing.leftWingState == WINGSTATE_INTACT)) {
                gSPDisplayList(gMasterDisp++, D_versus_300EE80);
            } else if ((player->arwing.rightWingState <= WINGSTATE_BROKEN) &&
                       (player->arwing.leftWingState == WINGSTATE_INTACT)) {
                gSPDisplayList(gMasterDisp++, D_versus_3010A90);
            } else if ((player->arwing.rightWingState == WINGSTATE_INTACT) &&
                       (player->arwing.leftWingState <= WINGSTATE_BROKEN)) {
                gSPDisplayList(gMasterDisp++, D_versus_3011470);
            } else {
                gSPDisplayList(gMasterDisp++, D_versus_300D550);
            }
        } else {
            if ((gLevelType == LEVELTYPE_SPACE) || (gCurrentLevel == LEVEL_BOLSE)) {
                player->arwing.unk_28 = player->arwing.upperRightFlapYrot = player->arwing.bottomRightFlapYrot =
                    player->arwing.upperLeftFlapYrot = player->arwing.bottomLeftFlapYrot = 0.0f;
            }
            D_display_800CA22C = true;
            gReflectY = reflectY;
            //Display_Arwing_Skel(&player->arwing);
            gSPDisplayList(gMasterDisp++, aKattShipDL_data);
        }
    }
}

RECOMP_FORCE_PATCH void Display_ArwingWingTrail_Draw(Player* player) {
    f32 sp5C = 40.0f;
    f32 sp58 = -28.0f;
    f32 sp54;
    f32 sp50;
    f32 yRot;

    if (0 /* player->wingPosition == 2 */ ) {
        sp5C = 108.0f;
        sp58 = -22.0f;
    }

    if (player->contrailScale != 0.0f) {
        sp54 = 0.0f;
        if ((gGameFrameCount % 2) != 0) {
            sp54 = 180.0f;
        }

        yRot = player->rot.y;
        if (yRot > 90.0f) {
            yRot -= 180.0f;
        }

        yRot *= 0.25f;
        sp50 = player->rot.x * 0.25f;

        if (player->state == PLAYERSTATE_LEVEL_COMPLETE) {
            yRot = 0.0f;
            sp50 = 0.0f;
        }

        RCP_SetupDL_64();
        gDPSetPrimColor(gMasterDisp++, 0x00, 0x00, 255, 255, 255, 100);

        // @recomp Tag the transform.
        gEXMatrixGroupDecomposedNormal(gMasterDisp++, 0x300, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_ALLOW);

        if (1 /* player->arwing.leftWingState == WINGSTATE_INTACT*/ ) {
            Matrix_Push(&gGfxMatrix);
            Matrix_Translate(gGfxMatrix, sp5C, sp58, -100.0f, MTXF_APPLY);
            Matrix_RotateX(gGfxMatrix, M_DTOR * sp50, MTXF_APPLY);
            Matrix_RotateY(gGfxMatrix, -(M_DTOR * yRot), MTXF_APPLY);
            Matrix_Scale(gGfxMatrix, player->contrailScale, 1.0f, 50.0f, MTXF_APPLY);
            Matrix_Translate(gGfxMatrix, 0.0f, 0.0f, -17.5f, MTXF_APPLY);
            Matrix_RotateX(gGfxMatrix, M_PI / 2, MTXF_APPLY);
            Matrix_RotateY(gGfxMatrix, M_DTOR * sp54, MTXF_APPLY);
            Matrix_SetGfxMtx(&gMasterDisp);
            gSPDisplayList(gMasterDisp++, aBallDL);
            Matrix_Pop(&gGfxMatrix);
        }

        // @recomp Pop the transform id.
        gEXPopMatrixGroup(gMasterDisp++, G_MTX_MODELVIEW);

        // @recomp Tag the transform.
        gEXMatrixGroupDecomposedNormal(gMasterDisp++, 0x300 + 1, G_EX_PUSH, G_MTX_MODELVIEW,
                                       G_EX_EDIT_ALLOW);

        if (1 /* player->arwing.rightWingState == WINGSTATE_INTACT */ ) {
            Matrix_Push(&gGfxMatrix);
            Matrix_Translate(gGfxMatrix, -sp5C, sp58, -100.0f, MTXF_APPLY);
            Matrix_RotateX(gGfxMatrix, M_DTOR * sp50, MTXF_APPLY);
            Matrix_RotateY(gGfxMatrix, -(M_DTOR * yRot), MTXF_APPLY);
            Matrix_Scale(gGfxMatrix, player->contrailScale, 1.0f, 50.0f, MTXF_APPLY);
            Matrix_Translate(gGfxMatrix, 0.0f, 0.0f, -17.5f, MTXF_APPLY);
            Matrix_RotateX(gGfxMatrix, M_PI / 2, MTXF_APPLY);
            Matrix_RotateY(gGfxMatrix, M_DTOR * sp54, MTXF_APPLY);
            Matrix_SetGfxMtx(&gMasterDisp);
            gSPDisplayList(gMasterDisp++, aBallDL);
            Matrix_Pop(&gGfxMatrix);
        }
        
        // @recomp Pop the transform id.
        gEXPopMatrixGroup(gMasterDisp++, G_MTX_MODELVIEW);

        // @recomp Tag the transform.
        gEXMatrixGroupDecomposedNormal(gMasterDisp++, 0x300+2, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_ALLOW);

        if (1 /* player->arwing.leftWingState == WINGSTATE_INTACT*/ ) {
            Matrix_Push(&gGfxMatrix);
            Matrix_Translate(gGfxMatrix, sp5C, -sp58, -100.0f, MTXF_APPLY);
            Matrix_RotateX(gGfxMatrix, M_DTOR * sp50, MTXF_APPLY);
            Matrix_RotateY(gGfxMatrix, -(M_DTOR * yRot), MTXF_APPLY);
            Matrix_Scale(gGfxMatrix, player->contrailScale, 1.0f, 50.0f, MTXF_APPLY);
            Matrix_Translate(gGfxMatrix, 0.0f, 0.0f, -17.5f, MTXF_APPLY);
            Matrix_RotateX(gGfxMatrix, M_PI / 2, MTXF_APPLY);
            Matrix_RotateY(gGfxMatrix, M_DTOR * sp54, MTXF_APPLY);
            Matrix_SetGfxMtx(&gMasterDisp);
            gSPDisplayList(gMasterDisp++, aBallDL);
            Matrix_Pop(&gGfxMatrix);
        }

        // @recomp Pop the transform id.
        gEXPopMatrixGroup(gMasterDisp++, G_MTX_MODELVIEW);

        // @recomp Tag the transform.
        gEXMatrixGroupDecomposedNormal(gMasterDisp++, 0x300 + 3, G_EX_PUSH, G_MTX_MODELVIEW,
                                       G_EX_EDIT_ALLOW);

        if (1 /* player->arwing.rightWingState == WINGSTATE_INTACT */ ) {
            Matrix_Push(&gGfxMatrix);
            Matrix_Translate(gGfxMatrix, -sp5C, -sp58, -100.0f, MTXF_APPLY);
            Matrix_RotateX(gGfxMatrix, M_DTOR * sp50, MTXF_APPLY);
            Matrix_RotateY(gGfxMatrix, -(M_DTOR * yRot), MTXF_APPLY);
            Matrix_Scale(gGfxMatrix, player->contrailScale, 1.0f, 50.0f, MTXF_APPLY);
            Matrix_Translate(gGfxMatrix, 0.0f, 0.0f, -17.5f, MTXF_APPLY);
            Matrix_RotateX(gGfxMatrix, M_PI / 2, MTXF_APPLY);
            Matrix_RotateY(gGfxMatrix, M_DTOR * sp54, MTXF_APPLY);
            Matrix_SetGfxMtx(&gMasterDisp);
            gSPDisplayList(gMasterDisp++, aBallDL);
            Matrix_Pop(&gGfxMatrix);
        }
        
        // @recomp Pop the transform id.
        gEXPopMatrixGroup(gMasterDisp++, G_MTX_MODELVIEW);
    }
}

RECOMP_FORCE_PATCH void Display_PlayerShadow_Draw(Player* player) {
    Math_SmoothStepToF(&sPlayerShadowing, player->shadowing, 1.0f, 10.0f, 0.0f);
    RCP_SetupDL(&gMasterDisp, SETUPDL_66);
    gDPSetPrimColor(gMasterDisp++, 0x00, 0x00, 0, 0, 0, (s32) sPlayerShadowing);

    if (player->groundPos.y > 30.0f) {
        gSPSetGeometryMode(gMasterDisp++, G_CULL_BACK);
    }

    if ((gGroundSurface == SURFACE_WATER) && (player->groundPos.y < 10.0f)) {
        player->shadowing = 90;
    } else {
        player->shadowing = 180;
    }

    // @recomp Tag the transform.
    gEXMatrixGroupSimple(gMasterDisp++, gPlayerNum + (0x3000), G_EX_PUSH, G_MTX_MODELVIEW, G_EX_COMPONENT_INTERPOLATE,
                         G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE,
                         G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_SKIP);

    switch (player->form) {
        case FORM_ARWING:
            Matrix_Scale(gGfxMatrix, 1.0f * 1.67f, 1.0f, 1.0f * 1.67f, MTXF_APPLY);
            Matrix_Translate(gGfxMatrix, 0.0f, 0.0f, 30.0f, MTXF_APPLY);
            Matrix_SetGfxMtx(&gMasterDisp);
            gSPDisplayList(gMasterDisp++, aLandmasterShadowDL_data); // the landmaster shadow looks closer to what katt ship looks like than the arwing
            break;

        case FORM_LANDMASTER:
            Matrix_Push(&gGfxMatrix);
            Matrix_Scale(gGfxMatrix, 1.17f, 1.17f, 1.17f, MTXF_APPLY);
            Matrix_Translate(gGfxMatrix, 0.0f, 0.0f, 20.0f, MTXF_APPLY);
            Matrix_SetGfxMtx(&gMasterDisp);
            if (gCurrentLevel == LEVEL_MACBETH) {
                RCP_SetupDL(&gMasterDisp, SETUPDL_65);
            }
            if (!gVersusMode) {
                gSPDisplayList(gMasterDisp++, aLandmasterShadowDL);
            } else {
                gSPDisplayList(gMasterDisp++, D_versus_301E570);
            }
            Matrix_Pop(&gGfxMatrix);
            break;

        case FORM_BLUE_MARINE:
            break;

        case FORM_ON_FOOT:
            Matrix_Push(&gGfxMatrix);
            Matrix_Scale(gGfxMatrix, 0.5f, 0.5f, 0.5f, MTXF_APPLY);
            Matrix_RotateX(gGfxMatrix, M_PI / 2, MTXF_APPLY);
            Matrix_SetGfxMtx(&gMasterDisp);
            gSPClearGeometryMode(gMasterDisp++, G_CULL_BACK);
            gSPDisplayList(gMasterDisp++, aBallDL);
            gSPSetGeometryMode(gMasterDisp++, G_CULL_BACK);
            Matrix_Pop(&gGfxMatrix);
            break;

        default:
            break;
    }
    // @recomp Pop the transform id.
    gEXPopMatrixGroup(gMasterDisp++, G_MTX_MODELVIEW);
}

RECOMP_PATCH void Player_WaterEffects(Player* player) {
    Vec3f waterSpray1SrcPos = { -45.0f, -22.0f, -45.0f };
    Vec3f waterSpray2SrcPos = { 45.0f, -22.0f, -45.0f };
    Vec3f waterSpray1Pos;
    Vec3f waterSpray2Pos;

    if (gGroundSurface == SURFACE_WATER) {
        Matrix_Translate(gCalcMatrix, player->pos.x, player->pos.y, player->trueZpos, MTXF_NEW);

        Matrix_RotateY(gCalcMatrix, (player->yRot_114 + player->rot.y + 180.0f) * M_DTOR, MTXF_APPLY);
        Matrix_RotateX(gCalcMatrix, -((player->rot.x + player->aerobaticPitch) * M_DTOR), MTXF_APPLY);
        Matrix_RotateZ(gCalcMatrix, -(player->bankAngle * M_DTOR), MTXF_APPLY);

        Matrix_MultVec3f(gCalcMatrix, &waterSpray1SrcPos, &waterSpray1Pos);
        Matrix_MultVec3f(gCalcMatrix, &waterSpray2SrcPos, &waterSpray2Pos);

        if (player->pos.y < (gGroundHeight + 100.0f)) {
            if ((waterSpray1Pos.y < gGroundHeight + 80.0f) && ((gGameFrameCount % 2) == 0)) {
                if (sPlayWingSplashSfx) {}
                Effect_WaterSpray_Spawn(waterSpray1Pos.x, gGroundHeight, waterSpray1Pos.z, 0.1f, 2.0f,
                                        player->rot.y + player->yRot_114 + 20.0f);
            }
            if ((waterSpray2Pos.y < gGroundHeight + 80.0f) && ((gGameFrameCount % 2) == 0)) {
                Effect_WaterSpray_Spawn(waterSpray2Pos.x, gGroundHeight, waterSpray2Pos.z, 0.1f, 2.0f,
                                        player->rot.y + player->yRot_114 - 20.0f);
            }
        }

        if ((waterSpray2Pos.y < gGroundHeight + 80.0f) || (waterSpray1Pos.y < gGroundHeight + 80.0f)) {
            if (!sPlayWingSplashSfx) {
                sPlayWingSplashSfx = true;
                AUDIO_PLAY_SFX(NA_SE_SPLASH_LEVEL_S, player->sfxSource, 0);
            }
        } else {
            sPlayWingSplashSfx = false;
            Audio_KillSfxBySourceAndId(player->sfxSource, NA_SE_SPLASH_LEVEL_S);
        }
    }
}