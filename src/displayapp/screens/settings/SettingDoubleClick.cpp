#include "displayapp/screens/settings/SettingDoubleClick.h"

#include "components/settings/Settings.h"
#include "displayapp/DisplayApp.h"
#include "displayapp/screens/CheckboxList.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/screens/Symbols.h"

#include <lvgl/lvgl.h>

#include <algorithm>
#include <array>
#include <memory>
#include <ranges>

namespace {
  struct Item {
    const char* name;
    Pinetime::Controllers::Settings::DoubleClickAction action;
    bool enabled {true};
  };

  std::unique_ptr<Pinetime::Applications::Screens::Screen> CreateScreen(unsigned int screenNum,
                                                                        Pinetime::Controllers::Settings& settingsController) {
    static constexpr std::array options = {
      Item {"None", Pinetime::Controllers::Settings::DoubleClickAction::None},
      Item {"Notifications", Pinetime::Controllers::Settings::DoubleClickAction::Notifications},
      Item {"Timer", Pinetime::Controllers::Settings::DoubleClickAction::Timer},
    };

    // clang-format off
    // Unsure how to fix clang-format but without this becomes horrible
    const auto optionList = [screenNum]() {
      std::array<Pinetime::Applications::Screens::CheckboxList::Item, Pinetime::Applications::Screens::CheckboxList::MaxItems> tempOptionList;
      const auto optionsRange = std::views::iota(0u, Pinetime::Applications::Screens::CheckboxList::MaxItems)
        | std::views::transform([screenNum](size_t i) { return i + (screenNum * Pinetime::Applications::Screens::CheckboxList::MaxItems); })
        | std::views::transform([&](size_t optionsIndex) -> Pinetime::Applications::Screens::CheckboxList::Item {
            if (optionsIndex >= options.size()) {
              return {"", false};
            }
            const auto& [name, action, enabled] = options[optionsIndex];
            std::ignore = action;
            return {name, enabled};
        });
      std::ranges::copy(optionsRange, tempOptionList.begin());
      return tempOptionList;
    }();
    // clang-format on

    return std::make_unique<Pinetime::Applications::Screens::CheckboxList>(
      screenNum,
      Pinetime::Applications::Screens::SettingDoubleClick::nScreens,
      "Double Click",
      Pinetime::Applications::Screens::Symbols::doubleClick,
      static_cast<uint32_t>(settingsController.GetDoubleClickAction()),
      [&settings = settingsController](uint32_t index) {
        settings.SetDoubleClickAction(static_cast<Pinetime::Controllers::Settings::DoubleClickAction>(index));
      },
      optionList);
  }

  auto CreateScreenList(Pinetime::Controllers::Settings& settingsController) {
    std::array<std::function<std::unique_ptr<Pinetime::Applications::Screens::Screen>()>,
               Pinetime::Applications::Screens::SettingDoubleClick::nScreens>
      screens;
    for (size_t i = 0; i < screens.size(); i++) {
      screens[i] = [i, &settingsController]() -> std::unique_ptr<Pinetime::Applications::Screens::Screen> {
        return CreateScreen(i, settingsController);
      };
    }
    return screens;
  }

}

Pinetime::Applications::Screens::SettingDoubleClick::SettingDoubleClick(DisplayApp* app, Controllers::Settings& settingsController)
  : app {app}, screens {app, 0, CreateScreenList(settingsController), Screens::ScreenListModes::UpDown} {
}

Pinetime::Applications::Screens::SettingDoubleClick::~SettingDoubleClick() {
  lv_obj_clean(lv_scr_act());
}

bool Pinetime::Applications::Screens::SettingDoubleClick::OnTouchEvent(Pinetime::Applications::TouchEvents event) {
  return screens.OnTouchEvent(event);
}