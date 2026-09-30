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

Before testing, record the firmware revision (`git log -1 --oneline`) and date.
Keep the simulator terminal and circuit visible. Except when testing inactivity,
trigger the PIR as needed to keep the system ACTIVE. The DHT22 is sampled every
2 seconds; allow one sample period after changing its controls.

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
4. Record the tested firmware revision and date; verify all six task-start
   messages and the OLED initialization before functional checks.
5. FT-01/FT-02: change DHT22 temperature and humidity separately; verify the
   next serial sample and corresponding OLED page reflect each changed value.
6. FT-03: compare dark and bright LDR settings on the LIGHT page; verify both
   values stay in 0-100% and brighter produces a higher percentage.
7. FT-04: trigger PIR motion, then wait for its output to clear; compare both
   terminal transitions with the MOTION page.
8. FT-05/FT-06: visit all four pages, then turn the encoder through a complete
   clockwise and counterclockwise cycle; verify single-page display and wrap.
9. FT-07: while ACTIVE, test 30.0 C, 30.1 C, 17.9 C, and 18.0 C; verify the
   threshold behavior in the terminal and buzzer. Keep the system ACTIVE.
10. FT-08/FT-09: stop triggering PIR; observe the 15-second transition, OLED
    blanking, and pause in sensing/encoder activity.
11. FT-10: trigger PIR while INACTIVE; verify ACTIVE transition, OLED return,
    resumed sampling, and encoder response.
12. Replace each pending observation with the actual behavior and mark
    PASS/FAIL/PARTIAL. Do not mark a test PASS without observing it.

For every test, preserve the observation text and use the same tested firmware
revision; results from another repository or an earlier build are not evidence
for this project.