# samp-keys.inc

[![SA-MP](https://img.shields.io/badge/SA--MP-0.3.7--R2+-blue.svg)](https://sa-mp.com/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Version](https://img.shields.io/badge/version-1.0.0-orange.svg)](https://github.com/yourusername/samp-keys)

A professional and dynamic key sequence minigame system for SA-MP servers. Create engaging quick-time events where players must press the correct sequence of keys within a time limit.

## 📋 Features

- ✅ **Dynamic Key Sequences** - Randomly generated key combinations
- ✅ **Customizable Timing** - Set your own time limits
- ✅ **Visual Feedback** - Color-coded on-screen display showing progress
- ✅ **Audio Feedback** - Success and failure sound effects
- ✅ **Hook System** - Compatible with other includes using ALS hooking
- ✅ **Callback Support** - Handle success/failure with custom callbacks
- ✅ **Easy Integration** - Simple API with minimal setup required

## 🎮 Available Keys

The system uses 7 basic SA-MP keys:

| Key | Description | SA-MP Constant |
|-----|-------------|----------------|
| **N** | No | `KEY_NO` |
| **Y** | Yes | `KEY_YES` |
| **F** | Enter/Action | `KEY_SECONDARY_ATTACK` |
| **2** | Submission | `KEY_SUBMISSION` |
| **ALT** | Walk | `KEY_WALK` |
| **H** | Handbrake | `KEY_HANDBRAKE` |

## 📦 Installation

1. Download `samp-keys.inc`
2. Place it in your `pawno/include` folder
3. Add to your gamemode:

```pawn
#include <samp-keys>
```

## 🚀 Quick Start

### Basic Usage

```pawn
CMD:lockpick(playerid, params[]) {
    CreateKeyGame(playerid, 10, 5); // 10 seconds, 5 keys
    return 1;
}

public OnKeyGameFinish(playerid, bool:success) {
    if(success) {
        SendClientMessage(playerid, -1, "You picked the lock!");
        // Give reward, unlock door, etc.
    }
    else {
        SendClientMessage(playerid, -1, "You failed to pick the lock!");
        // Apply penalty, alert guards, etc.
    }
    return 1;
}
```

### Advanced Usage with Variables

```pawn
new PlayerActivity[MAX_PLAYERS];

CMD:rob_car(playerid, params[]) {
    if(PlayerActivity[playerid] != 0) 
        return SendClientMessage(playerid, -1, "You're already doing something!");
    
    CreateKeyGame(playerid, 15, 6); // 15 seconds, 6 keys
    PlayerActivity[playerid] = 1; // Rob car activity
    return 1;
}

CMD:hack_atm(playerid, params[]) {
    if(PlayerActivity[playerid] != 0) 
        return SendClientMessage(playerid, -1, "You're already doing something!");
    
    CreateKeyGame(playerid, 20, 8); // 20 seconds, 8 keys (harder)
    PlayerActivity[playerid] = 2; // Hack ATM activity
    return 1;
}

public OnKeyGameFinish(playerid, bool:success) {
    if(!success) {
        SendClientMessage(playerid, -1, "You failed the minigame!");
        PlayerActivity[playerid] = 0;
        return 1;
    }
    
    switch(PlayerActivity[playerid]) {
        case 1: { // Rob car
            SendClientMessage(playerid, -1, "You successfully stole the car!");
            GivePlayerMoney(playerid, 5000);
        }
        case 2: { // Hack ATM
            SendClientMessage(playerid, -1, "You hacked the ATM!");
            GivePlayerMoney(playerid, 10000);
        }
    }
    
    PlayerActivity[playerid] = 0;
    return 1;
}
```

## 📖 API Reference

### Functions

#### CreateKeyGame
Creates a new key sequence minigame for a player.

```pawn
CreateKeyGame(playerid, timeSeconds, sequenceLength = 4)
```

**Parameters:**
- `playerid` - The ID of the player
- `timeSeconds` - Time limit in seconds (how long the player has to complete)
- `sequenceLength` - Number of keys in the sequence (default: 4, max: 8)

**Returns:**
- `1` on success
- `0` on failure

**Example:**
```pawn
CreateKeyGame(playerid, 10, 5); // 10 seconds, 5 keys
```

---

#### CancelKeyGame
Cancels an active key game for a player.

```pawn
CancelKeyGame(playerid)
```

**Parameters:**
- `playerid` - The ID of the player

**Returns:**
- `1` if game was cancelled
- `0` if player wasn't in a game

**Example:**
```pawn
if(GetPlayerWantedLevel(playerid) > 0) {
    CancelKeyGame(playerid);
    SendClientMessage(playerid, -1, "Minigame cancelled - you're wanted!");
}
```

---

#### IsPlayerInKeyGame
Checks if a player is currently in a key game.

```pawn
IsPlayerInKeyGame(playerid)
```

**Parameters:**
- `playerid` - The ID of the player

**Returns:**
- `1` if player is in a game
- `0` if player is not in a game

**Example:**
```pawn
if(IsPlayerInKeyGame(playerid)) {
    return SendClientMessage(playerid, -1, "Finish the minigame first!");
}
```

### Callbacks

#### OnKeyGameFinish
Called when a player finishes (or fails) a key game.

```pawn
public OnKeyGameFinish(playerid, bool:success)
```

**Parameters:**
- `playerid` - The ID of the player
- `success` - `true` if completed successfully, `false` if failed

**Example:**
```pawn
public OnKeyGameFinish(playerid, bool:success) {
    if(success) {
        // Player completed the sequence
    }
    else {
        // Player failed (wrong key or time ran out)
    }
    return 1;
}
```

## ⚙️ Configuration

You can customize these values by defining them **before** including the file:

```pawn
#define MAX_KEY_SEQUENCES       10    // Max simultaneous games
#define MAX_SEQUENCE_LENGTH     8     // Max keys per sequence
#define KEY_GAME_UPDATE_RATE    100   // Update rate in milliseconds

#include <samp-keys>
```

## 🎨 Visual Display

The system shows:
- **Green [KEY]** - Already pressed correctly
- **Yellow [KEY]** - Current key to press
- **White [KEY]** - Upcoming keys
- **Red timer** - Remaining time in seconds

## 🔊 Audio Feedback

- **Sound 1083** - Correct key pressed
- **Sound 1085** - Wrong key pressed

## 💡 Use Cases

Perfect for:
- 🚗 Vehicle theft/lockpicking systems
- 🏦 ATM/safe hacking minigames
- 🔓 Door lockpicking mechanics
- 💣 Bomb defusal sequences
- 🎮 Quick-time events
- 🏃 Escape sequences
- 🔧 Repair/crafting systems
- 🎯 Skill-based challenges

## 🛡️ Error Handling

The system includes built-in protection:
- Prevents multiple simultaneous games per player
- Validates sequence length (1-7 keys)
- Automatic cleanup on player disconnect
- Safe timer management
- Player controllability restoration

## 🔧 Technical Details

- **Memory:** Static player arrays for optimal performance
- **Hooks:** ALS (Alternative Logic System) compatible
- **Timers:** Automatic cleanup and management
- **TextDraws:** Dynamic creation and destruction

## 📝 Requirements

- SA-MP 0.3.7 or higher
- Basic knowledge of Pawn scripting

## 🤝 Contributing

Contributions are welcome! Feel free to:
- Report bugs
- Suggest features
- Submit pull requests
- Improve documentation

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.


## 📞 Support

If you encounter any issues or have questions:
- Open an issue on GitHub
- Contact on discord: leeeap
- Join our Discord server

---

**⭐ If you find this useful, please star the repository!**
