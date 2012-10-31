#!/bin/sh
$EXTRACTRC `find . -maxdepth 1 -name \*.rc -o -name \*.ui -o -name \*.kcfg` `find settings -name \*.rc -o -name \*.ui -o -name \*.kcfg` >> rc.cpp
$XGETTEXT `find . -maxdepth 1 -name \*.cc -o -name \*.cpp -o -name \*.h` `find settings -name \*.cc -o -name \*.cpp -o -name \*.h` -o $podir/kdevkrazy2.pot
rm -f rc.cpp
