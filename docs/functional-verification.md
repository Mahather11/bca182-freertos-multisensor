# Wokwi Functional Verification

## Build and Circuit Checks

- `pio run`: PASS
- `diagram.json` parses as valid JSON: PASS
- Required simulated parts are present: Blue Pill, DHT22, LDR, PIR, SSD1306,
  KY-040 encoder, and buzzer: PASS
- Firmware and ELF paths in `wokwi.toml` point to the Blue Pill build: PASS

Run interactive tests with **Wokwi: Start Simulator** in VS Code. The user
reports completing FT-01 through FT-10 on 5 October 2026. The observations below
are the supplied results; the exact tested firmware SHA was not recorded.

Before testing, record the firmware revision (`git log -1 --oneline`) and date.
Keep the simulator terminal and circuit visible. Except when testing inactivity,
trigger the PIR as needed to keep the system ACTIVE. The DHT22 is sampled every
2 seconds; allow one sample period after changing its controls.

## Functional Test Record

IDs below follow the laboratory PDF and list the user's reported outcomes.

| ID | Requirement / stimulus | Expected result | Actual observation | Result |
| --- | --- | --- | --- | --- |
| FT-01 | Change DHT22 temperature | Temperature updates within 2 s | Set temperature to 28.5 C; OLED updated to 28.5 C accurately | PASS |
| FT-02 | Change DHT22 humidity | Humidity updates within 2 s | Set humidity to 55.0% RH; OLED updated to 55.0% accurately | PASS |
| FT-03 | Move LDR from dark to bright | Light remains 0-100%; bright is higher | Varied light level from 10% to 90%; relative percentage displayed correctly | PASS |
| FT-04 | Rotate encoder clockwise | Next page is selected | Pages advanced Temperature -> Humidity -> Light -> Motion | PASS |
| FT-05 | Rotate encoder counterclockwise | Previous page is selected | Page reversed with proper wraparound | PASS |
| FT-06 | Set temperature above 30 C | Alarm activates | At 32.0 C, AlarmTask activated buzzer alert | PASS |
| FT-07 | Return temperature to normal | Alarm stops | At 24.0 C, buzzer silenced and normal state restored | PASS |
| FT-08 | Trigger PIR while ACTIVE | Motion is detected; system remains ACTIVE | PIR motion set the system to ACTIVE | PASS |
| FT-09 | Leave PIR clear for 15 s | System becomes INACTIVE | After 15 seconds idle, system entered INACTIVE and blanked OLED | PASS |
| FT-10 | Trigger PIR while INACTIVE | System returns ACTIVE and sensing/display resume | PIR trigger reactivated ACTIVE and restored OLED | PASS |

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
12. After any firmware change, rerun the affected cases and record the tested
   revision, actual behavior, and PASS/FAIL/PARTIAL result. Do not carry results
   forward to a different firmware revision without retesting.

For every test, preserve the observation text and use the same tested firmware
revision; results from another repository or an earlier build are not evidence
for this project.