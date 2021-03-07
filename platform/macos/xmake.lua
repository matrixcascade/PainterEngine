target("demo")

--input your painterengine directory here
painterengine_dir=""

after_build(function(target)
    -- import task modules
    import("core.base.task")
    -- run task modules
    task.run("project", {kind = "compile_commands"})
end)

set_kind("binary")
set_optimize("faster")
set_targetdir("$(buildir)/$(mode)")

add_includedirs(painterengine_dir)
add_includedirs(painterengine_dir .. "/platform/framework")

add_files(painterengine_dir .. "/core/*.c")
add_files(painterengine_dir .. "/kernel/*.c")
add_files(painterengine_dir .. "/architecture/*.c")
add_files(painterengine_dir .. "/platform/framework/*.c")
add_files(painterengine_dir .. "/platform/macos/*.c")

add_frameworks("GLUT", "OpenGL")
