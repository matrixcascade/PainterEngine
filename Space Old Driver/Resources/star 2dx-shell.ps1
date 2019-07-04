$index=0
for($index=1;$index -le 2;$index++)
{
 .\PainterEngine_PngToTRaw.exe ".\star\star$index.png" ".\star\star$index.traw"
}

.\PainterEngine_2dxMakeTool.exe .\star_makefile.txt .\star\star.2dx