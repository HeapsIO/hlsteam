#include "steamwrap.h"

HL_PRIM void HL_NAME(set_timeline_tooltip)( vbyte* description, float timeDelta ) {
	SteamTimeline()->SetTimelineTooltip((char*)description, timeDelta);
}

HL_PRIM void HL_NAME(clear_timeline_tooltip)( float timeDelta ) {
	SteamTimeline()->ClearTimelineTooltip(timeDelta);
}

HL_PRIM void HL_NAME(set_timeline_game_mode)( ETimelineGameMode mode ) {
	SteamTimeline()->SetTimelineGameMode(mode);
}

HL_PRIM TimelineEventHandle_t HL_NAME(add_instantaneous_timeline_event)( vbyte* title, vbyte* description, vbyte* icon, int priority, float startOffsetSeconds, ETimelineEventClipPriority possibleClip ) {
	return SteamTimeline()->AddInstantaneousTimelineEvent((char*)title, (char*)description, (char*)icon, priority, startOffsetSeconds, possibleClip);
}

HL_PRIM TimelineEventHandle_t HL_NAME(add_range_timeline_event)( vbyte* title, vbyte* description, vbyte* icon, int priority, float startOffsetSeconds, float durationSeconds, ETimelineEventClipPriority possibleClip ) {
	return SteamTimeline()->AddRangeTimelineEvent((char*)title, (char*)description, (char*)icon, priority, startOffsetSeconds, durationSeconds, possibleClip);
}

HL_PRIM TimelineEventHandle_t HL_NAME(start_range_timeline_event)( vbyte* title, vbyte* description, vbyte* icon, int priority, float startOffsetSeconds, ETimelineEventClipPriority possibleClip ) {
	return SteamTimeline()->StartRangeTimelineEvent((char*)title, (char*)description, (char*)icon, priority, startOffsetSeconds, possibleClip);
}

HL_PRIM void HL_NAME(update_range_timeline_event)( TimelineEventHandle_t event, vbyte* title, vbyte* description, vbyte* icon, int priority, ETimelineEventClipPriority possibleClip ) {
	SteamTimeline()->UpdateRangeTimelineEvent(event, (char*)title, (char*)description, (char*)icon, priority, possibleClip);
}

HL_PRIM void HL_NAME(end_range_timeline_event)( TimelineEventHandle_t event, float endOffsetSeconds ) {
	SteamTimeline()->EndRangeTimelineEvent(event, endOffsetSeconds);
}

HL_PRIM void HL_NAME(remove_timeline_event)( TimelineEventHandle_t event ) {
	SteamTimeline()->RemoveTimelineEvent(event);
}

#define _TIMELINEEVENT _I64
DEFINE_PRIM(_VOID, set_timeline_tooltip, _BYTES _F32);
DEFINE_PRIM(_VOID, clear_timeline_tooltip, _F32);
DEFINE_PRIM(_VOID, set_timeline_game_mode, _I32);
DEFINE_PRIM(_TIMELINEEVENT, add_instantaneous_timeline_event, _BYTES _BYTES _BYTES _I32 _F32 _I32);
DEFINE_PRIM(_TIMELINEEVENT, add_range_timeline_event, _BYTES _BYTES _BYTES _I32 _F32 _F32 _I32);
DEFINE_PRIM(_TIMELINEEVENT, start_range_timeline_event, _BYTES _BYTES _BYTES _I32 _F32 _I32);
DEFINE_PRIM(_VOID, update_range_timeline_event, _TIMELINEEVENT _BYTES _BYTES _BYTES _I32 _I32);
DEFINE_PRIM(_VOID, end_range_timeline_event, _TIMELINEEVENT _F32);
DEFINE_PRIM(_VOID, remove_timeline_event, _TIMELINEEVENT);
