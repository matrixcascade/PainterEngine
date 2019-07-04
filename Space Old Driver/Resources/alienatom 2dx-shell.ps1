$index=0
for($index=1;$index -le 5;$index++)
{
 .\PainterEngine_PngToTRaw.exe ".\aatom\at$index.png" ".\aatom\at$index.traw"
}

.\PainterEngine_2dxMakeTool.exe .\alienat_makefile.txt .\aatom\aatom.2dx