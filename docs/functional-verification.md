# Wokwi Functional Verification

## Build and Circuit Checks

- `pio run`: PASS
- `diagram.json` parses as valid JSON: PASS
- Required simulated parts are present: Blue Pill, DHT22, LDR, PIR, SSD1306,
  KY-040 encoder, and buzzer: PASS
- Firmware and ELF paths in `wokwi.toml` point to the Blue Pill build: PASS

Run interactive tests with **Wokwi: Start Simulator** in VS Code. This audit
cannot control the simulator; it records the current-firmware terminal output
reported by the user. Only behavior directly present in that output is recorded
below, and the remaining FT stimuli stay partial or pending.

Before testing, record the firmware revision (`git log -1 --oneline`) and date.
Keep the simulator terminal and circuit visible. Except when testing inactivity,
trigger the PIR as needed to keep the system ACTIVE. The DHT22 is sampled every
2 seconds; allow one sample period after changing its controls.

## Functional Test Record

IDs below follow the laboratory PDF. FT-01 to FT-03 have sample-value smoke
observations but no controlled input changes; FT-09 has a reported state
transition. Other cases remain pending until their specific stimulus is
observed on this firmware.

| ID | Requirement / stimulus | Expected result | Actual observation | Result |
| --- | --- | --- | --- | --- |
| FT-01 | Change DHT22 temperature | Temperature updates within 2 s | Sample output showed 24.00 C; a controlled change was not recorded | Partial |
| FT-02 | Change DHT22 humidity | Humidity updates within 2 s | Sample output showed 40.00%; a controlled change was not recorded | Partial |
| FT-03 | Move LDR from dark to bright | Light remains 0-100%; bright is higher | Sample output showed 76%; dark/bright comparison not recorded | Partial |
| FT-04 | Rotate encoder clockwise | Next page is selected | Not observed in the supplied current-firmware log | Pending |
| FT-05 | Rotate encoder counterclockwise | Previous page is selected | Not observed in the supplied current-firmware log | Pending |
| FT-06 | Set temperature above 30 C | Alarm activates | Not observed in the supplied current-firmware log | Pending |
| FT-07 | Return temperature to normal | Alarm stops | Not observed in the supplied current-firmware log | Pending |
| FT-08 | Trigger PIR while ACTIVE | Motion is detected; system remains ACTIVE | Current sample log showed no motion; PIR trigger not recorded | Pending |
| FT-09 | Leave PIR clear for 15 s | System becomes INACTIVE | Terminal printed `STATE: INACTIVE (no motion for 15 s)` after the no-motion interval | PASS (reported observation; exact stopwatch time not recorded) |
| FT-10 | Trigger PIR while INACTIVE | System returns ACTIVE and sensing/display resume | Not observed in the supplied current-firmware log | Pending |

## Manual Procedure

1. Run `pio run`.
2. Start **Wokwi: Start Simulator** in VS Code.
3. Keep the Wokwi Terminal visible with the circuit.
4. Record the tested firmware revision and date; verify task startup and OLED
   initialization before functional checks.
5. FT-01/FT-02: change DHT22 temperature and humidity separately; verify the
   next serial sample and corresponding OLED page reflect each changed value.
6. FT-03: compare dark and bright LDR settings on the LIGHT page; verify both
   values stay in 0-100% and brighter produces a higher percentage.
7. FT-04/FT-05: verify clockwise and counterclockwise encoder selection,
   including wraparound.
8. FT-06/FT-07: while ACTIVE, test above 30 C and return to the normal range;
   verify the terminal messages and buzzer response.
9. FT-08: trigger PIR while ACTIVE and verify motion is reported.
10. FT-09: leave PIR clear for 15 s and verify the INACTIVE transition.
11. FT-10: trigger PIR while INACTIVE; verify ACTIVE transition, OLED return,
    resumed sampling, and encoder response.
12. Replace each partial/pending observation only with actual behavior and
    mark PASS/FAIL/PARTIAL. Do not mark PASS without observing it.

For every test, preserve the observation text and use the same tested firmware
revision; results from another repository or an earlier build are not evidence
for this project.