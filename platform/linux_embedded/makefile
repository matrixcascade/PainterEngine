project_path := ../../../../project
painterengine_path := ../..

#gcc makefile
#####################################################
target :=PainterEngine
#####################################################

cc=arm-linux-gnueabihf-gcc

project_build := $(wildcard $(project_path)/*.c)
project_build_o := $(patsubst %.c,%.o,$(project_build))

painterengine_build_core := $(wildcard $(painterengine_path)/core/*.c)
painterengine_build_painterengine_o := $(patsubst %.c,%.o,$(painterengine_build_core))

painterengine_build_kernel := $(wildcard $(painterengine_path)/kernel/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_kernel))

painterengine_build_architecture := $(wildcard $(painterengine_path)/architecture/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_architecture))

painterengine_build_platform := $(wildcard $(painterengine_path)/platform/linux/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_platform))

painterengine_build_platform := $(wildcard $(painterengine_path)/platform/linux/*.cpp)
painterengine_build_painterengine_o += $(patsubst %.cpp,%.o,$(painterengine_build_platform))

painterengine_build_platform := $(wildcard $(painterengine_path)/platform/linux/tinyalsa/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_platform))


all:$(project_build_o)  $(painterengine_build_painterengine_o) 
	$(cc) $(project_build_o) $(painterengine_build_painterengine_o) \
	-o $(project_path)/$(target) \
	-I "$(painterengine_path)" \
	-I "$(project_path)" \
	-I "$(painterengine_path)/platform/linux" \
	-L. -lpthread -ldl	

$(project_path)/%.o:$(project_path)/%.c
	$(cc) -c $^ -o $@ -I "$(painterengine_path)" -I "$(painterengine_path)/platform/linux"

$(painterengine_path)/architecture/%.o:$(painterengine_path)/architecture/%.c 
	$(cc) -c $^ -o $@ -I "$(painterengine_path)"

$(painterengine_path)/kernel/%.o:$(painterengine_path)/kernel/%.c
	$(cc) -c $^ -o $@

$(painterengine_path)/core/%.o:$(painterengine_path)/core/%.c
	$(cc) -c $^ -o $@

$(painterengine_path)/platform/linux/%.o:$(painterengine_path)/platform/linux/%.c
	$(cc) -c $^ -o $@ -I "$(project_path)" -I "$(painterengine_path)" -I "$(painterengine_path)/platform/linux"

.PHONY:clean
clean:
	-del "$(painterengine_path)\core\*.o"
	-del "$(painterengine_path)\kernel\*.o"
	-del "$(painterengine_path)\architecture\*.o"
	-del "$(painterengine_path)\platform\linux\*.o"
	-del "$(project_path)\*.o"
cleanall:
	-del "$(painterengine_path)\core\*.o"
	-del "$(painterengine_path)\kernel\*.o"
	-del "$(painterengine_path)\architecture\*.o"
	-del "$(painterengine_path)\platform\linux\*.o"
	-del "$(project_path)\*.o"
	-del "$(project_path)\PainterEngine"

	
