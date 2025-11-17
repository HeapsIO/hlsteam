package steam;

@:hlNative("steam")
class Timeline {
    public static function setTimelineStateDescription(description:String, timeDelta:hl.F32):Void {
        @:privateAccess _SetTimelineStateDescription(description.toUtf8(), timeDelta);
    }
    public static function clearTimelineStateDescription(timeDelta:hl.F32):Void {}
    public static function addTimelineEvent(icon:String, title:String, description:String, priority:Int, startOffsetSeconds:hl.F32, durationSeconds:hl.F32, possibleClip:Int):Void {
        @:privateAccess _AddTimelineEvent(icon.toUtf8(), title.toUtf8(), description.toUtf8(), priority, startOffsetSeconds, durationSeconds, possibleClip);
    }
    public static function setTimelineGameMode(mode:Int):Void {}
    

    @:hlNative("?steam", "set_timeline_state_description") private static function _SetTimelineStateDescription(description:hl.Bytes, timeDelta:hl.F32):Void {}
    @:hlNative("?steam", "add_timeline_event") private static function _AddTimelineEvent(icon:hl.Bytes, title:hl.Bytes, description:hl.Bytes, priority:Int, startOffsetSeconds:hl.F32, durationSeconds:hl.F32, possibleClip:Int):Void {}
}