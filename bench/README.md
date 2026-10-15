# The bench track

An Arduino Nano and two sensors in place of the camera. The bench swaps the sensor and nothing else: `table.js`, `piece.js` and `fake-sense.js` stay exactly as they are. How to switch over is in the main README, under **Bench pairs**.

**Simple sensor first.** Every pair builds with sensors from the shelf below, the simple tier, and gets them working in the browser before anything else. An advanced sensor is a second sensor, added after the simple one works, never instead of it.

## Building it

The kits come apart, so building the circuit is part of your work.

1. A Nano, a breadboard and jumpers from the north racks in the Innovation Commons. If the Nano's header pins aren't soldered yet, that comes first. Ask Mr. Reasy before you heat an iron.
2. Two sensors on the one Nano, one per partner. Wire each one and check it in the Arduino IDE's Serial Monitor at 115200 before the browser sees it.
3. Add your sensor's number to the line `bench.ino` prints (`Serial.print(" light "); Serial.print(analogRead(A0));`), then its function in `SIGNALS` in `sense-serial.js`, then its name in `table.js`.

## What's on the shelf

| Sensor | A signal it can give | Watch for |
|---|---|---|
| HC-SR04 ultrasonic | `near`, `present` | The worked example in `bench.ino`. Soft clothing swallows the echo. |
| PIR motion | `moving` | On or off, and it holds itself on for a few seconds. Someone standing still disappears. |
| Photoresistor | `shade` | Needs a 10k resistor beside it. The room's light changes through the day. |
| TCRT5000 reflective IR | `close` | A hand within a few centimetres, nothing further. |
| Sound sensor (KY-037/038) | `loud` | Read its analog pin (A0), which gives a level. The digital pin only says over or under its dial. |
| TTP223 touch pad | `touch` | Works through a few millimetres of paper or plastic. |
| Piezo disc | `knock` | It spikes for an instant on a tap, so give its state a hold. |
| Tilt switch | `tilted` | For a piece someone picks up. |

Every sensor here prints a number; the meaning is yours. A `near` of 0.9 is a distance, never a feeling.

## Advanced sensing

Mr. Reasy's own bench sensors, lent to pairs whose simple sensor already works in the browser. Ask him for one; they live on his bench, not the shelf. Several need a 3.3 V board with more memory than the Nano: the Nano 33 IoT (north racks, a few) or an ESP32-S3 from his bench.

| Sensor | A signal it can give | How it talks | Board | Difficulty |
|---|---|---|---|---|
| MPR121 capacitive touch (12 pads) | `touched`, and which pad | I2C | Nano | Moderate. Any conductive thing becomes a pad: foil, fruit, graphite. |
| RC522 RFID reader | `tagged`, and which card | SPI | Nano, 3.3 V power only | Moderate. Each card has its own ID, so a piece can know which object is on it. |
| AS7341 / AS7343 spectral sensor | `colour`, `bright` | I2C | Nano | Moderate. Eight to fourteen colour bands; it needs steady light on the object. |
| BNO080/085 IMU | `tilted`, `turning`, `still` | I2C | Nano 33 IoT or ESP32-S3 | Hard. Gives orientation directly; too big for the Nano's memory. |
| INMP441 microphone | `loud` | I2S | Nano 33 IoT or ESP32-S3 | Hard. A real digital microphone; the Nano has no I2S. |
| HLK-LD2450 radar | `near`, `present`, `count`, where people stand | UART at 256000 baud | Nano 33 IoT | Hard. Tracks up to three people by position, through thin walls and in the dark. |
| Load cell, 10 kg or 15 kg bar | `loaded`, `weight` | Needs an amplifier board (HX711 or NAU7802) | Nano | Moderate once the amplifier is in hand. Ask Mr. Reasy whether one is free before you plan on it. |

Same rule as the shelf: the sensor gives a number and your State Table gives it a meaning. A radar `count` of 2 is two bodies in range, not a crowd that wants anything.

