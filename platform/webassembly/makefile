#emscripten makefile
#####################################################
target :=PainterEngine.html
project_path := ../../../PainterEngine_MathematicalSoundGame_Sources
painterengine_path := ../..
#####################################################

project_build := $(wildcard $(project_path)/*.c)
project_build_o := $(patsubst %.c,%.o,$(project_build))

painterengine_build_core := $(wildcard $(painterengine_path)/core/*.c)
painterengine_build_painterengine_o := $(patsubst %.c,%.o,$(painterengine_build_core))

painterengine_build_kernel := $(wildcard $(painterengine_path)/kernel/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_kernel))

painterengine_build_architecture := $(wildcard $(painterengine_path)/architecture/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_architecture))

painterengine_build_platform := $(wildcard $(painterengine_path)/platform/webassembly/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_platform))

painterengine_build_platform := $(wildcard $(painterengine_path)/platform/webassembly/*.cpp)
painterengine_build_painterengine_o += $(patsubst %.cpp,%.o,$(painterengine_build_platform))

all:$(project_build_o)  $(painterengine_build_painterengine_o) 
	emcc $(project_build_o) $(painterengine_build_painterengine_o) \
	-O3 \
	-o $(target) \
	-I "$(painterengine_path)" \
	-I "$(project_path)" \
	-I "$(painterengine_path)/platform/webassembly" \
	--preload-file assets
	emrun --no_browser --port 8080 PainterEngine.html
	

$(project_path)/%.o:$(project_path)/%.c
	emcc -c $^ -o $@ -I "$(painterengine_path)" -I "$(painterengine_path)/platform/webassembly"

$(painterengine_path)/architecture/%.o:$(painterengine_path)/architecture/%.c 
	emcc -c $^ -o $@ -I "$(painterengine_path)"

$(painterengine_path)/kernel/%.o:$(painterengine_path)/kernel/%.c
	emcc -c $^ -o $@

$(painterengine_path)/core/%.o:$(painterengine_path)/core/%.c
	emcc -c $^ -o $@

$(painterengine_path)/platform/webassembly/%.o:$(painterengine_path)/platform/webassembly/%.c
	emcc -c $^ -o $@ -I "$(project_path)" -I "$(painterengine_path)" -I "$(painterengine_path)/platform/webassembly"

.PHONY:clean
clean:
	-del /s  "$(painterengine_path)\core\*.o"
	-del /s  "$(painterengine_path)\kernel\*.o"
	-del /s  "$(painterengine_path)\architecture\*.o"
	-del /s  "$(painterengine_path)\platform\webassembly\*.o"
	-del /s  "$(project_path)\*.o"
