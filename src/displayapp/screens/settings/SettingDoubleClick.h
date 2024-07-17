#pragma once

#include "components/settings/Settings.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/screens/ScreenList.h"

namespace Pinetime::Applications::Screens {
  class SettingDoubleClick : public Screen {
  public:
    static constexpr size_t nScreens {1};

    SettingDoubleClick(DisplayApp* app, Controllers::Settings& settingsController);
    ~SettingDoubleClick() override;

    bool OnTouchEvent(TouchEvents event) override;

  private:
    DisplayApp* app;
    ScreenList<nScreens> screens;
  };
}
