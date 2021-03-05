-- 工程
target("demo")

-- 构建之后运行插件
after_build(function(target)
    -- 导入task模块
    import("core.base.task")
    -- 运行插件任务
    task.run("project", {kind = "compile_commands"})
end)

set_kind("binary")
set_optimize("faster")
set_targetdir("$(buildir)/$(mode)")

add_includedirs("$(projectdir)")
add_includedirs("$(projectdir)/platform/framework")

add_files("$(projectdir)/core/*.c")
add_files("$(projectdir)/kernel/*.c")
add_files("$(projectdir)/architecture/*.c")
add_files("$(projectdir)/platform/framework/*.c")
add_files("$(projectdir)/platform/linux/*.c|px_serialport.c|px_tcp.c|px_audio.c|px_udp.c")

add_frameworks("GLUT", "OpenGL")
