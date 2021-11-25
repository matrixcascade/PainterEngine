from building import *
Import('rtconfig')

cwd = GetCurrentDir()

path = []
src = []

path = [cwd + '/core', cwd + '/kernel']
src += Glob(cwd + '/core/*.c')
src += Glob(cwd + '/kernel/*.c')

group = DefineGroup('PainterEngine', src, depend=['PKG_USING_PAINTERENGINE'], CPPPATH=path)

Return('group')
