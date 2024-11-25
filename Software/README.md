# Software Guide

This README provides instructions on setting up the development environment, programming, and debugging the **Inercia Floppy Badge**.

------

## Development Environment

The development environment is built around [PlatformIO](https://platformio.org), which allows for efficient dependency management and provides greater control over the build process compared to the Arduino IDE. Is it perfect? No. But PlatformIO offers (what seems to us) the best balance of usability, distribution ease, and customization. If you have suggestions for improvement, we’re always open to feedback!

### Setting Up PlatformIO

To get started, install PlatformIO following the official [installation guide](https://platformio.org/install/). You can use:

- **PlatformIO VS Code Extension** for a graphical interface.

- **PlatformIO CLI** for command-line workflows.

> ##### Note: Additional steps when using Windows
> Windows has a default limitation on file path lengths (260 characters), which **WILL** cause issues when working with the PlatformIO project. To avoid errors:
>
> - Enable long file path support in Windows:
>
>   - Open the **Local Group Policy Editor** (`gpedit.msc`).
>
>   - Navigate to `Computer Configuration > Administrative Templates > System > Filesystem`.
>
>   - Enable the policy **Enable Win32 long paths**.
>
>   - Reboot

------

## Accessing the Codebase

The main firmware is located in the [`Software/inercia_badge_sw_pio`](https://github.com/afonsus1997/Inercia-floppy-badge/tree/main/Software/inercia_badge_sw_pio) directory of the repository. This is the most up-to-date source code for the badge.

To begin:

1. Open the project in PlatformIO.
2. Allow PlatformIO to scan the project files.

------

## Build Environments

Once the project is loaded, you’ll find two build environments in the `platformio.ini` file:

1. **`env:pico_usb`**
   - Use this for flashing the badge firmware via USB.
2. **`env:pico_cmsis`**
   - Select this if you are using a debug probe. [More info here](#).

> ##### Note: (more) Additional steps when using Windows
> For the first **USB** firmware upload, you need to plug in the badge with the BOOTL button pressed so that it shows up as UF2 drive.
> Then, use Zadig to change the driver of the RP2040 to WinUSB.