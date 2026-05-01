# PakKit

Reusable UI and utility components for [NextUI](https://github.com/LoveRetro/NextUI) pak development on TrimUI handheld devices.

Header-only C libraries built on top of [Apostrophe](https://github.com/Helaas/Apostrophe). Drop them into your project and go.

## Components

### pakkit_ui.h

Custom UI components with a minimal, consistent style.

- **pakkit_menu** -- selection menu with highlighted cursor, title, divider
- **pakkit_detail_screen** -- scrollable info screen with key/value pairs and credits
- **pakkit_draw_hints** -- minimal text hints replacing the standard footer

### Coming Soon

- **pakkit_list** -- scrollable list with secondary/tertiary actions
- **pakkit_keyboard** -- customizable on-screen keyboard
- **pakkit_net** -- embedded web server, QR code setup, fetch helpers

## Usage

PakKit is header-only. Add the include directory to your project and include with the implementation define in one C file:

    #define PAKKIT_UI_IMPLEMENTATION
    #include "pakkit_ui.h"

Requires Apostrophe to be initialized before calling any PakKit functions.

### Menu Example

    pakkit_menu_item items[] = {
        {.label = "Option 1" },
        {.label = "Option 2" },
        {.label = "Option 3" },
    };

    pakkit_menu_result result;
    int rc = pakkit_menu("My Menu", items, 3, &result);
    if (rc == AP_OK) {
        // result.selected_index has the selection
    }

### Detail Screen Example

    pakkit_info_pair info[] = {
        {.key = "Version",.value = "1.0.0" },
        {.key = "Author",.value = "Your Name" },
    };

    const char *credits[] = {
        "My App by Your Name",
        "Built with Apostrophe by Helaas",
        "For NextUI by LoveRetro",
    };

    pakkit_detail_opts opts = {.title = "My App",.subtitle = "A cool NextUI pak",.info = info,.info_count = 2,.credits = credits,.credit_count = 3,
    };
    pakkit_detail_screen(&opts);

## Integration

Add PakKit as a git submodule:

    git submodule add https://github.com/ericreinsmidt/pakkit.git third_party/pakkit

Add to your Makefile CFLAGS:

    -I$(WORKSPACE)/third_party/pakkit/include

## Dependencies

- [Apostrophe](https://github.com/Helaas/Apostrophe) UI toolkit (must be initialized first)
- SDL2, SDL2_ttf (provided by the NextUI toolchain)

## License

MIT

## Credits

- **PakKit** by Eric Reinsmidt
- Built on [Apostrophe](https://github.com/Helaas/Apostrophe) by [Helaas](https://github.com/Helaas)
- For [NextUI](https://github.com/LoveRetro/NextUI) by [LoveRetro](https://github.com/LoveRetro)
