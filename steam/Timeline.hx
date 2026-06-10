package steam;

enum abstract ETimelineGameMode(Int) from Int to Int {
	var Invalid = 0;
	var Playing = 1;
	var Staging = 2;
	var Menus = 3;
	var LoadingScreen = 4;
}

enum abstract ETimelineEventClipPriority(Int) from Int to Int {
	var Invalid = 0;
	var None = 1;
	var Standard = 2;
	var Featured = 3;
}

typedef TimelineEventHandle = hl.I64;

@:access(steam.TimelineNative)
class Timeline {
	public static function setTimelineTooltip(description:String, timeDelta:Float):Void {
		@:privateAccess TimelineNative.setTimelineTooltip(description.toUtf8(), timeDelta);
	}

	public static function clearTimelineTooltip(timeDelta:Float):Void {
		TimelineNative.clearTimelineTooltip(timeDelta);
	}

	public static function setTimelineGameMode(mode:ETimelineGameMode):Void {
		TimelineNative.setTimelineGameMode(mode);
	}

	public static function addInstantaneousTimelineEvent(title:String, description:String, icon:String, priority:Int, startOffsetSeconds:Float, possibleClip:ETimelineEventClipPriority = None):TimelineEventHandle {
		return @:privateAccess TimelineNative.addInstantaneousTimelineEvent(title.toUtf8(), description.toUtf8(), icon.toUtf8(), priority, startOffsetSeconds, possibleClip);
	}

	public static function addRangeTimelineEvent(title:String, description:String, icon:String, priority:Int, startOffsetSeconds:Float, durationSeconds:Float, possibleClip:ETimelineEventClipPriority = None):TimelineEventHandle {
		return @:privateAccess TimelineNative.addRangeTimelineEvent(title.toUtf8(), description.toUtf8(), icon.toUtf8(), priority, startOffsetSeconds, durationSeconds, possibleClip);
	}

	public static function startRangeTimelineEvent(title:String, description:String, icon:String, priority:Int, startOffsetSeconds:Float, possibleClip:ETimelineEventClipPriority = None):TimelineEventHandle {
		return @:privateAccess TimelineNative.startRangeTimelineEvent(title.toUtf8(), description.toUtf8(), icon.toUtf8(), priority, startOffsetSeconds, possibleClip);
	}

	public static function updateRangeTimelineEvent(event:TimelineEventHandle, title:String, description:String, icon:String, priority:Int, possibleClip:ETimelineEventClipPriority = None):Void {
		@:privateAccess TimelineNative.updateRangeTimelineEvent(event, title.toUtf8(), description.toUtf8(), icon.toUtf8(), priority, possibleClip);
	}

	public static function endRangeTimelineEvent(event:TimelineEventHandle, endOffsetSeconds:Float):Void {
		TimelineNative.endRangeTimelineEvent(event, endOffsetSeconds);
	}

	public static function removeTimelineEvent(event:TimelineEventHandle):Void {
		TimelineNative.removeTimelineEvent(event);
	}
}

@:hlNative("steam")
class TimelineNative {
	static function setTimelineTooltip(description:hl.Bytes, timeDelta:hl.F32):Void {}
	static function clearTimelineTooltip(timeDelta:hl.F32):Void {}
	static function setTimelineGameMode(mode:Int):Void {}

	static function addInstantaneousTimelineEvent(title:hl.Bytes, description:hl.Bytes, icon:hl.Bytes, priority:Int, startOffsetSeconds:hl.F32, possibleClip:Int):hl.I64 { return 0; }
	static function addRangeTimelineEvent(title:hl.Bytes, description:hl.Bytes, icon:hl.Bytes, priority:Int, startOffsetSeconds:hl.F32, durationSeconds:hl.F32, possibleClip:Int):hl.I64 { return 0; }

	static function startRangeTimelineEvent(title:hl.Bytes, description:hl.Bytes, icon:hl.Bytes, priority:Int, startOffsetSeconds:hl.F32, possibleClip:Int):hl.I64 { return 0; }
	static function updateRangeTimelineEvent(event:hl.I64, title:hl.Bytes, description:hl.Bytes, icon:hl.Bytes, priority:Int, possibleClip:Int):Void {}
	static function endRangeTimelineEvent(event:hl.I64, endOffsetSeconds:hl.F32):Void {}
	static function removeTimelineEvent(event:hl.I64):Void {}
}
