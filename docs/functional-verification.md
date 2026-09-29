# Wokwi Functional Verification

## Build and Circuit Checks

- `pio run`: PASS
- `diagram.json` parses as valid JSON: PASS
- Required simulated parts are present: Blue Pill, DHT22, LDR, PIR, SSD1306,
  KY-040 encoder, and buzzer: PASS
- Firmware and ELF paths in `wokwi.toml` point to the Blue Pill build: PASS

The interactive tests below must be run with **Wokwi: Start Simulator** in VS
Code. This environment does not include the Wokwi CLI, so runtime observations
are intentionally not marked as passed here.

## Functional Test Record

| ID | Stimulus | Expected result | Observation |
| --- | --- | --- | --- |
| FT-01 | Change DHT22 temperature | Temperature updates within 2 s | Pending Wokwi run |
| FT-02 | Change DHT22 humidity | Humidity updates within 2 s | Pending Wokwi run |
| FT-03 | Move LDR from dark to bright | Light remains 0-100%; bright is higher | Pending Wokwi run |
| FT-04 | Simulate PIR motion | `MOTION: detected`; Motion screen shows `DETECTED` | Pending Wokwi run |
| FT-05 | Visit all four encoder modes | OLED shows one selected measurement at a time | Pending Wokwi run |
| FT-06 | Turn encoder clockwise/counterclockwise | Modes wrap Temperature, Humidity, Light, Motion in both directions | Pending Wokwi run |
| FT-07 | Set 30.1 C, then 18.0 C | High alarm activates above 30 C; 18.0 C is normal | Pending Wokwi run |
| FT-08 | Stop PIR activity for 15 s | OLED blanks, sensing and encoder pause | Pending Wokwi run |
| FT-09 | Wait after the last motion | `STATE: INACTIVE (no motion for 15 s)` appears | Pending Wokwi run |
| FT-10 | Trigger PIR while inactive | `STATE: ACTIVE (motion)`; OLED and sensing resume | Pending Wokwi run |

## Manual Procedure

1. Run `pio run`.
2. Start **Wokwi: Start Simulator** in VS Code.
3. Keep the Wokwi Terminal visible with the circuit.
4. Use the DHT22/LDR controls, PIR **Simulate Motion** control, and KY-040
   encoder to execute FT-01 through FT-10.
5. Replace each pending observation with the actual terminal/OLED behavior and
   record PASS, FAIL, or PARTIAL. Do not mark a test PASS without observing it.