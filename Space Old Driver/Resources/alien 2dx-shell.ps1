$index=0
for($index=1;$index -le 3;$index++)
{
 .\PainterEngine_PngToTRaw.exe ".\alien\n$index.png" ".\alien\n$index.traw"
}
$index=0
for($index=1;$index -le 3;$index++)
{
 .\PainterEngine_PngToTRaw.exe ".\alien\a$index.png" ".\alien\a$index.traw"
}
$index=0
for($index=1;$index -le 3;$index++)
{
 .\PainterEngine_PngToTRaw.exe ".\alien\v$index.png" ".\alien\v$index.traw"
}
.\PainterEngine_2dxMakeTool.exe .\alien_makefile.txt .\alien\alien.2dx