#mingw32-64 makefile
#####################################################
target :=PainterEngine.exe
project_path := ..\..\project
painterengine_path := ..\..
#####################################################

project_build := $(wildcard $(project_path)/*.c)
project_build_o := $(patsubst %.c,%.o,$(project_build))

painterengine_build_core := $(wildcard $(painterengine_path)/core/*.c)
painterengine_build_painterengine_o := $(patsubst %.c,%.o,$(painterengine_build_core))

painterengine_build_kernel := $(wildcard $(painterengine_path)/kernel/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_kernel))

painterengine_build_architecture := $(wildcard $(painterengine_path)/architecture/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_architecture))

painterengine_build_platform := $(wildcard $(painterengine_path)/platform/windows/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_platform))

painterengine_build_platform := $(wildcard $(painterengine_path)/platform/windows/*.cpp)
painterengine_build_painterengine_o += $(patsubst %.cpp,%.o,$(painterengine_build_platform))

all:$(project_build_o)  $(painterengine_build_painterengine_o) 
	gcc $(project_build_o) $(painterengine_build_painterengine_o) \
	-o $(project_path)\$(target) \
	-I "$(painterengine_path)" \
	-I "$(project_path)" \
	-I "$(painterengine_path)\platform\windows" \
	-L. -lwinmm -ld2d1 -lws2_32 -ldsound -lcomdlg32
	$(project_path)\$(target)
	make clean
	

$(project_path)/%.o:$(project_path)/%.c
	gcc -c $^ -o $@ -I "$(painterengine_path)" -I "$(painterengine_path)/platform/windows" 

$(painterengine_path)/architecture/%.o:$(painterengine_path)/architecture/%.c 
	gcc -c $^ -o $@ -I "$(painterengine_path)"

$(painterengine_path)/kernel/%.o:$(painterengine_path)/kernel/%.c
	gcc -c $^ -o $@

$(painterengine_path)/core/%.o:$(painterengine_path)/core/%.c
	gcc -c $^ -o $@

$(painterengine_path)/platform/windows/%.o:$(painterengine_path)/platform/windows/%.c
	gcc -c $^ -o $@ -I "$(project_path)" -I "$(painterengine_path)" -I "$(painterengine_path)/platform/windows"


.PHONY:clean
clean:
	-del /s  "$(painterengine_path)\core\*.o"
	-del /s  "$(painterengine_path)\kernel\*.o"
	-del /s  "$(painterengine_path)\architecture\*.o"
	-del /s  "$(painterengine_path)\platform\windows\*.o"
	-del /s  "$(project_path)\*.o"

	