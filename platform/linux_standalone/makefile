project_path := .
painterengine_path := ../../PainterEngine

#gcc makefile
#####################################################
target :=server
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

painterengine_build_platform := $(wildcard $(painterengine_path)/platform/linux_standalone/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_platform))

painterengine_build_platform := $(wildcard $(painterengine_path)/platform/linux_standalone/*.cpp)
painterengine_build_painterengine_o += $(patsubst %.cpp,%.o,$(painterengine_build_platform))


all:$(project_build_o)  $(painterengine_build_painterengine_o) 
	$(cc) -O3 $(painterengine_build_painterengine_o) $(project_build_o) \
	-o $(project_path)/$(target) \
	-I "$(painterengine_path)" \
	-I "$(project_path)" \
	-I "$(painterengine_path)/platform/linux_standalone" \
	-L. -lpthread -ldl	\
	-lm


$(project_path)/%.o:$(project_path)/%.c
	$(cc) -O3 -c $^ -o $@ -I "$(painterengine_path)" -I "$(painterengine_path)/platform/linux_standalone" 

$(project_path)/%.o:$(project_path)/%.cpp
	$(cc) -O3 -c $^ -o $@ -I "$(painterengine_path)" -I "$(painterengine_path)/platform/linux_standalone" 

$(painterengine_path)/kernel/%.o:$(painterengine_path)/kernel/%.c
	$(cc) -O3 -c $^ -o $@

$(painterengine_path)/core/%.o:$(painterengine_path)/core/%.c
	$(cc) -O3 -c $^ -o $@

$(painterengine_path)/platform/linux_standalone/%.o:$(painterengine_path)/platform/linux_standalone/%.c
	$(cc) -O3 -c $^ -o $@ -I "$(painterengine_path)" -I "$(painterengine_path)/platform/linux_standalone" 
	
.PHONY:clean
clean:
	-rm  $(painterengine_path)/core/*.o
	-rm  $(painterengine_path)/kernel/*.o
	-rm  $(painterengine_path)/runtime/*.o
	-rm  $(painterengine_path)/platform/linux_standalone/*.o
	-rm  $(project_path)/*.o

	