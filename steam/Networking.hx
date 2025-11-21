package steam;

typedef NetworkApi = {
	function onConnectionRequest( u : User ) : Bool;
	function onConnectionError( u : User, error : NetworkStatus ) : Void;
	function onData( u : User, data : haxe.io.Bytes ) : Void;
}

enum abstract NetworkStatus(Int) {
	var None = 0;
	var NotRunningApp = 1;
	var NoRigtsToApp = 2;
	var DestinationNotLoggedIn = 3;
	var Timeout = 4;
}

enum abstract PacketType(Int) {
	var Unreliable = 0;
	var UnreliableNoDelay = 1;
	var Reliable = 2;
	var ReliableWithBuffering = 3;
}

typedef NetworkSessionData = {
	var connecting : Bool;
	var alive : Bool;
	var relay : Bool;
	var error : NetworkStatus;
	var bytesSendQueue : Int;
	var packetSendQueue : Int;
	var remoteIP : Int;
	var remotePort : Int;
}

private typedef ThreadData = {
	var consumed : Int;
	var disposed : Int;
	var users : Array<UID>;
	var length : Array<Int>;
	var lock : sys.thread.Mutex;
}

@:hlNative("steam")
class Networking {

	static var initDone = false;
	static var api : NetworkApi;
	static var connections : Map<String,UID> = new Map();
	static var buffer : hl.Bytes = null;
	static var bufferSize = 0;

	public static var USE_MULTITHREAD(default,set) : Bool;

	static function set_USE_MULTITHREAD(b) {
		if( USE_MULTITHREAD == b ) return b;
		if( initDone ) throw "You cannot change Mulithread after init";
		USE_MULTITHREAD = b;
		return b;
	}

	public static function startP2P( napi : NetworkApi ) {
		api = napi;

		if( initDone ) return;

		initDone = true;
		// P2PSessionRequest_t
		Api.registerGlobalEvent(1202, function(data:{uid:UID}) {
			if( api == null ) return;
			var user = User.fromUID(data.uid);
			if( api.onConnectionRequest(user) ) {
				if( !accept_p2p_session(data.uid) )
					throw "fail to accept p2p session";
				addConnection(user);
			}
		});

		// P2PSessionConnectFail_t
		Api.registerGlobalEvent(1203, function(data:{uid:UID, error:NetworkStatus}) {
			if( api != null )
				api.onConnectionError(User.fromUID(data.uid), data.error);
		});

		if( USE_MULTITHREAD ) {
			var t : ThreadData = { consumed : 0, disposed : 0, users : [], length : [], lock : new sys.thread.Mutex() };
			haxe.MainLoop.add(checkP2PMain.bind(t));
			sys.thread.Thread.create(threadLoop.bind(t));
		} else
			haxe.MainLoop.add(checkP2PMessage);
	}

	static function checkP2PMessage() {
		if( api == null ) return;
		// decode message !
		var len = 0;
		while( true ) {
			var result;
			try {
				result = is_p2p_packet_available(len, 0);
			} catch( e : Dynamic ) {
				var flags = new haxe.EnumFlags<hl.UI.DialogFlags>();
				flags.set(IsError);
				hl.UI.dialog("Error", "An error occured in network layer, Steam can no longer be reached.\n("+Std.string(e)+")", flags);
				Sys.exit(1);
				return;
			}
			if( !result )
				break;
			if( len > bufferSize ) {
				bufferSize = len;
				buffer = new hl.Bytes(bufferSize);
			}
			var uid = read_p2p_packet(buffer, bufferSize, len, 0);
			if( uid == null ) continue;
			api.onData(User.fromUID(uid), buffer.toBytes(len));
		}
	}

	static function threadLoop( t : ThreadData ) {
		try {
			var len = 0;
			var pos = 0;
			while( true ) {
				if( api != null ) {
					while( true ) {
						var result = is_p2p_packet_available(len, 0);
						if( !result )
							break;
						if( len + pos > bufferSize ) {
							bufferSize = len + pos;
							var newBuf = new hl.Bytes(bufferSize);
							newBuf.blit(0, buffer, 0, pos);
							buffer = newBuf;
						}
						var uid = read_p2p_packet(buffer.offset(pos), bufferSize - pos, len, 0);
						if( uid == null ) continue;
						pos += len;
						t.lock.acquire();
						if( t.disposed > 0 ) {
							buffer.blit(0, buffer, t.disposed, pos - t.disposed);
							pos -= t.disposed;
							t.consumed -= t.disposed;
							t.disposed = 0;
						}
						t.length.push(len);
						t.users.push(uid);
						t.lock.release();
					}
				}
				Sys.sleep(1/30);
			}
		} catch( e : Dynamic ) {
			var flags = new haxe.EnumFlags<hl.UI.DialogFlags>();
			flags.set(IsError);
			hl.UI.dialog("Error", "An error occured in network layer, Steam can no longer be reached.\n("+Std.string(e)+")", flags);
			Sys.exit(1);
		}
	}

	static function checkP2PMain( t : ThreadData ) {
		if( api == null ) return;
		static var tmpBytes = haxe.io.Bytes.alloc(1);
		while( t.users.length > 0 ) {
			t.lock.acquire();
			var u = t.users.shift();
			var len = t.length.shift();
			var buffer = buffer; // keep for GC - if expanded on server
			@:privateAccess tmpBytes.b = buffer.offset(t.consumed);
			@:privateAccess tmpBytes.length = len;
			t.disposed = 0; // prevent any offset/discard
			t.consumed += len; // store in case of exception in onData
			t.lock.release();
			api.onData(User.fromUID(u), tmpBytes);
		}
		if( t.consumed > 0 && t.disposed != t.consumed ) {
			t.lock.acquire();
			t.disposed = t.consumed;
			t.lock.release();
		}
	}

	public static function sendP2P( user : User, data : haxe.io.Bytes, type : PacketType, pos = 0, len = -1 ) {
		if( len < 0 ) len = data.length;
		if( !send_p2p_packet(user.uid, (data:hl.Bytes).offset(pos), len, type, 0) )
			return false;
		addConnection(user);
		return true;
	}

	public static function closeSession( user : User ) {
		@:privateAccess {
			if( !user.p2pcnx ) return;
			user.p2pcnx = false;
		}
		// we need to delay the session close so some pending packets are delivered
		haxe.Timer.delay(close_p2p_session.bind(user.uid),100);
		connections.remove(user.uid.toString());
	}

	static inline function addConnection( user : User ) {
		@:privateAccess if( !user.p2pcnx ) {
			user.p2pcnx = true;
			connections.set(user.uid.toString(), user.uid);
		}
	}

	public static function closeP2P() {
		api = null;
		var cnx = connections;
		connections = new Map();
		for( c in cnx )
			closeSession(User.fromUID(c));
	}


	public static function getSessionData( user : User ) : NetworkSessionData {
		return get_p2p_session_data(user.uid);
	}

	// -- native

	static function send_p2p_packet( to : UID, data : hl.Bytes, dataLen : Int, type : PacketType, channel : Int ) : Bool {
		return false;
	}

	static function read_p2p_packet( out : hl.Bytes, maxLen : Int, len : hl.Ref<Int>, channel : Int ) : UID {
		return null;
	}

	static function is_p2p_packet_available( len : hl.Ref<Int>, channel : Int ) {
		return false;
	}

	static function get_p2p_session_data( uid : UID ) : Dynamic {
		return null;
	}

	static function accept_p2p_session( uid : UID ) : Bool {
		return false;
	}

	static function close_p2p_session( uid : UID ) : Bool {
		return false;
	}

}