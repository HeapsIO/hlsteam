#include "steamwrap.h"

HL_PRIM void HL_NAME(set_timeline_state_description)(vbyte* description, float timeDelta) {
    ISteamTimeline* timeline = SteamTimeline();
    if (timeline) {
        timeline->SetTimelineStateDescription((char*)description, timeDelta);
    }
}

HL_PRIM void HL_NAME(clear_timeline_state_description)(float timeDelta) {
    ISteamTimeline* timeline = SteamTimeline();
    if (timeline) {
        timeline->ClearTimelineStateDescription(timeDelta);
    }
}

HL_PRIM void HL_NAME(add_timeline_event)(vbyte* icon, vbyte* title, vbyte* description, int priority, float startOffsetSeconds, float durationSeconds, int possibleClip) {
    ISteamTimeline* timeline = SteamTimeline();
    if (timeline) {
        timeline->AddTimelineEvent((char*)icon, (char*)title, (char*)description, priority, startOffsetSeconds, durationSeconds, (ETimelineEventClipPriority)possibleClip);
    }
}

HL_PRIM void HL_NAME(set_timeline_game_mode)(int mode) {
    ISteamTimeline* timeline = SteamTimeline();
    if (timeline) {
        timeline->SetTimelineGameMode((ETimelineGameMode)mode);
    }
}

DEFINE_PRIM(_VOID, set_timeline_state_description, _BYTES _F32);
DEFINE_PRIM(_VOID, clear_timeline_state_description, _F32);
DEFINE_PRIM(_VOID, add_timeline_event, _BYTES _BYTES _BYTES _I32 _F32 _F32 _I32);
DEFINE_PRIM(_VOID, set_timeline_game_mode, _I32);