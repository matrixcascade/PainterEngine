$index=0
for($index=1;$index -le 1;$index++)
{
 .\PainterEngine_PngToTRaw.exe ".\userdef\u$index.png" ".\userdef\u$index.traw"
}
 .\PainterEngine_PngToTRaw.exe ".\userdef\huaji_bullet.png" ".\userdef\huaji_bullet.traw"

.\PainterEngine_2dxMakeTool.exe .\userdef_makefile.txt .\userdef\userdef.2dx