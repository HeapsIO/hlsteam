#include "steamwrap.h"

SteamNetworkingIdentity hl_to_steamnetid(steamnetid v) {
	union {
		vbyte b[sizeof(SteamNetworkingIdentity)];
		SteamNetworkingIdentity v;
	} data;
	memcpy(data.b, v, sizeof(SteamNetworkingIdentity));
	return SteamNetworkingIdentity(data.v);
}

steamnetid hl_of_steamnetid(SteamNetworkingIdentity v) {
	union {
		vbyte b[sizeof(SteamNetworkingIdentity)];
		SteamNetworkingIdentity v;
	} data;
	data.v = v;
	return (steamnetid)hl_copy_bytes(data.b, sizeof(SteamNetworkingIdentity));
}

HL_PRIM steamnetid HL_NAME(get_identity)() {
    SteamNetworkingIdentity identity;
    SteamNetworkingSockets()->GetIdentity(&identity);
    return hl_of_steamnetid(identity);
}

HL_PRIM vbyte* HL_NAME(identity_to_string)(steamnetid identity) {
    char *buf = (char *)hl_gc_alloc_noptr(SteamNetworkingIdentity::k_cchMaxString);
    hl_to_steamnetid(identity).ToString(buf, SteamNetworkingIdentity::k_cchMaxString);
    return (vbyte*)buf;
}

HL_PRIM vbyte* HL_NAME(identity_from_string)(vbyte* v) {
    SteamNetworkingIdentity identity;
    identity.ParseString((char*)v);
    return hl_of_steamnetid(identity);
}

DEFINE_PRIM(_STEAMNETID, get_identity, _NO_ARG);
DEFINE_PRIM(_BYTES, identity_to_string, _STEAMNETID);
DEFINE_PRIM(_STEAMNETID, identity_from_string, _BYTES);

HL_PRIM void HL_NAME(init_authentication)() {
    SteamNetworkingSockets()->InitAuthentication();
}

HL_PRIM bool HL_NAME(accept_session_with_user)(steamnetid identity) {
    return SteamNetworkingMessages()->AcceptSessionWithUser(hl_to_steamnetid(identity));
}

HL_PRIM bool HL_NAME(close_session_with_user)(steamnetid identity) {
	return SteamNetworkingMessages()->CloseSessionWithUser(hl_to_steamnetid(identity));
}

HL_PRIM int HL_NAME(send_message_to_user)(steamnetid identityRemote, vbyte* pubData, int cubData, int nSendFlags, int nRemoteChannel) {
	return SteamNetworkingMessages()->SendMessageToUser(hl_to_steamnetid(identityRemote), pubData, cubData, nSendFlags, nRemoteChannel);
}

DEFINE_PRIM(_VOID, init_authentication, _NO_ARG);
DEFINE_PRIM(_BOOL, accept_session_with_user, _STEAMNETID);
DEFINE_PRIM(_BOOL, close_session_with_user, _STEAMNETID);
DEFINE_PRIM(_I32, send_message_to_user, _STEAMNETID _BYTES _I32 _I32 _I32);

vdynamic *CallbackHandler::EncodeSteamNetworkingMessagesSessionRequest(SteamNetworkingMessagesSessionRequest_t *d) {
	HLValue ret;
	ret.Set("identity", d->m_identityRemote);
	return ret.value;
}

vdynamic *CallbackHandler::EncodeSteamNetworkingMessagesSessionFailed(SteamNetworkingMessagesSessionFailed_t *d) {
	HLValue ret;
	// ret.Set("info", d->m_info);
	printf("%d %s", d->m_info.m_eEndReason, d->m_info.m_szEndDebug);
	return ret.value;
}

vdynamic *CallbackHandler::EncodeGameRichPresenceJoinRequested(GameRichPresenceJoinRequested_t *d) {
	HLValue ret;
	ret.Set("friend", d->m_steamIDFriend);
	ret.Set("connect", d->m_rgchConnect);
	return ret.value;
}

HL_PRIM SteamNetworkingMessage_t* HL_NAME(receive_message_on_channel)(int nLocalChannel) {
    SteamNetworkingMessage_t* message = NULL;
	SteamNetworkingMessages()->ReceiveMessagesOnChannel(nLocalChannel, &message, 1);
	return message;
}

HL_PRIM void HL_NAME(release_message)(SteamNetworkingMessage_t* message) {
	message->Release();
}

HL_PRIM int HL_NAME(get_message_size)(SteamNetworkingMessage_t* message) {
	return message->GetSize();
}

HL_PRIM vbyte* HL_NAME(get_message_data)(SteamNetworkingMessage_t* message) {
	return (vbyte*)message->GetData();
}

#define NWMSG _ABSTRACT(steam_networkmsg)
DEFINE_PRIM(NWMSG, receive_message_on_channel, _I32);
DEFINE_PRIM(_VOID, release_message, NWMSG);
DEFINE_PRIM(_I32, get_message_size, NWMSG);
DEFINE_PRIM(_BYTES, get_message_data, NWMSG);