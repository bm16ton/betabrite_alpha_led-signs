Section "ServerLayout"
    Identifier "layout"
    Screen 0 "nvidia"
    Inactive "intel"
EndSection

Section "Device"
    Identifier "intel"
    Driver "modesetting"
    BusID "PCI:0@0:2:0"
    Option "AccelMethod" "None"
EndSection

Section "Screen"
    Identifier "intel"
    Device "intel"
EndSection

#Section "Monitor"
#    Identifier     "Monitor0"
#    VendorName     "LGD"
#    ModelName      "LP156WF6-SPB5"
#    ModelName      "Unknown"
#    Modeline       "1920x1080"  138.70  1920 1968 2000 2080  1080 1083 1088 1111 +hsync -vsync 
#    HorizSync       66.7
#    VertRefresh     60.0
#    Option         "DPMS"
#    DisplaySize     344 194
#EndSection

Section "Device"
    Identifier "nvidia"
    Driver "nvidia"
    BusID "PCI:1@0:0:0"
    Option "ConstrainCursor" "off"
    Option "ModeValidation" "NoVirtualSizeCheck"
    Option "ConnectedMonitor" "eDP-1,eDP-1-0"
#    Option "CustomEDID" "eDP-1:/sys/class/drm/card0-eDP-1/edid"
    Option "CustomEDID" "eDP-1-0:/lib/firmware/lg1080p.edid"
    Option "CustomEDID" "eDP-1:/lib/firmware/lg1080p.edid"
    Option "IgnoreEDID" "false"
    Option "UseEDID" "true"
#    Option "NoDDC"  "true"
    Option "DPI" "96x96"
#    Option "RandRRotation" "on"
EndSection

Section "Screen"
    Identifier "nvidia"
    Device "nvidia"
#    Monitor        "Monitor0"
    Option "AllowEmptyInitialConfiguration" "on"
    Option "IgnoreDisplayDevices" "CRT"
#    Option "UseDisplayDevice" "None"
    SubSection "Display"
	Depth	24
	Virtual	1920 1080
	Modes   "1920x1080_60.00" "1920x1080"
#	Modes   "1920x1080"
    EndSubSection
EndSection

