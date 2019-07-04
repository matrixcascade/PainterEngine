$index=0
for($index=1;$index -le 5;$index++)
{
 .\PainterEngine_PngToTRaw.exe ".\cxk\cxk$index.png" ".\cxk\cxk$index.traw"
}
 .\PainterEngine_PngToTRaw.exe ".\cxk\ball.png" ".\cxk\ball.traw"

.\PainterEngine_2dxMakeTool.exe .\userdef2_makefile.txt .\cxk\cxk.2dx