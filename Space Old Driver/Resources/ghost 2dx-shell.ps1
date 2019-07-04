$index=0
for($index=1;$index -le 5;$index++)
{
 .\PainterEngine_PngToTRaw.exe ".\ghost\normal$index.png" ".\ghost\normal$index.traw"
}
$index=0
for($index=1;$index -le 5;$index++)
{
 .\PainterEngine_PngToTRaw.exe ".\ghost\angry$index.png" ".\ghost\angry$index.traw"
}

.\PainterEngine_2dxMakeTool.exe .\ghost_makefile.txt .\ghost\ghost.2dx