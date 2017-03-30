#include "SettingsUI.h"
#include "EngineTypes.h"
#include "EnginePointers.h"
#include "Patcher.h"
#include "PatchGroup.h"
#include "Codefinder.h"
#include "DebugHelper.h"
#include "SettingsUIEnums.h"
#include "Preferences.h"
#include <vector>
#include <memory>

using namespace EngineTypes;

namespace SettingsUI {

struct Bounds {
	Bounds(std::uint16_t lower, std::uint16_t upper) {
		this->lower = lower;
		this->upper = upper;
	}
	std::uint16_t lower;
	std::uint16_t upper;
};

typedef std::vector<std::pair<std::uint16_t, Bounds>> SliderValues;
typedef int (*UI_EVENT_HANDLER)(UIWidgetGroup*, int, int);
typedef int (*GAME_DATA_INPUT_HANDLER)(UIWidgetGroup*);

std::unique_ptr<PatchGroup> patches = nullptr;
UI_EVENT_HANDLER eventHandlers[UI_EVENT_CALLBACKS_END];
GAME_DATA_INPUT_HANDLER gameDataHandlers[DATA_INPUT_FUNCTIONS_END];
SliderValues chatPrefs, opticPrefs, controlPrefs, generalPrefs;

SliderValues generalGetDefaults();
SliderValues opticGetDefaults();
SliderValues controlsGetDefaults();
SliderValues chatGetDefaults();
void updateIntermediatePreferences(SliderValues& prefs, const std::vector<std::uint16_t>& values);
int __cdecl opticMenuUpdate(UIWidgetGroup* group);
int __cdecl chatMenuUpdate(UIWidgetGroup* group);
int __cdecl generalSetDefaults(UIWidgetGroup* group, int two, int three);
int __cdecl generalInitialiseOptions(UIWidgetGroup* group, int two, int three);
int __cdecl generalSave(UIWidgetGroup* group, int two, int three);
int __cdecl chatSetDefaults(UIWidgetGroup* group, int two, int three);
int __cdecl chatInitialiseOptions(UIWidgetGroup* group, int two, int three);
int __cdecl chatDisposeOptions(UIWidgetGroup* group, int two, int three);
int __cdecl chatSave(UIWidgetGroup* group, int two, int three);
int __cdecl controlsSetDefaults(UIWidgetGroup* group, int two, int three);
int __cdecl controlsInitialiseOptions(UIWidgetGroup* group, int two, int three);
int __cdecl controlsSave(UIWidgetGroup* group, int two, int three);
int __cdecl opticSetDefaults(UIWidgetGroup* group, int two, int three);
int __cdecl opticInitialiseOptions(UIWidgetGroup* group, int two, int three);
int __cdecl opticDisposeOptions(UIWidgetGroup* group, int two, int three);
int __cdecl opticSave(UIWidgetGroup* group, int two, int three);
int hacOptionsBeginEditing(UIWidgetGroup* group, int two, int three);
int hacOptionsEndEditing(UIWidgetGroup* group, int two, int three);
int hacOptionsSet(UIWidgetGroup* group, int two, int three);
int __cdecl hacOptionsDispose(UIWidgetGroup* group, int two, int three);
void setChatSpinners(UIWidgetGroup* group);
void setSpinners(UIWidgetGroup* widget, const SliderValues& values);
std::vector<std::uint16_t> getSpinners(UIWidgetGroup* group, int spinners);
bool playerEditingProfile = false;
bool stalePreferences = false;

void appendEventHandlers() {
	short signature[] = {0xFF, 0x14, 0x8D, -1, -1, -1, -1, 0x83, 0xC4, 0x0C, 0x84, 0xC0};
	std::uintptr_t address = FindCode(GetModuleHandle(0), signature, sizeof(signature) / 2);

	if(address == NULL) {
		throw HookException("Erp.");
	}
	
	UI_EVENT_HANDLER* original = *reinterpret_cast<UI_EVENT_HANDLER**>(address += 3);
	memcpy_s(eventHandlers, UI_EVENT_CALLBACKS_END * 4, original, (LOAD_GAME_MENU_DELETE_FINISH + 1) * 4);
	
	eventHandlers[GENERAL_SCREEN_INIT] = generalInitialiseOptions;
	eventHandlers[GENERAL_SCREEN_DEFAULTS] = generalSetDefaults;
	eventHandlers[GENERAL_SCREEN_SET] = generalSave;
	eventHandlers[OPTIC_SCREEN_INIT] = opticInitialiseOptions;
	//eventHandlers[OPTIC_SCREEN_DISPOSE] = opticDisposeOptions;
	eventHandlers[OPTIC_SCREEN_DEFAULTS] = opticSetDefaults;
	eventHandlers[OPTIC_SCREEN_SET] = opticSave;
	eventHandlers[CHAT_SCREEN_INIT] = chatInitialiseOptions;
	//eventHandlers[CHAT_SCREEN_DISPOSE] = chatDisposeOptions
	eventHandlers[CHAT_SCREEN_DEFAULTS] = chatSetDefaults;
	eventHandlers[CHAT_SCREEN_SET] = chatSave;
	eventHandlers[HAC_CONTROLS_SCREEN_INIT] = controlsInitialiseOptions;
	eventHandlers[HAC_CONTROLS_SCREEN_DEFAULTS] = controlsSetDefaults;
	eventHandlers[HAC_CONTROLS_SET] = controlsSave;
	eventHandlers[HAC_OPTIONS_BEGIN_EDITING] = hacOptionsBeginEditing;
	eventHandlers[HAC_OPTIONS_END_EDITING] = hacOptionsEndEditing;
	eventHandlers[HAC_OPTIONS_SAVE_PREFERENCES] = hacOptionsSet;
}

void appendDataInputHandlers() {
	short signature[] = {0xFF, 0x14, 0x85, -1, -1, -1, -1, 0x83, 0xC4, 0x04, 0x8B};
	std::uintptr_t address = FindCode(GetModuleHandle(0), signature, sizeof(signature) / 2);

	if(address == NULL) {
		throw HookException("Erp.");
	}
	
	GAME_DATA_INPUT_HANDLER* original = *reinterpret_cast<GAME_DATA_INPUT_HANDLER**>(address += 3);
	memcpy_s(eventHandlers, DATA_INPUT_FUNCTIONS_END * 4, original, (NETWORK_SETTINGS_UPDATE + 1) * 4);
	
	//gameDataHandlers[OPTIC_MENU_UPDATE] = opticMenuUpdate;
	//gameDataHandlers[CHAT_MENU_UPDATE] = chatMenuUpdate;
}

void install() {
	appendEventHandlers();
	appendDataInputHandlers();

	patches = std::unique_ptr<PatchGroup>(new PatchGroup());
	
	{ //Get Halo to use our event handler callbacks
		std::uint8_t replacement[sizeof(void*)];
		UI_EVENT_HANDLER* pHandlers = eventHandlers;
		memcpy(replacement, &pHandlers, sizeof(void*));
		short signature[] = {0xFF, 0x14, 0x8D, -1, -1, -1, -1, 0x83, 0xC4, 0x0C};
		CodeFinder cf(GetModuleHandle(0), signature, sizeof(signature) / 2);
		for(auto i = cf.begin(); i != cf.end(); i++) {
			patches->add(new PatchHook(*i, 3, replacement, sizeof(replacement)));
		}
	}

	{ //Adjust event handler callback bounds checking
		short signature[] = {0x66, 0x3D, 0xBE, 0x00, 0x73};
		std::uint16_t replacement = UI_EVENT_CALLBACKS_END;
		CodeFinder cf(GetModuleHandle(0), signature, sizeof(signature) / 2);
		for(auto i = cf.begin(); i != cf.end(); i++) {
			patches->add(new PatchHook(*i, 2, reinterpret_cast<std::uint8_t*>(&replacement), sizeof(replacement)));
		}
	}

	{ //Get Halo to use our game data input callbacks
		std::uint8_t replacement[sizeof(void*)];
		GAME_DATA_INPUT_HANDLER* pHandlers = gameDataHandlers;
		memcpy(replacement, &pHandlers, sizeof(void*));
		short signature[] = {0xFF, 0x14, 0x85, -1, -1, -1, -1, 0x83, 0xC4, 0x04, 0x8B};
		CodeFinder cf(GetModuleHandle(0), signature, sizeof(signature) / 2);
		for(auto i = cf.begin(); i != cf.end(); i++) {
			patches->add(new PatchHook(*i, 3, replacement, sizeof(replacement)));
		}
	}

	{ //Adjust game data callback bounds checking
		short signature[] = {0x66, 0x3D, 0x3B};
		std::uint16_t replacement = DATA_INPUT_FUNCTIONS_END;
		CodeFinder cf(GetModuleHandle(0), signature, sizeof(signature) / 2);
		for(auto i = cf.begin(); i != cf.end(); i++) {
			patches->add(new PatchHook(*i, 2, reinterpret_cast<std::uint8_t*>(&replacement), sizeof(replacement)));
		}
	}

	if(!patches->install()) {
		patches.reset();
		throw HookException("Erp.");
	}
}

void uninstall() {
	patches.reset();
}

void tempPreferencesDispose() {
	generalPrefs.clear();
	chatPrefs.clear();
	opticPrefs.clear();
	controlPrefs.clear();
}

void savePreferences() {
	Preferences::add("hud_ratio_fix", generalPrefs[0].first);
	Preferences::add("no_scope_blur", generalPrefs[1].first);
	Preferences::add("active_camo_fix", generalPrefs[2].first);
	Preferences::add("play_during_queue", generalPrefs[3].first);
	Preferences::add("optic_pack", "er");
	Preferences::add("optic_redirect", opticPrefs[1].first);
	Preferences::add("chat_renderer", chatPrefs[1].first);
	Preferences::add("line_limit",  chatPrefs[2].first);
	Preferences::add("display_time", chatPrefs[3].first);
	Preferences::add("chat_align", chatPrefs[4].first);
	Preferences::add("combat_align", chatPrefs[5].first);
	Preferences::add("curved_effect", chatPrefs[6].first);
	Preferences::add("typeface", chatPrefs[7].first);
	Preferences::add("font_size", chatPrefs[8].first);
	Preferences::add("log_messages", chatPrefs[9].first);
}

int hacOptionsSet(UIWidgetGroup* group, int two, int three) {
	if(stalePreferences) {
		savePreferences();
	}

	tempPreferencesDispose();
	playerEditingProfile = false;
	return 1;
}

int hacOptionsEndEditing(UIWidgetGroup* group, int two, int three) {
	tempPreferencesDispose();
	playerEditingProfile = false;
	return 1;
}

/* Welcome to converting between Halo's preferences system and HAC's */
int hacOptionsBeginEditing(UIWidgetGroup* group, int two, int three) {
	if(playerEditingProfile) {
		return 1;
	}

	SliderValues general = generalGetDefaults();
	std::pair<std::uint16_t, Bounds>& default = general[WIDESCREEN_HUD];
	generalPrefs.emplace_back(std::make_pair(Preferences::find<std::uint16_t>("hud_ratio_fix", default.first), default.second));
	default = general[SCOPE_BLUR];
	generalPrefs.emplace_back(std::make_pair(!Preferences::find<std::uint16_t>("no_scope_blur", default.first), default.second));
	default = general[ACTIVE_CAMO];
	generalPrefs.emplace_back(std::make_pair(Preferences::find<std::uint16_t>("active_camo_fix", default.first), default.second));
	default = general[PLAY_DURING_QUEUE];
	generalPrefs.emplace_back(std::make_pair(Preferences::find<std::uint16_t>("play_during_queue", default.first), default.second));

	SliderValues optic = opticGetDefaults();
	opticPrefs.emplace_back(std::make_pair(0, Bounds(0, 1))); //generated by a function later on
	default = optic[MEDAL_MESSAGES];
	opticPrefs.emplace_back(std::make_pair(Preferences::find<std::uint16_t>("optic_redirect", default.first), default.second));

	SliderValues chatDefs = chatGetDefaults();
	default = chatDefs[CHAT_RENDERER];
	chatPrefs.emplace_back(std::make_pair(Preferences::find<std::uint16_t>("chat_renderer", default.first), default.second));
	default = chatDefs[LINE_LIMIT];
	chatPrefs.emplace_back(std::make_pair(Preferences::find<std::uint16_t>("line_limit",  default.first), default.second));
	default = chatDefs[DISPLAY_TIME];
	chatPrefs.emplace_back(std::make_pair(Preferences::find<std::uint16_t>("display_time", default.first), default.second));
	default = chatDefs[CHAT_ALIGN];
	chatPrefs.emplace_back(std::make_pair(Preferences::find<std::uint16_t>("chat_align", default.first), default.second));
	default = chatDefs[COMBAT_ALIGN];
	chatPrefs.emplace_back(std::make_pair(Preferences::find<std::uint16_t>("combat_align", default.first), default.second));
	default = chatDefs[CURVED_EFFECT];
	chatPrefs.emplace_back(std::make_pair(Preferences::find<std::uint16_t>("curved_effect", default.first), default.second));
	default = chatDefs[TYPEFACE];
	chatPrefs.emplace_back(std::make_pair(Preferences::find<std::uint16_t>("typeface", default.first), default.second));
	default = chatDefs[FONT_SIZE];
	chatPrefs.emplace_back(std::make_pair(Preferences::find<std::uint16_t>("font_size", default.first), default.second));
	default = chatDefs[LOG_MESSAGES];
	chatPrefs.emplace_back(std::make_pair(Preferences::find<std::uint16_t>("log_messages", default.first), default.second));

	playerEditingProfile = true;
	return 1;
}


int opticSave(UIWidgetGroup* group, int two, int three) {
	std::vector<std::uint16_t> values = getSpinners(group->parent->parent, OPTIC_SETTINGS::OS_END);
	updateIntermediatePreferences(opticPrefs, values);
	return 1;
}

int chatSave(UIWidgetGroup* group, int two, int three) {
	std::vector<std::uint16_t> values = getSpinners(group->parent->parent, CHAT_SETTINGS::CS_END);
	updateIntermediatePreferences(chatPrefs, values);
	return 1;
}

int controlsSave(UIWidgetGroup* group, int two, int three) {

	return 1;
}

int generalSetDefaults(UIWidgetGroup* group, int two, int three) {
	setSpinners(group->parent->parent, generalGetDefaults());
	return 1;
}

void updateIntermediatePreferences(SliderValues& prefs, const std::vector<std::uint16_t>& values) {
	if(values.size() != prefs.size()) {
		MessageBox(NULL, "Fix me", 0, 0);
		return;
	}

	for(int i = 0, j = values.size(); i < j; i++) {
		Bounds bounds = prefs[i].second;
		std::uint16_t index = values[i];
		if(index >= bounds.lower && index < bounds.upper) {
			if(index != prefs[i].first) {
				prefs[i].first = index;
				stalePreferences = true;
			}
		}
	}
}

int generalSave(UIWidgetGroup* group, int two, int three) {
	std::vector<std::uint16_t> values = getSpinners(group->parent->parent, GENERAL_SETTINGS::GS_END);
	updateIntermediatePreferences(generalPrefs, values);
	return 1;
}

SliderValues generalGetDefaults() {
	SliderValues values;
	values.emplace_back(std::make_pair(YES_NO::YES, Bounds(0, YES_NO::YN_END)));
	values.emplace_back(std::make_pair(YES_NO::YES, Bounds(0, YES_NO::YN_END)));
	values.emplace_back(std::make_pair(YES_NO::YES, Bounds(0, YES_NO::YN_END)));	
	values.emplace_back(std::make_pair(YES_NO::NO, Bounds(0, YES_NO::YN_END)));
	return values;
}

int generalInitialiseOptions(UIWidgetGroup* group, int two, int three) {
	setSpinners(group, generalPrefs);
	return 1;
}

SliderValues controlsGetDefaults() {
}

SliderValues opticGetDefaults() {
	SliderValues values;
	values.emplace_back(std::make_pair(YES_NO::NO, Bounds(0, YES_NO::YN_END)));
	values.emplace_back(std::make_pair(MESSAGE_DISPLAY::HUD_ONLY, Bounds(0, MESSAGE_DISPLAY::MD_END)));
	return values;
}

SliderValues chatGetDefaults() {
	SliderValues values;
	values.emplace_back(std::make_pair(CHAT_RENDERER::HAC2, Bounds(0, CHAT_RENDERER::CR_END)));
	values.emplace_back(std::make_pair(LINE_LIMIT::SIX, Bounds(0, LINE_LIMIT::LL_END)));
	values.emplace_back(std::make_pair(DISPLAY_TIME::_SEVEN, Bounds(0, DISPLAY_TIME::DT_END)));	
	values.emplace_back(std::make_pair(TEXT_ALIGN::LEFT, Bounds(0, TEXT_ALIGN::TA_END)));
	values.emplace_back(std::make_pair(TEXT_ALIGN::LEFT, Bounds(0, TEXT_ALIGN::TA_END)));
	values.emplace_back(std::make_pair(YES_NO::YES, Bounds(0, YES_NO::YN_END)));
	values.emplace_back(std::make_pair(TYPEFACE_DEFAULTS::DEFAULT, Bounds(0, TYPEFACE_DEFAULTS::TD_END)));
	values.emplace_back(std::make_pair(FONT_SIZE::THIRTY_FOUR, Bounds(0, FONT_SIZE::FS_END)));
	values.emplace_back(std::make_pair(YES_NO::NO, Bounds(0, YES_NO::YN_END)));
	return values;
}

int chatSetDefaults(UIWidgetGroup* group, int two, int three) {
	setSpinners(group->parent->parent, chatGetDefaults());
	return 1;
}

int opticSetDefaults(UIWidgetGroup* group, int two, int three) {
	setSpinners(group->parent->parent, opticGetDefaults());
	return 1;
}

int controlsSetDefaults(UIWidgetGroup* group, int two, int three) {
	SliderValues values;
	setSpinners(group->parent->parent, values);
	return 1;
}

int controlsInitialiseOptions(UIWidgetGroup* group, int two, int three) {
	setSpinners(group, controlPrefs);
	return 1;
}

int opticInitialiseOptions(UIWidgetGroup* group, int two, int three) {
	setSpinners(group, opticPrefs);
	return 1;
}

int chatInitialiseOptions(UIWidgetGroup* group, int two, int three) {
	setSpinners(group, chatPrefs);
	return 1;
}

std::vector<std::uint16_t> getSpinners(UIWidgetGroup* group, int spinners) {
	std::vector<std::uint16_t> values;
	UIWidgetGroup* listItem = group->child;

	for(int i = 0; i < spinners; i++) {
		UIWidgetGroup* element = listItem->child;
		
		while(element->index != 2) {
			element = element->next;
			if(!element->index) {
				//error
			}
		}
		
		values.emplace_back(element->stringIndex);
		listItem = listItem->next;
	}

	return values;
}

void setSpinners(UIWidgetGroup* group, const SliderValues& values) {
	UIWidgetGroup* listItem = group->child; //column list widget to first child container

	for(auto i = values.begin(); i != values.end(); i++) {
		UIWidgetGroup* element = listItem->child; //child container to first element (label or spinner)
		
		while(element->index != 2) {
			element = element->next;
			if(!element->index) {
				//error
			}
		}

		if(i->first < i->second.lower || i->first >= i->second.upper) {
			element->stringIndex = i->second.lower;
		} else {
			element->stringIndex = i->first;
		}

		listItem = listItem->next;
	}
}

template<typename T>
std::vector<T*> tagSearch(uint32_t tagClass, const char* tagName = NULL) {
	TagEntry* table = pTagTableHeader->TagTableEntryPointer;
	std::vector<T*> tags;

	for(uint32_t i = 0; i < pTagTableHeader->TagCount; i++) {
		if(table[i].class0 == tagClass) {
			if(tagName == NULL || _stricmp(table[i].name, tagName) == 0) {
				tags.emplace_back(reinterpret_cast<T*>(table[i].tagStruct));
			}
		}
	}

	return tags;
}

void testFunc() {
	std::vector<UIWidget*> widgets = tagSearch<UIWidget>(DELA, "ui\\shell\\main_menu\\settings_select\\player_setup\\player_profile_edit\\video_settings\\video_options_menu");
	
	if(widgets.empty()) {
		DebugHelper::DisplayAddress(1337, 10);
	}

	UIWidget* widget = widgets[0];
	DebugHelper::DisplayAddress((DWORD)&(widget->functions.tag[0]), 16);
	//DebugHelper((DWORD)&widget->eventHandlers.tag[1].function, 16);
}

};