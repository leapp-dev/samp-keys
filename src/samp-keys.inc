/*
 * ============================================================================
 *  samp-keys.inc
 * ============================================================================
 *  Description: Dynamic key pressing mini-game system for SA-MP
 *  Author: Leap
 *  Version: 1.0.0
 * ============================================================================
 */

#if defined _samp_keys_included
    #endinput
#endif
#define _samp_keys_included

#if !defined MAX_KEY_SEQUENCES
    #define MAX_KEY_SEQUENCES       (10)
#endif

#if !defined MAX_SEQUENCE_LENGTH
    #define MAX_SEQUENCE_LENGTH     (8)
#endif

#if !defined KEY_GAME_UPDATE_RATE
    #define KEY_GAME_UPDATE_RATE    (100)
#endif


enum E_KEY_GAME_DATA {
    bool:kgActive,
    kgSequenceCount,
    kgCurrentIndex,
    kgSequence[MAX_SEQUENCE_LENGTH],
    kgTimeLimit,
    kgStartTick,
    kgTimerID,
    Text:kgTextDraw[3]
}
static gs_aPlayerKeyGame[MAX_PLAYERS][E_KEY_GAME_DATA];


static const gs_aKeyNames[][] = {
    "N",
    "Y",
    "C",
    "F",
    "ALT",
    "H"
};

static const gs_aKeyValues[] = {
    KEY_NO,                // N
    KEY_YES,               // Y
    KEY_CROUCH,            // C
    KEY_SECONDARY_ATTACK,  // F
    KEY_WALK,              // ALT
    KEY_CTRL_BACK          // H
};


forward OnKeyGameFinish(playerid, bool:bSuccess);
forward OnKeyGameUpdate(playerid);

static stock _DestroyKeyGameTextDraws(const iPlayerID) {
    for(new i = 0; i < 3; i++) {
        if(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][i] != Text:INVALID_TEXT_DRAW) {
            TextDrawDestroy(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][i]);
            gs_aPlayerKeyGame[iPlayerID][kgTextDraw][i] = Text:INVALID_TEXT_DRAW;
        }
    }
}

static stock _CreateKeyGameTextDraws(const iPlayerID) {
    // bg
    gs_aPlayerKeyGame[iPlayerID][kgTextDraw][0] = TextDrawCreate(320.0, 150.0, "_");
    TextDrawAlignment(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][0], 2);
    TextDrawBackgroundColor(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][0], 255);
    TextDrawFont(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][0], 1);
    TextDrawLetterSize(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][0], 0.5, 15.0);
    TextDrawColor(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][0], -1);
    TextDrawSetOutline(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][0], 0);
    TextDrawSetProportional(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][0], 1);
    TextDrawSetShadow(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][0], 1);
    TextDrawUseBox(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][0], 1);
    TextDrawBoxColor(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][0], 0x00000088);
    TextDrawTextSize(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][0], 0.0, 400.0);
    
    // title
    gs_aPlayerKeyGame[iPlayerID][kgTextDraw][1] = TextDrawCreate(320.0, 140.0, "~y~PRESIONA LAS TECLAS");
    TextDrawAlignment(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][1], 2);
    TextDrawBackgroundColor(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][1], 255);
    TextDrawFont(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][1], 1);
    TextDrawLetterSize(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][1], 0.4, 1.6);
    TextDrawColor(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][1], -1);
    TextDrawSetOutline(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][1], 1);
    TextDrawSetProportional(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][1], 1);
    
    // secuence display
    gs_aPlayerKeyGame[iPlayerID][kgTextDraw][2] = TextDrawCreate(320.0, 180.0, "_");
    TextDrawAlignment(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][2], 2);
    TextDrawBackgroundColor(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][2], 255);
    TextDrawFont(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][2], 1);
    TextDrawLetterSize(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][2], 0.35, 1.4);
    TextDrawColor(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][2], -1);
    TextDrawSetOutline(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][2], 1);
    TextDrawSetProportional(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][2], 1);
}

static stock _UpdateSequenceDisplay(const iPlayerID) {
    new szDisplay[256];
    new szTemp[64];
    
    for(new i = 0; i < gs_aPlayerKeyGame[iPlayerID][kgSequenceCount]; i++) {
        if(i < gs_aPlayerKeyGame[iPlayerID][kgCurrentIndex]) {
            // pressed key
            format(szTemp, sizeof(szTemp), "~g~[%s]~w~ ", 
                gs_aKeyNames[gs_aPlayerKeyGame[iPlayerID][kgSequence][i]]);
        }
        else if(i == gs_aPlayerKeyGame[iPlayerID][kgCurrentIndex]) {
            // actual key
            format(szTemp, sizeof(szTemp), "~y~[%s]~w~ ", 
                gs_aKeyNames[gs_aPlayerKeyGame[iPlayerID][kgSequence][i]]);
        }
        else {
            // next key
            format(szTemp, sizeof(szTemp), "[%s] ", 
                gs_aKeyNames[gs_aPlayerKeyGame[iPlayerID][kgSequence][i]]);
        }
        strcat(szDisplay, szTemp);
    }
    
    new iTimeLeft = gs_aPlayerKeyGame[iPlayerID][kgTimeLimit] - 
        ((GetTickCount() - gs_aPlayerKeyGame[iPlayerID][kgStartTick]) / 1000);
    
    if(iTimeLeft < 0) iTimeLeft = 0;
    
    format(szTemp, sizeof(szTemp), "~n~~n~Tiempo: ~r~%d~w~s", iTimeLeft);
    strcat(szDisplay, szTemp);
    
    TextDrawSetString(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][2], szDisplay);
}

static stock _GenerateSequence(const iPlayerID, const iLength) {
    gs_aPlayerKeyGame[iPlayerID][kgSequenceCount] = iLength;
    
    for(new i = 0; i < iLength; i++) {
        gs_aPlayerKeyGame[iPlayerID][kgSequence][i] = random(sizeof(gs_aKeyNames));
    }
}

static stock _EndKeyGame(const iPlayerID, const bool:bSuccess) {
    if(!gs_aPlayerKeyGame[iPlayerID][kgActive]) return;
    
    gs_aPlayerKeyGame[iPlayerID][kgActive] = false;
    
    if(gs_aPlayerKeyGame[iPlayerID][kgTimerID] != -1) {
        KillTimer(gs_aPlayerKeyGame[iPlayerID][kgTimerID]);
        gs_aPlayerKeyGame[iPlayerID][kgTimerID] = -1;
    }
    
    for(new i = 0; i < 3; i++) {
        if(gs_aPlayerKeyGame[iPlayerID][kgTextDraw][i] != Text:INVALID_TEXT_DRAW) {
            TextDrawHideForPlayer(iPlayerID, gs_aPlayerKeyGame[iPlayerID][kgTextDraw][i]);
        }
    }
    
    _DestroyKeyGameTextDraws(iPlayerID);
    
    TogglePlayerControllable(iPlayerID, true);
    
    CallLocalFunction("OnKeyGameFinish", "ii", iPlayerID, bSuccess);
}

public OnKeyGameUpdate(playerid) {
    if(!gs_aPlayerKeyGame[playerid][kgActive]) return 0;
    
    new iElapsedTime = (GetTickCount() - gs_aPlayerKeyGame[playerid][kgStartTick]) / 1000;
    
    if(iElapsedTime >= gs_aPlayerKeyGame[playerid][kgTimeLimit]) {
        GameTextForPlayer(playerid, "~r~TIEMPO AGOTADO!", 2000, 3);
        _EndKeyGame(playerid, false);
        return 0;
    }
    
    _UpdateSequenceDisplay(playerid);
    return 1;
}

stock CreateKeyGame(const iPlayerID, const iTimeSeconds, const iSequenceLength = 4) {
    if(iPlayerID < 0 || iPlayerID >= MAX_PLAYERS) return 0;
    if(!IsPlayerConnected(iPlayerID)) return 0;
    if(gs_aPlayerKeyGame[iPlayerID][kgActive]) return 0;
    
    if(iSequenceLength < 1 || iSequenceLength > MAX_SEQUENCE_LENGTH) {
        printf("[samp-keys] Error: Longitud de secuencia invalida (%d). Rango: 1-%d", 
            iSequenceLength, MAX_SEQUENCE_LENGTH);
        return 0;
    }
    
    gs_aPlayerKeyGame[iPlayerID][kgActive] = true;
    gs_aPlayerKeyGame[iPlayerID][kgCurrentIndex] = 0;
    gs_aPlayerKeyGame[iPlayerID][kgTimeLimit] = iTimeSeconds;
    gs_aPlayerKeyGame[iPlayerID][kgStartTick] = GetTickCount();
    
    _GenerateSequence(iPlayerID, iSequenceLength);
    _CreateKeyGameTextDraws(iPlayerID);
    
    for(new i = 0; i < 3; i++) {
        TextDrawShowForPlayer(iPlayerID, gs_aPlayerKeyGame[iPlayerID][kgTextDraw][i]);
    }
    
    _UpdateSequenceDisplay(iPlayerID);
    
    TogglePlayerControllable(iPlayerID, false);
    
    gs_aPlayerKeyGame[iPlayerID][kgTimerID] = SetTimerEx("OnKeyGameUpdate", 
        KEY_GAME_UPDATE_RATE, true, "i", iPlayerID);
    
    return 1;
}

stock CancelKeyGame(const iPlayerID) {
    if(iPlayerID < 0 || iPlayerID >= MAX_PLAYERS) return 0;
    if(!gs_aPlayerKeyGame[iPlayerID][kgActive]) return 0;
    
    _EndKeyGame(iPlayerID, false);
    return 1;
}

stock IsPlayerInKeyGame(const iPlayerID) {
    if(iPlayerID < 0 || iPlayerID >= MAX_PLAYERS) return 0;
    return gs_aPlayerKeyGame[iPlayerID][kgActive];
}

public OnPlayerConnect(playerid) {
    gs_aPlayerKeyGame[playerid][kgActive] = false;
    gs_aPlayerKeyGame[playerid][kgTimerID] = -1;
    gs_aPlayerKeyGame[playerid][kgCurrentIndex] = 0;
    gs_aPlayerKeyGame[playerid][kgSequenceCount] = 0;
    
    for(new i = 0; i < 3; i++) {
        gs_aPlayerKeyGame[playerid][kgTextDraw][i] = Text:INVALID_TEXT_DRAW;
    }
    
    #if defined KEYS_OnPlayerConnect
        return KEYS_OnPlayerConnect(playerid);
    #else
        return 1;
    #endif
}

public OnPlayerDisconnect(playerid, reason) {
    if(gs_aPlayerKeyGame[playerid][kgActive]) {
        CancelKeyGame(playerid);
    }
    
    #if defined KEYS_OnPlayerDisconnect
        return KEYS_OnPlayerDisconnect(playerid, reason);
    #else
        return 1;
    #endif
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    if(!gs_aPlayerKeyGame[playerid][kgActive]) {
        #if defined KEYS_OnPlayerKeyStateChange
            return KEYS_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
        #else
            return 1;
        #endif
    }
    
    new iCurrentKey = gs_aPlayerKeyGame[playerid][kgSequence][gs_aPlayerKeyGame[playerid][kgCurrentIndex]];
    new iExpectedKey = gs_aKeyValues[iCurrentKey];
    
    if(newkeys & iExpectedKey) {
        PlayerPlaySound(playerid, 1083, 0.0, 0.0, 0.0);
        gs_aPlayerKeyGame[playerid][kgCurrentIndex]++;
        
        if(gs_aPlayerKeyGame[playerid][kgCurrentIndex] >= gs_aPlayerKeyGame[playerid][kgSequenceCount]) {
            GameTextForPlayer(playerid, "~g~COMPLETADO!", 2000, 3);
            _EndKeyGame(playerid, true);
            return 1;
        }
        
        _UpdateSequenceDisplay(playerid);
    }
    else {
        new bool:bWrongKey = false;
        
        for(new i = 0; i < sizeof(gs_aKeyValues); i++) {
            if(newkeys & gs_aKeyValues[i]) {
                bWrongKey = true;
                break;
            }
        }
        
        if(bWrongKey) {
            PlayerPlaySound(playerid, 1085, 0.0, 0.0, 0.0);
            GameTextForPlayer(playerid, "~r~TECLA INCORRECTA!", 2000, 3);
            _EndKeyGame(playerid, false);
            return 1;
        }
    }
    
    #if defined KEYS_OnPlayerKeyStateChange
        return KEYS_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
    #else
        return 1;
    #endif
}

#if defined _ALS_OnPlayerConnect
    #undef OnPlayerConnect
#else
    #define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect KEYS_OnPlayerConnect
#if defined KEYS_OnPlayerConnect
    forward KEYS_OnPlayerConnect(playerid);
#endif

#if defined _ALS_OnPlayerDisconnect
    #undef OnPlayerDisconnect
#else
    #define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect KEYS_OnPlayerDisconnect
#if defined KEYS_OnPlayerDisconnect
    forward KEYS_OnPlayerDisconnect(playerid, reason);
#endif

#if defined _ALS_OnPlayerKeyStateChange
    #undef OnPlayerKeyStateChange
#else
    #define _ALS_OnPlayerKeyStateChange
#endif
#define OnPlayerKeyStateChange KEYS_OnPlayerKeyStateChange
#if defined KEYS_OnPlayerKeyStateChange
    forward KEYS_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
#endif