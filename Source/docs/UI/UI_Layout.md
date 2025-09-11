# UI Layout
UI layout structure are presented using Facade pattern. Architecture are maded from a 4 main classes, and use HUD actor class to manage them. Game logic are not communicate directly with a UI layers, using HUD manager class for this. Additionally, to simplify game-ui communication, RTS_HUD_Lib function library are used, providing a static helper function to access and communicate with UI.

## UI layers:

| **Layer**            | **Description**                                                             |
|----------------------|-----------------------------------------------------------------------------|
| **Game menus layer** | Handle and manage in-game menus panels and selectors                        |
| **Game layer**       | Main HUD layer, for an elements that always visible on the screen           |
| **Menu layer**       | Handles and manages high-level menus (pause menu, main menu, settings etc.) |
| **Modals layer**     | Notifications, popups, confirmation windows etc                             |

> There is no actual limit for UI layers. This 4 presented layers are the main ones, that
> more than enough for the most cases. However, there is no denial to add some new layer
> when it's required, just simple create new UUserWidget instance for a new layer
> and add it onto RTS_HUD class as a property, and initialize in ARTS_HUD::InitLayers();

## RTS_HUD

UI manager class. Handle and manage UI Layers. Main communication bridge between game and UI.
RTS_HUD class holds all UI layers, and give access to them for game-ui communication through functions forward declaration (usage of a Facade pattern). Additionally, class gives a getters for these layers for direct access to public functions and methods in this layer.

## RTS_HUD_Library
Helper function library with a static helper functions, serving to simplify game-UI communocation. See more in ./docs/UI/RTS_HUD_Library.md
