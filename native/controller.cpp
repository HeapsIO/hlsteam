#include "steamwrap.h"

void CallbackHandler::OnGamepadTextInputDismissed( GamepadTextInputDismissed_t *pCallback ){
	SendEvent(GamepadTextInputDismissed, pCallback->m_bSubmitted, NULL);
}


//A simple data structure that holds on to the native 64-bit handles and maps them to regular ints.
//This is because it is cumbersome to pass back 64-bit values over CFFI, and strictly speaking, the haxe
//side never needs to know the actual values. So we just store the full 64-bit values locally and pass back
//0-based index values which easily fit into a regular int.
class steamHandleMap{
	//TODO: figure out templating or whatever so I can make typed versions of this like in Haxe (steamHandleMap<InputHandle_t>)
	//      all the steam handle typedefs are just renamed uint64's, but this could always change so to be 100% super safe I should
	//      figure out the templating stuff.

	private:
		std::map<int, uint64> values;
		std::map<int, uint64>::iterator it;
		int maxKey;

	public:

		void init()		{
			values.clear();
			maxKey = -1;
		}

		bool exists(uint64 val){
			return find(val) >= 0;
		}

		int find(uint64 val){
			for(int i = 0; i <= maxKey; i++){
				if(values[i] == val)
					return i;
			}
			return -1;
		}

		uint64 get(int index){
			return values[index];
		}

		//add a unique uint64 value to this data structure & return what index it was stored at
		int add(uint64 val)	{
			int i = find(val);

			//if it already exists just return where it is stored
			if(i >= 0)
				return i;

			//if it is unique increase our maxKey count and return that
			maxKey++;
			values[maxKey] = val;

			return maxKey;
		}
};

static steamHandleMap mapControllers;

HL_PRIM bool HL_NAME(init_controllers)(bool explicitlyCallRunFrame){
	if (!SteamInput()) return false;

	bool result = SteamInput()->Init(explicitlyCallRunFrame);
	if( result )
		mapControllers.init();
	return result;
}
DEFINE_PRIM(_BOOL, init_controllers, _BOOL);

HL_PRIM bool HL_NAME(shutdown_controllers)(){
	bool result = SteamInput()->Shutdown();
	if (result)
		mapControllers.init();
	return result;
}
DEFINE_PRIM(_BOOL, shutdown_controllers, _NO_ARG);

HL_PRIM void HL_NAME(run_frame_controllers)(){
	SteamInput()->RunFrame(false);
}
DEFINE_PRIM(_VOID, run_frame_controllers, _NO_ARG);

HL_PRIM bool HL_NAME(new_data_available_controllers)(){
	return SteamInput()->BNewDataAvailable();
}
DEFINE_PRIM(_BOOL, new_data_available_controllers, _NO_ARG);

HL_PRIM bool HL_NAME(show_binding_panel)(int controller){
	InputHandle_t c_handle = controller != -1 ? mapControllers.get(controller) : STEAM_INPUT_HANDLE_ALL_CONTROLLERS;

	return SteamInput()->ShowBindingPanel(c_handle);
}
DEFINE_PRIM(_BOOL, show_binding_panel, _I32);

HL_PRIM bool HL_NAME(show_gamepad_text_input)(int inputMode, int lineMode, vbyte *description, int charMax, vbyte *existingText){

	EGamepadTextInputMode eInputMode = static_cast<EGamepadTextInputMode>(inputMode);
	EGamepadTextInputLineMode eLineInputMode = static_cast<EGamepadTextInputLineMode>(lineMode);

	return SteamUtils()->ShowGamepadTextInput(eInputMode, eLineInputMode, (char*)description, (uint32)charMax, (char*)existingText);
}
DEFINE_PRIM(_BOOL, show_gamepad_text_input, _I32 _I32 _BYTES _I32 _BYTES);

HL_PRIM vbyte *HL_NAME(get_entered_gamepad_text_input)(){
	uint32 length = SteamUtils()->GetEnteredGamepadTextLength();
	char *pchText = (char *)hl_gc_alloc_noptr(length);
	if( SteamUtils()->GetEnteredGamepadTextInput(pchText, length) )
		return (vbyte*)pchText;
	return (vbyte*)"";

}
DEFINE_PRIM(_BYTES, get_entered_gamepad_text_input, _NO_ARG);

HL_PRIM varray *HL_NAME(get_connected_controllers)(varray *arr){
	SteamInput()->RunFrame();

	InputHandle_t handles[STEAM_INPUT_MAX_COUNT];
	int result = SteamInput()->GetConnectedControllers(handles);

	if( !arr )
		arr = hl_alloc_array(&hlt_i32, STEAM_INPUT_MAX_COUNT);
	int *cur = hl_aptr(arr, int);
	for(int i = 0; i < result; i++)	{
		int index = mapControllers.find(handles[i]);
		if( index < 0 )
			index = mapControllers.add(handles[i]);
		cur[i] = index;
	}

	for (int i = result; i < STEAM_INPUT_MAX_COUNT; i++)
		cur[i] = -1;

	return arr;
}
DEFINE_PRIM(_ARR, get_connected_controllers, _ARR);

HL_PRIM int HL_NAME(get_action_set_handle)(vbyte *actionSetName){
	return (int)SteamInput()->GetActionSetHandle((char*)actionSetName);
}
DEFINE_PRIM(_I32, get_action_set_handle, _BYTES);

HL_PRIM int HL_NAME(get_digital_action_handle)(vbyte *actionName){
	return (int)SteamInput()->GetDigitalActionHandle((char*)actionName);
}
DEFINE_PRIM(_I32, get_digital_action_handle, _BYTES);

HL_PRIM int HL_NAME(get_analog_action_handle)(vbyte *actionName){
	return (int)SteamInput()->GetAnalogActionHandle((char*)actionName);
}
DEFINE_PRIM(_I32, get_analog_action_handle, _BYTES);

//-----------------------------------------------------------------------------------------------------------
HL_PRIM int HL_NAME(get_digital_action_data)(int controllerHandle, int actionHandle){
	InputHandle_t c_handle = controllerHandle != -1 ? mapControllers.get(controllerHandle) : STEAM_INPUT_HANDLE_ALL_CONTROLLERS;
	InputDigitalActionHandle_t a_handle = actionHandle;

	InputDigitalActionData_t data = SteamInput()->GetDigitalActionData(c_handle, a_handle);

	int result = 0;

	//Take both bools and pack them into an int

	if(data.bState)
		result |= 0x1;

	if(data.bActive)
		result |= 0x10;

	return result;
}
DEFINE_PRIM(_I32, get_digital_action_data, _I32 _I32);


//-----------------------------------------------------------------------------------------------------------

typedef struct {
	hl_type *t;
	bool bActive;
	int eMode;
	double x;
	double y;
} analog_action_data;

HL_PRIM void HL_NAME(get_analog_action_data)(int controllerHandle, int actionHandle, analog_action_data *data){
	InputHandle_t c_handle = controllerHandle != -1 ? mapControllers.get(controllerHandle) : STEAM_INPUT_HANDLE_ALL_CONTROLLERS;
	InputAnalogActionHandle_t a_handle = actionHandle;

	InputAnalogActionData_t d = SteamInput()->GetAnalogActionData(c_handle, a_handle);

	data->bActive = d.bActive;
	data->eMode = d.eMode;
	data->x = d.x;
	data->y = d.y;
}
DEFINE_PRIM(_VOID, get_analog_action_data, _I32 _I32 _OBJ(_BOOL _I32 _F64 _F64));

HL_PRIM varray *HL_NAME(get_digital_action_origins)(int controllerHandle, int actionSetHandle, int digitalActionHandle){
	InputHandle_t c_handle              = mapControllers.get(controllerHandle);
	InputActionSetHandle_t s_handle     = actionSetHandle;
	InputDigitalActionHandle_t a_handle = digitalActionHandle;

	EInputActionOrigin origins[STEAM_INPUT_MAX_ORIGINS];
	for(int i = 0; i < STEAM_INPUT_MAX_ORIGINS; i++)
		origins[i] = k_EInputActionOrigin_None;

	int result = SteamInput()->GetDigitalActionOrigins(c_handle, s_handle, a_handle, origins);
	varray *arr = hl_alloc_array(&hlt_i32, result);
	int *cur = hl_aptr(arr, int);
	for(int i = 0; i < STEAM_INPUT_MAX_ORIGINS; i++)
		cur[i] = origins[i];
	return arr;
}
DEFINE_PRIM(_ARR, get_digital_action_origins, _I32 _I32 _I32);

//-----------------------------------------------------------------------------------------------------------
HL_PRIM varray *HL_NAME(get_analog_action_origins)(int controllerHandle, int actionSetHandle, int analogActionHandle){
	InputHandle_t c_handle              = mapControllers.get(controllerHandle);
	InputActionSetHandle_t s_handle     = actionSetHandle;
	InputAnalogActionHandle_t a_handle  = analogActionHandle;

	EInputActionOrigin origins[STEAM_INPUT_MAX_ORIGINS];

	//Initialize the whole thing to None to avoid garbage
	for(int i = 0; i < STEAM_INPUT_MAX_ORIGINS; i++)
		origins[i] = k_EInputActionOrigin_None;

	int result = SteamInput()->GetAnalogActionOrigins(c_handle, s_handle, a_handle, origins);
	varray *arr = hl_alloc_array(&hlt_i32, result);
	int *cur = hl_aptr(arr, int);
	for(int i = 0; i < STEAM_INPUT_MAX_ORIGINS; i++)
		cur[i] = origins[i];
	return arr;
}
DEFINE_PRIM(_ARR, get_analog_action_origins, _I32 _I32 _I32);

//-----------------------------------------------------------------------------------------------------------
HL_PRIM vbyte *HL_NAME(get_glyph_for_action_origin)(int origin){
	if (!CheckInit()) return NULL;

	if (origin >= k_EInputActionOrigin_Count)
		return NULL;

	EInputActionOrigin eOrigin = static_cast<EInputActionOrigin>(origin);

	const char * result = SteamInput()->GetGlyphForActionOrigin_Legacy(eOrigin);
	return (vbyte*)result;
}
DEFINE_PRIM(_BYTES, get_glyph_for_action_origin, _I32);

//-----------------------------------------------------------------------------------------------------------
HL_PRIM vbyte *HL_NAME(get_string_for_action_origin)(int origin){
	if (!CheckInit()) return NULL;

	if (origin >= k_EInputActionOrigin_Count)
		return NULL;

	EInputActionOrigin eOrigin = static_cast<EInputActionOrigin>(origin);

	const char * result = SteamInput()->GetStringForActionOrigin(eOrigin);
	return (vbyte*)result;
}
DEFINE_PRIM(_BYTES, get_string_for_action_origin, _I32);

//-----------------------------------------------------------------------------------------------------------
HL_PRIM void HL_NAME(activate_action_set)(int controllerHandle, int actionSetHandle){
	InputHandle_t c_handle = controllerHandle != -1 ? mapControllers.get(controllerHandle) : STEAM_INPUT_HANDLE_ALL_CONTROLLERS;
	InputActionSetHandle_t a_handle = actionSetHandle;

	SteamInput()->ActivateActionSet(c_handle, a_handle);
}
DEFINE_PRIM(_VOID, activate_action_set, _I32 _I32);

//-----------------------------------------------------------------------------------------------------------
HL_PRIM int HL_NAME(get_current_action_set)(int controllerHandle){
	InputHandle_t c_handle = controllerHandle != -1 ? mapControllers.get(controllerHandle) : STEAM_INPUT_HANDLE_ALL_CONTROLLERS;
	return (int)SteamInput()->GetCurrentActionSet(c_handle);
}
DEFINE_PRIM(_I32, get_current_action_set, _I32);

HL_PRIM void HL_NAME(trigger_haptic_pulse)(int controllerHandle, int targetPad, int durationMicroSec){
	InputHandle_t c_handle = controllerHandle != -1 ? mapControllers.get(controllerHandle) : STEAM_INPUT_HANDLE_ALL_CONTROLLERS;
	ESteamControllerPad eTargetPad;
	switch(targetPad)	{
		case 0:  eTargetPad = k_ESteamControllerPad_Left;
		case 1:  eTargetPad = k_ESteamControllerPad_Right;
		default: eTargetPad = k_ESteamControllerPad_Left;
	}
	unsigned short usDurationMicroSec = durationMicroSec;

	SteamInput()->Legacy_TriggerHapticPulse(c_handle, eTargetPad, usDurationMicroSec);
}
DEFINE_PRIM(_VOID, trigger_haptic_pulse, _I32 _I32 _I32);

HL_PRIM void HL_NAME(trigger_repeated_haptic_pulse)(int controllerHandle, int targetPad, int durationMicroSec, int offMicroSec, int repeat, int flags){
	InputHandle_t c_handle = controllerHandle != -1 ? mapControllers.get(controllerHandle) : STEAM_INPUT_HANDLE_ALL_CONTROLLERS;
	ESteamControllerPad eTargetPad;
	switch(targetPad)	{
		case 0:  eTargetPad = k_ESteamControllerPad_Left;
		case 1:  eTargetPad = k_ESteamControllerPad_Right;
		default: eTargetPad = k_ESteamControllerPad_Left;
	}
	unsigned short usDurationMicroSec = durationMicroSec;
	unsigned short usOffMicroSec = offMicroSec;
	unsigned short unRepeat = repeat;
	unsigned short nFlags = flags;

	SteamInput()->Legacy_TriggerRepeatedHapticPulse(c_handle, eTargetPad, usDurationMicroSec, usOffMicroSec, unRepeat, nFlags);
}
DEFINE_PRIM(_VOID, trigger_repeated_haptic_pulse, _I32 _I32 _I32 _I32 _I32 _I32);

HL_PRIM void HL_NAME(trigger_vibration)(int controllerHandle, int leftSpeed, int rightSpeed){
	InputHandle_t c_handle = controllerHandle != -1 ? mapControllers.get(controllerHandle) : STEAM_INPUT_HANDLE_ALL_CONTROLLERS;
	SteamInput()->TriggerVibration(c_handle, (unsigned short)leftSpeed, (unsigned short)rightSpeed);
}
DEFINE_PRIM(_VOID, trigger_vibration, _I32 _I32 _I32);

HL_PRIM void HL_NAME(set_led_color)(int controllerHandle, int r, int g, int b, int flags){
	InputHandle_t c_handle = controllerHandle != -1 ? mapControllers.get(controllerHandle) : STEAM_INPUT_HANDLE_ALL_CONTROLLERS;
	SteamInput()->SetLEDColor(c_handle, (uint8)r, (uint8)g, (uint8)b, (unsigned int) flags);
}
DEFINE_PRIM(_VOID, set_led_color, _I32 _I32 _I32 _I32 _I32);

//-----------------------------------------------------------------------------------------------------------
typedef struct {
	hl_type *t;
	double rotQuatX;
	double rotQuatY;
	double rotQuatZ;
	double rotQuatW;
	double posAccelX;
	double posAccelY;
	double posAccelZ;
	double rotVelX;
	double rotVelY;
	double rotVelZ;
} motion_data;

HL_PRIM void HL_NAME(get_motion_data)(int controllerHandle, motion_data *data){
	InputHandle_t c_handle = controllerHandle != -1 ? mapControllers.get(controllerHandle) : STEAM_INPUT_HANDLE_ALL_CONTROLLERS;
	InputMotionData_t d = SteamInput()->GetMotionData(c_handle);

	data->rotQuatX = d.rotQuatX;
	data->rotQuatY = d.rotQuatY;
	data->rotQuatZ = d.rotQuatZ;
	data->rotQuatW = d.rotQuatW;
	data->posAccelX = d.posAccelX;
	data->posAccelY = d.posAccelY;
	data->posAccelZ = d.posAccelZ;
	data->rotVelX = d.rotVelX;
	data->rotVelY = d.rotVelY;
	data->rotVelZ = d.rotVelZ;
}
DEFINE_PRIM(_VOID, get_motion_data, _I32 _OBJ(_F64 _F64 _F64 _F64 _F64 _F64 _F64 _F64 _F64 _F64));

HL_PRIM bool HL_NAME(show_digital_action_origins)(int controllerHandle, int digitalActionHandle, double scale, double xPosition, double yPosition){
	InputHandle_t c_handle = controllerHandle != -1 ? mapControllers.get(controllerHandle) : STEAM_INPUT_HANDLE_ALL_CONTROLLERS;
	return false; //SteamController()->ShowDigitalActionOrigins(c_handle, digitalActionHandle, (float)scale, (float)xPosition, (float)yPosition);
}
DEFINE_PRIM(_BOOL, show_digital_action_origins, _I32 _I32 _F64 _F64 _F64);

HL_PRIM bool HL_NAME(show_analog_action_origins)(int controllerHandle, int analogActionHandle, double scale, double xPosition, double yPosition){
	InputHandle_t c_handle = controllerHandle != -1 ? mapControllers.get(controllerHandle) : STEAM_INPUT_HANDLE_ALL_CONTROLLERS;
	return false; //SteamController()->ShowAnalogActionOrigins(c_handle, analogActionHandle, (float)scale, (float)xPosition, (float)yPosition);
}
DEFINE_PRIM(_BOOL, show_analog_action_origins, _I32 _I32 _F64 _F64 _F64);


//-----------------------------------------------------------------------------------------------------------
HL_PRIM int HL_NAME(get_controller_max_count)(){
	return STEAM_INPUT_MAX_COUNT;
}
DEFINE_PRIM(_I32, get_controller_max_count, _NO_ARG);

HL_PRIM int HL_NAME(get_controller_max_analog_actions)(){
	return STEAM_INPUT_MAX_ANALOG_ACTIONS;
}
DEFINE_PRIM(_I32, get_controller_max_analog_actions, _NO_ARG);

HL_PRIM int HL_NAME(get_controller_max_digital_actions)(){
	return STEAM_INPUT_MAX_DIGITAL_ACTIONS;
}
DEFINE_PRIM(_I32, get_controller_max_digital_actions, _NO_ARG);

HL_PRIM int HL_NAME(get_controller_max_origins)(){
	return STEAM_INPUT_MAX_ORIGINS;
}
DEFINE_PRIM(_I32, get_controller_max_origins, _NO_ARG);

HL_PRIM double HL_NAME(get_controller_min_analog_action_data)(){
	return STEAM_INPUT_MIN_ANALOG_ACTION_DATA;
}
DEFINE_PRIM(_F64, get_controller_min_analog_action_data, _NO_ARG);

HL_PRIM double HL_NAME(get_controller_max_analog_action_data)(){
	return STEAM_INPUT_MAX_ANALOG_ACTION_DATA;
}
DEFINE_PRIM(_F64, get_controller_max_analog_action_data, _NO_ARG);

