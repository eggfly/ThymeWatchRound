#include "ThymeApp.h"

#include <framework/app_manager.h>
#include <common.h>
#include <apps/apps.h>

#define TAG "ThymeApp"

using namespace Thyme;

void ThymeApp::onBackPressed()
{
    MY_LOG("ThymeApp::onBackPressed()");
    AppManager::navigateToApp<ThymeWatchFace>();
}
