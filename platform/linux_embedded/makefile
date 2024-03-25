project_path := "your project path here"
painterengine_path := ../..

#gcc makefile
#####################################################
target :=PainterEngine
#####################################################
cc=gcc

project_build := $(wildcard $(project_path)/*.c)
project_build_cpp = $(wildcard $(project_path)/*.cpp)

project_build_o := $(patsubst %.c,%.o,$(project_build))
project_build_o += $(patsubst %.cpp,%.o,$(project_build_cpp))

painterengine_build_core := $(wildcard $(painterengine_path)/core/*.c)
painterengine_build_painterengine_o := $(patsubst %.c,%.o,$(painterengine_build_core))

painterengine_build_kernel := $(wildcard $(painterengine_path)/kernel/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_kernel))

painterengine_build_runtime := $(wildcard $(painterengine_path)/runtime/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_runtime))

painterengine_build_platform := $(wildcard $(painterengine_path)/platform/linux_embedded/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_platform))

painterengine_build_platform := $(wildcard $(painterengine_path)/platform/linux_embedded/*.cpp)
painterengine_build_painterengine_o += $(patsubst %.cpp,%.o,$(painterengine_build_platform))

painterengine_build_platform := $(wildcard $(painterengine_path)/platform/linux_embedded/tinyalsa/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_platform))


all:$(project_build_o)  $(painterengine_build_painterengine_o) 
	$(cc) $(project_build_o) $(painterengine_build_painterengine_o) \
	-o $(project_path)/$(target) \
	-I "$(painterengine_path)" \
	-I "$(project_path)" \
	-I "$(painterengine_path)/platform/linux_embedded" \
	-L. -lpthread -ldl	
	make clean

$(project_path)/%.o:$(project_path)/%.c
	$(cc) -c $^ -o $@ -I "$(painterengine_path)" -I "$(painterengine_path)/platform/linux_embedded" -I "$(painterengine_path)/runtime"

$(project_path)/%.o:$(project_path)/%.cpp
	$(cc) -c $^ -o $@ -I "$(painterengine_path)" -I "$(painterengine_path)/platform/linux_embedded" -I "$(painterengine_path)/runtime"

$(painterengine_path)/runtime/%.o:$(painterengine_path)/runtime/%.c 
	$(cc) -c $^ -o $@ -I "$(painterengine_path)" 

$(painterengine_path)/kernel/%.o:$(painterengine_path)/kernel/%.c
	$(cc) -c $^ -o $@

$(painterengine_path)/core/%.o:$(painterengine_path)/core/%.c
	$(cc) -c $^ -o $@

$(painterengine_path)/platform/linux_embedded/%.o:$(painterengine_path)/platform/linux_embedded/%.c
	$(cc) -c $^ -o $@ -I "$(project_path)" -I "$(painterengine_path)" -I "$(painterengine_path)/platform/linux_embedded" -I "$(painterengine_path)/runtime"
	
