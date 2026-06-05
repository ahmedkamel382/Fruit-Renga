# 2D Point-and-Click Fruit Drop Game

A fast-paced, reflex-based 2D arcade game built with C++ and the Simple DirectMedia Layer (SDL2) library. The objective is to click on falling fruits to earn points while avoiding bombs and managing your health. The game includes dynamic entity spawning, real-time event polling, and persistent file-based high-score tracking.

## ✨ Features

* **Dynamic Gameplay Mechanics:** Objects continuously spawn at randomized X-coordinates and fall at a constant velocity. Missing normal fruits or clicking bombs reduces health, while special items grant bonus points or restore health.
* **Interactive UI & States:** Fully functional game states including a Main Menu, active gameplay, a Pause overlay, and a Game Over screen with a "Try Again" loop.
* **Audio Mixing:** Integrated background music for different game states (Menu, Gameplay, Game Over) and distinct sound effects for scoring, taking damage, and picking up special items.
* **Persistent Data Tracking:** Reads and writes high scores to a local text file (`highscore.txt`) to maintain player records across sessions.
* **Real-Time Rendering:** Utilizes hardware-accelerated rendering for background textures, animated sprite textures, and dynamic TrueType font (TTF) text overlays.

## 🛠️ Technology Stack

* **Language:** C++
* **Graphics & Windowing:** [SDL2](https://www.libsdl.org/) (Simple DirectMedia Layer)
* **Image Loading:** SDL2_image (supports PNG textures)
* **Audio Management:** SDL2_mixer (supports MP3 background music and WAV sound effects)
* **Font Rendering:** SDL2_ttf (renders dynamic score and health text)

## 🎮 Controls

* **Left Mouse Button:** Click on falling items to interact with them.
* **'P' Key:** Pause / Resume the game.

## 🍎 Game Items
* **Apples, Bananas, Oranges:** Click to earn +1 point. Letting them hit the bottom of the screen costs 1 health.
* **Bomb:** DO NOT CLICK! Clicking a bomb reduces health by 1.
* **Golden Fruit:** Click to earn a +5 point bonus.
* **Health Potion:** Click to restore +3 health.

## 🚀 Getting Started

### Prerequisites
* A C++ compiler (e.g., MSVC, GCC, or Clang).
* The SDL2, SDL2_image, SDL2_ttf, and SDL2_mixer development libraries installed and linked to your build environment.

### Installation & Execution
1. Clone the repository.
2. Ensure your project is configured to link the required SDL2 libraries.
3. Place the necessary assets (images in a `Pictures/` folder, fonts in a `Fonts/` folder, and audio in a `Sounds/` folder) relative to the executable.
4. Compile and run the application.