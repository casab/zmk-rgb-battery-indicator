# zmk-rgb-battery-indicator

A [ZMK](https://zmk.dev) key behavior that shows the battery level on the RGB LEDs for a moment,
as a gauge.

- `leds` fill in order (list them left to right): all of them at 100%, half at 50%, at least one.
  The gauge fills up with a short animation, then fades out.
- The color says how urgent it is: green from `high-level` (50%) up, orange from `low-level`
  (25%) up, red below.

It is drawn on top of the normal underglow and needs
[zmk-rgb-led-overlay](https://github.com/casab/zmk-rgb-led-overlay).

## Add it to your zmk-config

1. `config/west.yml`: add `zmk-rgb-led-overlay` and this module.

   ```yaml
       - name: zmk-rgb-led-overlay
         remote: casab          # url-base: https://github.com/casab
         revision: main
       - name: zmk-rgb-battery-indicator
         remote: casab
         revision: main
   ```

2. Set up zmk-rgb-led-overlay (see its README), then define the behavior and bind it to a key:

   ```dts
   / {
       behaviors {
           bat_ind: battery_indicator {
               compatible = "zmk,behavior-rgb-battery-indicator";
               #binding-cells = <0>;
               leds = <2 1 3 0 4 5 9 6 8 7>;   // left to right
           };
       };
   };
   ```

   ```dts
   &bat_ind
   ```

Battery reporting must be on (`CONFIG_ZMK_BATTERY_REPORTING`, on by default for Bluetooth
keyboards).

## Properties

| Property | Type | Default | Meaning |
| --- | --- | --- | --- |
| `leds` | array | required | LED indexes in fill order. |
| `brightness` | int | 50 | Brightness 0-100. |
| `high-level` | int | 50 | Battery % from which `high-hue` is used. |
| `low-level` | int | 25 | Battery % from which `mid-hue` is used; below it `low-hue`. |
| `high-hue` | int | 120 | Green. |
| `mid-hue` | int | 30 | Orange. |
| `low-hue` | int | 0 | Red. |
| `duration-ms` | int | 3000 | How long the level stays shown. |
| `fill-ms` | int | 400 | Time for a full gauge to fill up from the first LED to the last. |
| `slot` | int | 2 | zmk-rgb-led-overlay slot; higher slots are drawn over lower ones. |

## License

MIT
