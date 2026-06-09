package steam;

private typedef NetworkMessage = hl.Abstract<"steam_networkmsg">;

@:hlNative("steam")
class NetworkingMessages {
    public static function getIdentity():SteamNetworkingIdentity return null;
    public static function identityToString(identity) return @:privateAccess String.fromUTF8(_IdentityToString(identity));
    public static function identityFromString(v:String) return @:privateAccess _IdentityFromString(v.toUtf8());    
    
    public static function initAuthentication() {}
    public static function acceptSessionWithUser(identity:SteamNetworkingIdentity):Bool return false;
    public static function closeSessionWithUser(identity:SteamNetworkingIdentity):Bool return false;
    public static function sendMessageToUser(identity:SteamNetworkingIdentity, data:haxe.io.Bytes, length:Int, nSendFlags:Int, nRemoteChannel:Int):Int {
        return _SendMessageToUser(identity, hl.Bytes.fromBytes(data), length, nSendFlags, nRemoteChannel);
    }
    public static var onSessionRequest:SteamNetworkingIdentity -> Void;

    public static function receiveMessageOnChannel(channel:Int):NetworkMessage return null;
    public static function releaseMessage(msg:NetworkMessage) {}
    public static function getMessageSize(msg:NetworkMessage):Int return -1;
    public static function getMessageData(msg:NetworkMessage):haxe.io.Bytes {
        return _GetMessageData(msg).toBytes(getMessageSize(msg));
    }
    
    @:hlNative("?steam", "identity_to_string") private static function _IdentityToString(identity:SteamNetworkingIdentity):hl.Bytes return null;
    @:hlNative("?steam", "identity_from_string") private static function _IdentityFromString(v:hl.Bytes):SteamNetworkingIdentity return null;
    @:hlNative("?steam", "send_message_to_user") private static function _SendMessageToUser(identity:SteamNetworkingIdentity, pubData:hl.Bytes, cubData:Int, nSendFlags:Int, nRemoteChannel:Int):Int return -1;
    @:hlNative("?steam", "get_message_data") private static function _GetMessageData(msg:NetworkMessage):hl.Bytes return null;
}