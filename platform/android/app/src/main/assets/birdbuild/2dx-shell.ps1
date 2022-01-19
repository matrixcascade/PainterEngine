mkdir ./release

$index=0
for($index=1;$index -le 5;$index++)
{
 .\PainterEngine_PngToTRaw.exe ".\bird$index.png" ".\release\bird$index.traw"
}

.\PainterEngine_2dxMakeTool.exe .\makefile.txt .\release\release.2dx

del .\release\*.traw