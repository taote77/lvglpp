# smart_speaker — 360×360 智能音响 / 运动表盘 demo

A lvglpp Widget demo derived from the smart-speaker ESP32 UI. The same round
face also works as a sports-watch face; the demo is named after the
smart-speaker use case:

- a round **living face** that mirrors the voice-assistant state
  (`Init / Idle / Listening / Thinking / Speaking / Error`)
- a **blow-out-the-candles** game on the second page
- touch/mouse: tap the face to pet it, `PLAY` to open the game, `<` to return

The face is drawn with plain LVGL objects (no bitmap assets), so the demo runs
anywhere lvglpp runs and is cheap enough for an ESP32-S3.

## Build and run

From the repository root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target smart_speaker -j$(nproc)
cd build/target/bin
./smart_speaker
```

On a desktop window larger than 360×360 the panel is centred, with a title,
the simulated state and a short hint around it. On a 360×360 display the panel
fills the screen and the captions are omitted.

## What it demonstrates

- `widgets::TileView` for two full-screen pages
- `widgets::Widget`/`Label` for a vector face, ring, candles and confetti
- `core::sys::Timer` for 20 Hz animation and auto state cycling
- `Widget::onClicked` / `onPressed` / `onReleased` for touch interaction
- `WidgetStore` lifetime for callback-bearing widgets
- reusable `FacePage` / `CandlePage` pages and a `FacePanel` that composes
  them for the standalone demo

The demo simulates the voice state every 3 seconds and a microphone level
while `Listening` is active. The candle game can be played with the
`BLOW / HOLD` button; on a device, a real breath above the mic threshold also
works.

## Reuse on ESP32

The two pages are independent of the demo activity. Host them directly in an
existing `TileView` (this is what the smart-speaker app does; the sync script
copies these files into its `main/smart_speaker_ui/`):

```cpp
#include "CandlePage.h"
#include "FacePage.h"

auto face   = std::make_unique<lvglpp::demo::FacePage>(homeTile);
auto candle = std::make_unique<lvglpp::demo::CandlePage>(candleTile);

face->setOnPlay([&] { tileview->setTileByIndex(1, 0, LV_ANIM_ON); });
candle->setOnBack([&] { tileview->setTileByIndex(0, 0, LV_ANIM_ON); });

face->setVoiceState(lvglpp::demo::VoiceState::Listening);
candle->setVoiceState(lvglpp::demo::VoiceState::Listening);
face->setMicLevel(voice::mic_level());   // 0..100, UI thread
candle->setMicLevel(voice::mic_level());
face->setTime("12:34");
face->setStatus("WiFi: 192.168.1.15");

face->setActive(homeActive);       // pauses hidden animation timers
candle->setActive(candleActive);
```

`setMicLevel()` is called from the UI thread with a smoothed 0..100 loudness
value. The face uses it for the Listening ring; the candle page uses it for
blow detection. The game also accepts a held button, so it remains playable
without a microphone.

If you want a self-contained 360×360 panel with its own two-page navigation,
use `FacePanel` instead:

```cpp
auto face = std::make_unique<lvglpp::demo::FacePanel>(activityRoot);
face->setVoiceState(lvglpp::demo::VoiceState::Listening);
face->setMicLevel(voice::mic_level());
face->setTime("12:34");
face->setStatus("WiFi: 192.168.1.15");
```

The smart-speaker port note: its CST816S panel reports raw coordinates in
0..359 directly. Do **not** scale them by 360/240, or taps on the bottom
buttons will land off target.

## Source layout

- `FaceTypes.h` — `VoiceState`
- `FacePage.h/.cc` — living face page; `setVoiceState`, `setMicLevel`,
  `setTime`, `setStatus`, `setActive`, `setOnPlay`
- `CandlePage.h/.cc` — candle game page; `setVoiceState`, `setMicLevel`,
  `setActive`, `setOnBack`
- `FacePanel.h/.cc` — TileView composition of the two pages
- `SmartSpeakerActivity.h/.cc` — demo host, state cycling and clock
- `main.cc` — creates the lvglpp Application and pushes the activity
