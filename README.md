# nodemcu-wifi-scanner

A wifi scanner for the nodemcu that displays results on 128x64 screen.

Change the fonts using _TitleFont_ and _DetailFont_. Number of pages of results is determined by _DetailFont_ metrics. You can pick a font from here: https://github.com/olikraus/u8g2/wiki/fntgrp

Once started, it will list pages of wifi networks and their supported standards for about 10 seconds a page, before rescanning for networks again.
