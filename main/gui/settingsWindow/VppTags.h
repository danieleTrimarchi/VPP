#ifndef VPP_TAGS_H
#define VPP_TAGS_H

#include <string>

/// XML tag used to identify the beginning of the settings
static std::string vppSettingsTag("vppSettings");

/// XML tag used to identify the version of the vpp settings
static std::string vppSettingsVersionTag("version");

/// XML tag used to identify the version of the vpp settings
static std::string vppSettingsVersion("1.0");

/// XML tag used to describe if an item is expanded
static std::string ExpandedTag("Expanded");

/// XML tag used to describe the internal name of an item
static std::string internalNameTag("InternalName");

/// XML tag used to describe the internal name of an item
static std::string displayNameTag("DisplayName");

/// XML tag used to describe the value of an item
static std::string valueTag("Value");

/// XML tag used to describe the value of an item
static std::string nameTag("Name");

/// XML tag used to describe the unit of an item
static std::string unitTag("Unit");

/// XML tag used to describe the name of a class
static std::string classNameTag("ClassName");

/// XML tag used to describe the tooltip of an item
static std::string tooltipTag("ToolTip");

/// XML tag used for identifying the begin of the settings tree section
static std::string vppSettingTreeTag("vppSettingTree");

/// XML tag used for identifying the begin of the General Settings section
static std::string vppGeneralSettingTag("vppGeneralSettings");

/// XML tag used to identify the solver index of a combo-box
static std::string comboBoxActiveSolver("Solver");



#endif
