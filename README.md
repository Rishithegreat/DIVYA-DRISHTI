Divya Drishti — Interactive Braille Learning Device

> *"Divya Drishti" (दिव्य दृष्टि) — Sanskrit for "Divine Vision"*

An affordable, multi-sensory Braille learning device that combines servo-actuated tactile feedback with synchronized audio guidance to help visually impaired students learn the Braille alphabet independently.

---

# Project Background

**Event:** ARTEMIS Hackathon 2026
**Team:** Team Curiosity
**Category:** Hardware
**Problem Statement:** Affordable and interactive Braille learning tools are limited, making independent education difficult for visually impaired students.

---

The Problem

Most existing Braille learning devices are expensive, require trained supervision, and are inaccessible to students in low-income families or resource-limited special schools. Divya Drishti was built to change that — targeting a sub-₹5,000 price point while delivering a fully interactive, self-paced learning experience.

---

 
##How It Works

1. The user navigates letters A–Z using two physical buttons (Next / Previous).
2. The **ESP32 microcontroller** activates up to 6 servo motors, each corresponding to one of the six Braille dots.
3. Servos raise or lower their pins to physically form the correct Braille character.
4. Simultaneously, the **DFPlayer Mini** audio module plays the letter's name aloud through a speaker.
5. The system waits for the user to feel the pattern, then resets when they navigate to the next letter.

---

##  Hardware

| Component | Purpose |
|---|---|
| ESP32 (38-pin) | Main microcontroller |
| 6× Micro Servo Motors | Braille dot actuation (Dots 1–6) |
| DFPlayer Mini | MP3 audio playback |
| MicroSD Card | Stores 27 audio files (A–Z + intro) |
| Speaker | Audio output |
| 2× Push Buttons | Next / Previous navigation |

### Pin Mapping

| Signal | ESP32 Pin |
|---|---|
| Button NEXT | GPIO 13 |
| Button PREV | GPIO 14 |
| Servo Dot 1 (Top Left) | GPIO 4 |
| Servo Dot 2 (Middle Left) | GPIO 5 |
| Servo Dot 3 (Bottom Left) | GPIO 26 |
| Servo Dot 4 (Top Right) | GPIO 25 |
| Servo Dot 5 (Middle Right) | GPIO 33 |
| Servo Dot 6 (Bottom Right) | GPIO 32 |
| DFPlayer RX | GPIO 16 |
| DFPlayer TX | GPIO 17 |

### Braille Dot Layout

```
  Left  Right
    1  •  • 4
    2  •  • 5
    3  •  • 6
```

---

## 🖥️ Software

**Language:** C++ (Arduino Framework)
**Board:** ESP32 (Arduino IDE)

### Dependencies

Install the following libraries via the Arduino Library Manager:

| Library | Version |
|---|---|
| `ESP32Servo` | Latest |
| `DFRobotDFPlayerMini` | Latest |

### Audio Files (SD Card Setup)

Place 27 MP3 files in the root of the MicroSD card:

```
0001.mp3  →  Letter A
0002.mp3  →  Letter B
...
0026.mp3  →  Letter Z
0027.mp3  →  Introduction / welcome audio
```

---

## 🚀 Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/<your-username>/divya-drishti.git
cd divya-drishti
```

### 2. Install Dependencies

Open Arduino IDE and install the required libraries:
- **ESP32Servo** — by Kevin Harrington
- **DFRobotDFPlayerMini** — by DFRobot

### 3. Prepare the SD Card

Format a MicroSD card as FAT32 and copy your 27 numbered MP3 audio files to the root directory.

### 4. Flash the Firmware

Open `divya_drishti1.ino` in Arduino IDE, select your **ESP32** board and port, then upload.

### 5. Serial Monitor

Open the Serial Monitor at **115200 baud** to view real-time system logs and navigation events.

---

## 🎛️ Configuration

Key settings can be tuned in the `Settings` namespace at the top of the sketch:

```cpp
namespace Settings {
    const int VOLUME_LEVEL         = 25;    // DFPlayer volume (0–30)
    const int SERVO_MOVEMENT_DELAY = 500;   // ms for servos to reach position
    const int AUDIO_DISPLAY_DELAY  = 2000;  // ms to hold pattern before accepting input
    const int DEBOUNCE_DELAY       = 250;   // Button debounce time in ms
    const int INTRO_AUDIO_DURATION = 4000;  // Intro audio length in ms
}
```

Servo up/down angles are individually calibrated per motor in the `ServoAngles` namespace and may require adjustment based on your physical assembly.

---

## 📂 Project Structure

```
divya-drishti/
├── divya_drishti1.ino   # Main firmware sketch
├── README.md            # This file
└── sd_card/
    ├── 0001.mp3         # Audio for 'A'
    ├── 0002.mp3         # Audio for 'B'
    ├── ...
    └── 0027.mp3         # Introduction audio
```

---

## 🌍 Impact

Divya Drishti directly addresses the UN Sustainable Development Goal **#4 — Quality Education** by providing an affordable, inclusive learning tool for visually impaired students.

**Who benefits:**
- Blind primary school students learning basic Braille alphabets
- Students in rural areas with limited access to assistive technology
- Children from low-income families who cannot afford commercial Braille devices
- NGOs and rehabilitation centres training blind students
- Newly visually impaired learners beginning their Braille education

---

## 🔮 Future Scope

- Mobile app integration for learning progress tracking
- Expansion to words, sentences, and Braille contractions
- Multi-language audio support (Hindi, regional languages)
- AI-based adaptive learning modes
- Cloud data storage for teacher monitoring
- Integration with school inclusive education programmes

---

## ⚠️ Known Challenges

- Mechanical wear on servo motors with heavy use — addressed by using quality servos and a modular/replaceable pin design
- Accurate tactile dot height requires per-unit servo calibration
- Battery life management for portable use

---

## 📚 References

- *Electronic Device for Learning Braille Reading/Writing System* — Springer
- *Refreshable Braille Display using Solenoids and Microcontroller* — Arduino-based prototype research
- *Low-Cost Refreshable Braille Display Design & Development* — Electromagnetic actuator-based tactile interface research

---

## 📄 License

This project is open-source and available under the [MIT License](LICENSE).

---

## 🤝 Contributing

Contributions, issues, and feature requests are welcome! Feel free to open a pull request or file an issue.

---

*Built with ❤️ by Team Curiosity — ARTEMIS Hackathon 2026*
