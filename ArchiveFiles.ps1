$sourcePath = "$($Env:SkyrimPath)\Data"
$destinationDataPath = "$($Env:ModDevPath)\MODS\SkyrimLE\moreHUD\SKSE\skse\moreHUD\Data"
$destinationAS2Path = "$($Env:ModDevPath)\MODS\SkyrimLE\moreHUD\SKSE\skse\moreHUD\AS2"

if (!$(Test-Path "$destinationDataPath\Interface"))
{
    New-Item -ItemType Directory "$destinationDataPath\Interface"
    New-Item -ItemType Directory "$destinationDataPath\Scripts"
    New-Item -ItemType Directory "$destinationDataPath\Scripts\Source"
}

if (!$(Test-Path "$destinationDataPath\Interface\exported"))
{
    New-Item -ItemType Directory "$destinationDataPath\Interface\exported"
}

if (!$(Test-Path "$destinationDataPath\Interface\translations"))
{
    New-Item -ItemType Directory "$destinationDataPath\Interface\translations"
}

Copy-Item "$sourcePath\Scripts\ahz*.pex" -Destination "$destinationDataPath\Scripts"
Copy-Item "$sourcePath\Scripts\Source\ahz*.psc" -Destination "$destinationDataPath\Scripts\Source"
Copy-Item "$sourcePath\ahzmorehud.esp" -Destination "$destinationDataPath"
Copy-Item "$sourcePath\Interface\ahzhudinfo.swf" -Destination "$destinationDataPath\Interface"
Copy-Item "$sourcePath\Interface\exported\ahzhudinfo.swf" -Destination "$destinationDataPath\Interface\exported"
Copy-Item "$sourcePath\Interface\exported\ahzmorehudlogo.dds" -Destination "$destinationDataPath\Interface\exported"

Copy-Item "$sourcePath\Interface\translations\ahz*.txt" -Destination "$destinationDataPath\Interface\translations"

#AS2


Copy-Item "$($Env:ModDevPath)\MODS\SkyrimLE\moreHUD\ScaleForm\src\HUDWidgets\*" -Destination $destinationAS2Path -Exclude .git* -Recurse -Force