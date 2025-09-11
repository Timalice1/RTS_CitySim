# RTS_HUD_Library

> Function library with static helper functions, serving to simplify game-UI communication. Works through RTS_HUD class manager.

### Functions:

```c++
URTS_HUD_Library::PushNotification(APlayerController* player, const FString& InMessage, const FName &Verbosity = FName("System"));
```
Push notification to the Modals layer
- **_player_** - owning player controller, that holds HUD class.
- **_FString Message_** - notification message.
- **_FName_** Verbosity - verbosity level of the notification. Available 3 verbosity levels:
  - **System** - default system message
  - **Success** - Success message verbosity
  - **Warning** - warning/allert message verbosity

```c++
    SomeAnotherFunction(const FName param1, const FName param 2);
```
