# AspenCnc Control Screen

I started this intending it as straight forward fork of Candle.
It's become more than that. 

Something to consider if you want to use this code:

  * A pull request against Candle master seems unlikely to be accepted because 
    I dont use the UI files (or will not when work is complete). This is a big
    format related change. The developers used the UI files for a reason I have
    not used them for my own reasons.

My overall purpose: 
  * In my project I am exploring the benefits of software on rigidity 
    limitations in hobby CNC machines. 
  * I am adding a number of sensors which are not common in hobby machines 
    and also some experimental sensors not found often in industrial CNCs. 
  * I'm bringing much of that sensor data to the screen.
  * Obviously there will also be a control board design that ties this all 
    together. I'll link the url when I have published this design.

## Description

Hobby/desktop machine tools suffer from rigidity issues. The components
tend to be light weight, off the shelf and inexpensive.

S/w has solved similar problems in other domains. Can it be used in machine
tools. As in: a sufficiently powerful embedded processor is in distinguishable 
from magic, or something to that effect.

My target machine:
  * Work envelop: 300mm x 300mm x 125mm
  * Frame envelop: 500mm x 500mm x 500mm
  * Target weight of the mechanics:  75 lbs
  * Cost target: <$750 all in (machine, motion board, excludes PC)
  * Accuracy: capable of 0.0025 in. with repeatability in materials at the 
    hardness of brass and below.

## Example

Example screen shots

![Main panel](./screenshots/dro_feed_jog.png?raw=true "Main panel")
![Middle controls](./screenshots/middle_controls.png?raw=true "Middle controls")
 
## Authors

Habanero-GH : no contact info

## Version History

* 0.1
    * Initial Release

## License

I have not decided what license. Once this project is more useful I will
decode.

## Acknowledgments
  Credits to the open source materials I am using. 

  * Candle:
      * Copyright 2015-2016 Hayrullin Denis Ravilevich
   
  * Candle borrows bits from "Universal GcodeSender" 
      * application written by Will Winder, no date.
  
  * Esp32 Grbl:
      * I use Bart Dring's ESP32 port of Grbl
      * https://github.com/bdring/Grbl_Esp32/blob/main/README.md
  
  * Grbl:
      * https://github.com/gnea/grbl
      * Lead Developer: Sungeun "Sonny" Jeon, Ph.D. (USA) aka @chamnit
  
  * Grbl originally built from the wonderful Grbl v0.6 (2011) 
      * firmware written by Simen Svale Skogsrud (Norway).
  
  * Physics Anonymous
      * https://www.youtube.com/c/PhysicsAnonymous
      * The interface I am creating and layering onto Candle in Qt is borrowed 
      with repect and appreciation from PA.
      * I'm not using Mach 3/4 but I like their design.

  * Qt5:
      * Candle uses Qt5, a Qt6 recompile failed, missing module, hence I use Qt5.
      * Qt, a work of pure genius by then Trolltech, then Nokia, then some other
      names, then Digia, now 'The Qt Company' I think. In their original form
      Qt3/4 were superb. 

  * Icons:
      * Icons are borrowed from multiple free sources.
        * <Oxygen> FIXME: add URL
        * <a href="https://www.flaticon.com/free-icons/document" title="document icons"> Document icons created by Freepik - Flaticon</a>
        * <a href="https://www.flaticon.com/free-icons/before" title="before icons"> Before icons created by Smashicons - Flaticon</a> 
        * <a href="https://www.flaticon.com/free-icons/edit" title="edit icons"> Edit icons created by Pixel perfect - Flaticon</a>
        * <a href="https://www.flaticon.com/free-icons/play" title="play icons"> Play icons created by Freepik - Flaticon</a>
        * <a href="https://www.flaticon.com/free-icons/close" title="close icons">Close icons created by ariefstudio - Flaticon</a>
        * <a href="https://www.flaticon.com/free-icons/pause" title="pause icons">Pause icons created by inkubators - Flaticon</a>
        * <a href="https://www.flaticon.com/free-icons/rewind" title="rewind icons">Rewind icons created by Freepik - Flaticon</a>
        * <a href="https://www.flaticon.com/free-icons/next" title="next icons">Next icons created by Roundicons - Flaticon</a>
        * <a href="https://www.flaticon.com/free-icons/stairs" title="stairs icons">Stairs icons created by Icon mania - Flaticon</a>

  Thanks folks.

