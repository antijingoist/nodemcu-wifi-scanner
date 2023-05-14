# nodemcu-wifi-scanner

A wifi scanner for the nodemcu that displays results on 128x64 screen.

Change the fonts using _TitleFont_ and _DetailFont_, and change the _lineheight_ when changing _DetailFont_ to ensure it paginates properly. pageResults should be equal to the ((displayHeight - _pxlStart_) / _lineHeight_) but I did not set it up. 

Once started, it will list pages of wifi networks and their supported standards for about 10 seconds a page, before rescanning for networks again.
