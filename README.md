# Path's Event Log example

This code serves as an example to create your own Event Log log and events, the modern Manifest way.

**WARNING** Don't using the `auto_wevtutil_*` functions in a production system!
They are a dodgy-way to do this, insted add the provider during an install process or something

# Steps
## Create Man XML file
Create `provider.man` using `ecmangen.exe` UI (from Windows SDK)

Unique Levels, Opcode, Keywords only neccessary if you want to define your own.
Templates also only neccessary if you want to display a unique message

Make sure to fill out `resourceFileName` and `messageFileName` with full paths to actual executable

## Compile
Using `mc.exe` from either windows kits or Visual Studio tools
```batch
mc -um provider.man
```
Add the outputted `provider.rc` file as an "existing item" in Visual Studio

## Install
As administator:
```batch
wevtutil im provider.man
```
If you get an error about a GUID you never specified, look in `Computer\HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\WINEVT\Publishers\` for *any* provider that doesn't have a text in it's `(default)` value.

## View logs
Open `Event Viewer`, view event in `eventwriter/Operational`

Or use PowerShell
```powershell
Get-WinEvent "eventwriter/Operational"

# Get the first field from every message:
Get-WinEvent "eventwriter/Operational" |% { $_.Properties[0].Value }

# Get Every Event in the last 6 hours
$t1 = [datetime]::Today.AddHours(-6).ToUniversalTime().ToString('s')
$xpath = @"
    *[System[TimeCreated[@SystemTime>'$t1']]]
"@
Get-WinEvent "eventwriter/Operational" -FilterXPath $xpath
```

# Cleanup
```batch
wevtutil um provider.man
```


# Notes
https://docs.microsoft.com/en-us/windows/win32/wes/windows-event-log
