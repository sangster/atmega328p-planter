# Planter

This project manages the water and light cycles of a single houseplant.

This was a learning exercise to begin teaching myself embedded development. I
wouldn't recommend you use this code for anything you deem to be important.

For the sake of code-cleanliness, non-application code has been extracted to a
helper library:
[embedded-sangster_atmega328p](https://github.com/sangster/embedded-sangster_atmega328p).

## Features

 - An LCD screen which shows live statistics: Soil moisture, air humidity, air
   temperature, and the current date and time.
 - To save power, the LCD turns off 1 minute after the user stops pressing
   buttons.
 - A menu system which allows the user to change the runtime settings of the
   system (detailed below), using the four general-purpose buttons below the LCD
   screen.
 - Automatically waters the plant on a given schedule, but only if the soil
   moisture falls below a certain percentage.
 - An audible alarm system to notify you when the water tank runs low. This
   also disables the pump so it doesn't "run dry."
 - A toggle switch on the front panel to disable the audible alarm, in case it
   gets annoying.
 - Automatically turns on/off a grow light on a given schedule.
 - Supports an optional microSD card. If present, various spreadsheets will be
   populated over time. Hopefully, this data can be used to fine-tune your
   plant's care.


### Menu

The menu provides the following functionality and configuration options:

 - **Water Plant!**: Runs the watering process, regardless of the schedule or
   current soil moisture.
 - **Toggle Lamp**: Switches the lamp on/off, regardless of the current
   schedule. This "override" will continue until the next scheduled event. ex:
   If the lamp is scheduled to turn on at 8:00 am. If you toggle it off at 8:05
   am, it will remain off until 8:00 am the following day.
 - **Date Config**: Set the current year, month, and day.
 - **Time Config**: Set the current hour (24h clock) and minute.
 - **Lamp Config**:
   - **Starting hour**: The hour of the day the lamp should turn on.
   - **Period**: The number of hours the lamp should remain on.
 - **Pump Config**:
   - **Moisture Trigger**: The level of soil moisture (%) below which the pump
     will be allowed to run.
   - **Delay**: The minimum amount of time that must pass after watering the
     plant before watering it again.
   - **Amount**: The amount of water (mL) to water the plant with each time.
 - **Log Config**:
   - **Period**: How often general statistics will be written to the SD card
     (if one is available)


### Log Files

If the optional SD card is inserted, three types of log files (in the form of
.csv spreadsheets) will be populated over time.

#### General Statistics

Every so often (See the menu option **Log Config > Period**), the Planter will
dump all the statistics it has available to a log file. As this file will be
updated most often and has the most columns, a new file will be created each
month, with the filename format of `YYYY-MM.log`.

##### Example

| Time | Moisture (%) | Humidity (%) | Temperature (C) | Time of Last Watering | Conf: Lamp Start | Conf: Lamp Period (Hrs) | Conf: Pump Minimum (% Moisture) | Conf: Pump Delay (Hrs) | Conf: Pump Amount (mL) | Conf: Log Period (Mins) | Lamp State | Buoy State |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 2018-03-22 10:00:00 | 90 | 20 | 21 | 2018-03-21 20:31:59 | 08:00 | 10 | 20 | 4 | 50 | 6 | Off | Up |
| 2018-03-22 11:00:01 | 86 | 20 | 22 | 2018-03-21 20:31:59 | 08:00 | 10 | 20 | 4 | 50 | 6 | On | Up |

#### Watering History

Every time the plant gets watered a new entry is added to the `Water.log`
spreadsheet, including the soil moisture (%) before the plant was watered and
how much water was supplied.

##### Example

| Time | Moisture Before | mL |
| --- | --- | --- |
| 2018-03-12 00:12:17 | 19 | 52 |
| 2018-03-15 15:15:31 | 17 | 50 |

#### Watering History

Every time the grow light is turned on or off, the event is recorded in the
`Lamp.log` spreadsheet.

##### Example

| Time | New Lamp State |
| --- | --- |
| 2018-03-18 00:08:00 | On  |
| 2018-03-18 00:18:00 | Off |
| 2018-03-19 00:09:00 | On  |

## Build Process

### Major Components

  - Two custom PCBs (more info below):
    - Motherboard
    - UI panel
  - Generic 2x16 LCD panel
  - 4 momentary push buttons
  - 1 toggle switch
  - OSEPP Humidity & Moisture Breakout (MHUM-01)
  - OSEPP Real-time Clock & microSD Breakout (RTCSD-01)
  - Generic YF-S402 Water Flow Sensor
  - Water Level Sensor Vertical Float Switch
  - The cheapest, weakest DC acquarium pump you can find
  - A cheapo desk lamp, with a "grow" lightbulb
  - 12v/2amp AC adapter
  - 12cm x 12cm x 9cm plastic project box
  - A plastic container to act as the water resevoir

### Custom PCBs

This project uses 2 homemade PCBs to wire everything together:

 - The Motherboard
 - The UI panel daughterboard

Both were designed using the open-source [Fritzing](http://fritzing.org), then
etched onto copper clad using the "toner transfer" process.

#### Motherboard

The motherboard's responsibilities are:

 - Receive 12v from the AC adapter.
 - Step the power down to 5v for the MCU and various low voltage components.
 - Provide 12v to the pump (when active).
 - Switch the Lamp and Pump relays.
 - Provide headers to connect the UI panel and other peripheral components.

##### Files

 - [mainboard.fzz](./planning/mainboard.fzz): The Fritzing file used to design
   the PCB and print out the toner-transfer sheets.
 - [mainboard.pdf](./toner-transfers/mainboard.pdf): The sheet to print to the
   transparency sheet, used for the transfer process. There are multiple copies
   per sheet, to allow for mistakes.

#### UI Panel

The UI Panel's responsibilities are:

 - Attaching to the lid of the project box.
 - Hosting buttons on one side, to be pushed through holes in the lid, and
   internal connects on the reverse side.
 - Providing headers to connect the LCD screen, also attached to box's lid.
 - Providing headers to connect to the alarm mute switch, also attache to the
   box's lid.
 - Provide a trimmer to adjust the LCD backlight

##### Files

 - [ui-panel.fzz](./planning/ui-panel.fzz): The Fritzing file used to design
   the PCB and print out the toner-transfer sheets.
 - [ui-panel.pdf](./toner-transfers/ui-panel.pdf): The sheet to print to the
   transparency sheet, used for the transfer process. There are multiple copies
   per sheet, to allow for mistakes.


### Assembly

The open-source [OpenSCAD](http://www.openscad.org/) CAD modelling tool was used
to determine where each component would be physicall located within the project
box. These files are held in the [cad/](./cad/) directory, but the main file is
[assembly.scad](./cad/assembly.scad).

OpenSCAD is also use for printing out the pattern of holes you need to cut out
of the project box to make room for buttons/LCD/etc by printing a 2D project of
the 3D model.

### Photos

Because this project is to create an actual device, photos should help. Please
keep in mind that this is my first embedded device. :)

How the project started: an empty plastic box and a "mockup" of the major
components on a breadboard.

![The project box next to the breadboard version](photos/project-box-and-breadboard.jpg "The project box next to the breadboard version")

---

The homebrew motherboard. The relays switch the pump and lamp. The LM7805
voltage regulator (right side of the image) bringe the 12v (required for the
pump) down to 5v (required for the MCU and other components).

![Motherboard PCB: front](photos/motherboard-front.jpg "Motherboard PCB: front")
![Motherboard PCB: back](photos/motherboard-back.jpg "Motherboard PCB: back")

---

The components mounted to the box itself: the 12v AC adapter, motherboard,
SD/RTC breakout board, and flow sensor.

![Internal components, without lid](photos/box-assembly.jpg "Internal components, without lid")

---

The UI panel PCB. You can see the buttons alone are on the front side. This is
so they can be pushed through holes cut in the lid. The remaining components
are on the back, so the front can remain as flat as possible.

On the back there is the LCD backlight trimmer, water-low-alarm buzzer, and LCD
shift register (drives the LCD with 2 pins instead of 4)

![UI Panel PCB: front](photos/ui-panel-front.jpg "UI Panel PCB: front")
![UI Panel PCB: back](photos/ui-panel-back.jpg "UI Panel PCB: back")

---

The assembled lid. OpenSCAD provided the template used to cut out holes in the
lid. The alignment was pretty critial to ensure the buttons on the PCB ligned
up with the holes, as well as their icons on the LCD display. Also featured:
the alarm mute switch.

![Template of holes to cut out of front panel](photos/front-panel-cutout.jpg "Template of holes to cut out of front panel")
![Template showing the buttons are correctly spaced](photos/front-panel-cutout-with-buttons.jpg "Template showing the buttons are correctly spaced")

![Assembled UI panel: front](photos/lid-assembly-front.jpg "Assembled UI panel: front")
![Assembled UI panel: back](photos/lid-assembly-back.jpg "Assembled UI panel: back")

---

Final assembly was simply plugging in all the wires to their correct headers.

![UI Panel: wired to LCB and motherboard](photos/ui-panel-wiring.jpg "UI Panel: wired to LCB and motherboard")
![Assembled Project](photos/assembly.jpg "Assembled Project")

---

And the initial test run. You can see the probe in the soil and the pump/float
system in the water resevoir. The project is quite light, so I just screwed it
to the top of the resevoir. Also featured: a bonsai tree near death from
waiting for this project's completion.

![The first test run of the system](photos/test-run.jpg "The first test run of the system")
![Humidity/Moisture/Temperature probe in the soil](photos/sensor-in-soil.jpg "Humidity/Moisture/Temperature probe in the soil")
![The pump and float in the water resevoir](photos/pump-and-float.jpg "The pump and float in the water resevoir")

---

## Deployment

Uploading the code to the microcontroller should be as simple as:

```sh
make upload
```

## License

This project is licensed under the GPL License - see the [LICENSE](./LICENSE)
file for details
