[CmdletBinding()]
param()
$ErrorActionPreference='Stop'
Add-Type -AssemblyName System.Drawing
$sourceRoot=Split-Path -Parent $PSScriptRoot
$inputFile=Join-Path $sourceRoot 'Sources\GameClient\PhongThan.ico'
$outputFile=Join-Path $sourceRoot 'Sources\GameClient\PhongThan-vc6.ico'
# Preserve the original PNG icon. RC6 needs classic BITMAPINFOHEADER images.
$bytes=[IO.File]::ReadAllBytes($inputFile)
$length=[BitConverter]::ToUInt32($bytes,14)
$offset=[BitConverter]::ToUInt32($bytes,18)
if($offset+$length -gt $bytes.Length){throw 'Invalid original icon entry.'}
$imageStream=New-Object IO.MemoryStream(,$bytes[$offset..($offset+$length-1)])
$image=[Drawing.Image]::FromStream($imageStream)
$images=New-Object 'System.Collections.Generic.List[byte[]]'
try {
    foreach($size in @(16,32,48)) {
        $bitmap=New-Object Drawing.Bitmap($size,$size,[Drawing.Imaging.PixelFormat]::Format32bppArgb)
        $graphics=[Drawing.Graphics]::FromImage($bitmap)
        $stream=New-Object IO.MemoryStream
        $writer=New-Object IO.BinaryWriter($stream)
        try {
            $graphics.InterpolationMode=[Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
            $graphics.DrawImage($image,0,0,$size,$size)
            $maskStride=[int]([Math]::Ceiling($size/32.0)*4)
            $writer.Write([int]40);$writer.Write([int]$size);$writer.Write([int]($size*2))
            $writer.Write([uint16]1);$writer.Write([uint16]32);$writer.Write([int]0)
            $writer.Write([int]($size*$size*4+$maskStride*$size))
            foreach($unused in 1..4){$writer.Write([int]0)}
            for($y=$size-1;$y -ge 0;$y--){for($x=0;$x -lt $size;$x++){
                $pixel=$bitmap.GetPixel($x,$y)
                $writer.Write([byte]$pixel.B);$writer.Write([byte]$pixel.G);$writer.Write([byte]$pixel.R);$writer.Write([byte]$pixel.A)
            }}
            for($y=$size-1;$y -ge 0;$y--){
                $mask=New-Object byte[] $maskStride
                for($x=0;$x -lt $size;$x++){if($bitmap.GetPixel($x,$y).A -eq 0){$mask[[int][Math]::Floor($x/8)]= $mask[[int][Math]::Floor($x/8)] -bor (128 -shr ($x%8))}}
                $writer.Write($mask)
            }
            $images.Add($stream.ToArray())
        } finally {$writer.Dispose();$stream.Dispose();$graphics.Dispose();$bitmap.Dispose()}
    }
    $stream=New-Object IO.MemoryStream;$writer=New-Object IO.BinaryWriter($stream)
    try {
        $writer.Write([uint16]0);$writer.Write([uint16]1);$writer.Write([uint16]3)
        $offset=6+16*3;$index=0
        foreach($size in @(16,32,48)){
            $writer.Write([byte]$size);$writer.Write([byte]$size);$writer.Write([uint16]0)
            $writer.Write([uint16]1);$writer.Write([uint16]32)
            $writer.Write([uint32]$images[$index].Length);$writer.Write([uint32]$offset)
            $offset+=$images[$index].Length;$index++
        }
        foreach($entry in $images){$writer.Write($entry)}
        [IO.File]::WriteAllBytes($outputFile,$stream.ToArray())
    } finally {$writer.Dispose();$stream.Dispose()}
} finally {$image.Dispose();$imageStream.Dispose()}
Write-Output "VC6_ICON_READY: $outputFile"
