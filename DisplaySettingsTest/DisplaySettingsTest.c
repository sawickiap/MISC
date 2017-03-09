// DisplaySettingsTest
// Author:  Adam Sawicki, http://asawicki.info/, adam__REMOVE__@asawicki.info
// Version: 1.0, 2017-03-09
// License: Public Domain


// This simple Windows console C program demostrates how to enumerate and change
// display settings using Windows API.
//
// It uses function EnumDisplayDevices to enumerate all available adapters and
// display devices. Their parameters are printed to standard output.

// If CHANGE_MODE is defined as nonzero, it also searches for a specific display
// mode specified in TARGET_* macros. If found, display settings are temporarily
// changed to that mode using function ChangeDisplaySettingsEx. The program then
// waits for any key press and restores default settings.


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h> // for wprintf()
#include <conio.h> // for _getch()
#include <stdlib.h> // for _countof()
#include <assert.h> // for assert()


// Set to 0 to only print available modes.
// Set to 1 to temporarily change display mode to the parameters specified below.
#define CHANGE_MODE           1

#define TARGET_ADAPTER_INDEX  0
#define TARGET_WIDTH          1024
#define TARGET_HEIGHT         768
#define TARGET_FREQUENCY_MIN  59
#define TARGET_FREQUENCY_MAX  60


static void PrintDisplayDeviceStateFlags(DWORD stateFlags)
{
    if(stateFlags & DISPLAY_DEVICE_ACTIVE)
        wprintf(L" ACTIVE");
    if(stateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)
        wprintf(L" MIRRORING_DRIVER");
    if(stateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)
        wprintf(L" MODESPRUNED");
    if(stateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
        wprintf(L" PRIMARY_DEVICE");
    if(stateFlags & DISPLAY_DEVICE_REMOVABLE)
        wprintf(L" REMOVABLE");
    if(stateFlags & DISPLAY_DEVICE_VGA_COMPATIBLE)
        wprintf(L" VGA_COMPATIBLE");
}

static void PrintDisplayDeviceMode(const DEVMODE* mode)
{
    wprintf(L"%u x %u, %u%s Hz, %u bpp",
        mode->dmPelsWidth,
        mode->dmPelsHeight,
        mode->dmDisplayFrequency,
        (mode->dmDisplayFrequency <= 1 ? L" (Default)" : L""),
        mode->dmBitsPerPel);
    
    // Somehow this flag is documented but not exists in header.
    //if(mode->dmDisplayFlags & DM_GRAYSCALE)
    //    wprintf(L" GRAYSCALE");
    if(mode->dmDisplayFlags & DM_INTERLACED)
        wprintf(L" INTERLACED");
}

int main()
{
    wchar_t targetDeviceName[32] = { 0 };
    DEVMODE targetMode = { 0 };
    BOOL targetModeFound = FALSE;

    DISPLAY_DEVICE adapter = { sizeof(DISPLAY_DEVICE) };
    DWORD adapterIndex = 0;
    while(EnumDisplayDevices(NULL, adapterIndex, &adapter, 0))
    {
        wprintf(L"Adapter %u:\n", adapterIndex);

        wprintf(L"\tDeviceName = %s\n", adapter.DeviceName);
        wprintf(L"\tDeviceString = %s\n", adapter.DeviceString);

        wprintf(L"\tStateFlags =");
        PrintDisplayDeviceStateFlags(adapter.StateFlags);
        wprintf(L"\n");

        DISPLAY_DEVICE displayDevice = { sizeof(DISPLAY_DEVICE) };
        DWORD displayDeviceIndex = 0;
        while(EnumDisplayDevices(adapter.DeviceName, displayDeviceIndex, &displayDevice, 0))
        {
            wprintf(L"\tDisplay Device %u:\n", displayDeviceIndex);

            wprintf(L"\t\tDeviceName = %s\n", displayDevice.DeviceName);
            wprintf(L"\t\tDeviceString = %s\n", displayDevice.DeviceString);

            wprintf(L"\t\tStateFlags =");
            PrintDisplayDeviceStateFlags(displayDevice.StateFlags);
            wprintf(L"\n");

            ++displayDeviceIndex;
        }

        DEVMODE mode = { 0 };
        mode.dmSize = sizeof(DEVMODE);

        if(EnumDisplaySettings(adapter.DeviceName, ENUM_CURRENT_SETTINGS, &mode))
        {
            wprintf(L"\tDisplay Mode Current: ");
            PrintDisplayDeviceMode(&mode);
            wprintf(L"\n");
        }

        if(EnumDisplaySettings(adapter.DeviceName, ENUM_REGISTRY_SETTINGS, &mode))
        {
            wprintf(L"\tDisplay Mode Registry: ");
            PrintDisplayDeviceMode(&mode);
            wprintf(L"\n");
        }

        DWORD modeIndex = 0;
        while(EnumDisplaySettings(adapter.DeviceName, modeIndex, &mode))
        {
            wprintf(L"\tDisplay Mode %u: ", modeIndex);
            PrintDisplayDeviceMode(&mode);
            wprintf(L"\n");

            if(!targetModeFound && adapterIndex == TARGET_ADAPTER_INDEX)
            {
                if(CHANGE_MODE &&
                    mode.dmPelsWidth == TARGET_WIDTH &&
                    mode.dmPelsHeight == TARGET_HEIGHT &&
                    mode.dmDisplayFrequency >= TARGET_FREQUENCY_MIN &&
                    mode.dmDisplayFrequency <= TARGET_FREQUENCY_MAX)
                {
                    wcscpy_s(targetDeviceName, _countof(targetDeviceName), adapter.DeviceName);
                    targetMode = mode;
                    targetModeFound = TRUE;
                }
            }

            ++modeIndex;
        }

        ++adapterIndex;
    }

    if(targetModeFound)
    {
        // Change display mode.
        LONG r = ChangeDisplaySettingsEx(targetDeviceName, &targetMode, NULL, CDS_FULLSCREEN, NULL);
        assert(r == DISP_CHANGE_SUCCESSFUL);
            
        // Wait for key in console.
        _getch();

        // Restore default display mode.
        r = ChangeDisplaySettingsEx(targetDeviceName, NULL, NULL, 0, NULL);
        assert(r == DISP_CHANGE_SUCCESSFUL);
    }

    return 0;
}
