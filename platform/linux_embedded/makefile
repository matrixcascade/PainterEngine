project_path := D:\working\PainterEngine_test
painterengine_path := ../..

target :=PainterEngine
#####################################################
#mingw32-64 makefile
CC := arm-linux-gnueabihf-gcc
AR := arm-linux-gnueabihf-ar
#####################################################
project_build := $(wildcard $(project_path)/*.c)
project_build_o := $(patsubst %.c,%.o,$(project_build))

painterengine_build_core := $(wildcard $(painterengine_path)/core/*.c)
painterengine_build_painterengine_core_o := $(patsubst %.c,%.o,$(painterengine_build_core))

painterengine_build_kernel := $(wildcard $(painterengine_path)/kernel/*.c)
painterengine_build_painterengine_kernel_o := $(patsubst %.c,%.o,$(painterengine_build_kernel))

painterengine_build_runtime := $(wildcard $(painterengine_path)/runtime/*.c)
painterengine_build_painterengine_runtime_o := $(patsubst %.c,%.o,$(painterengine_build_runtime))

painterengine_build_platform_c := $(wildcard $(painterengine_path)/platform/linux_embedded/*.c)
painterengine_build_platform_cpp := $(wildcard $(painterengine_path)/platform/linux_embedded/*.cpp)
painterengine_build_platform_alsa_c := $(wildcard $(painterengine_path)/platform/linux_embedded/tinyalsa/*.c)
painterengine_build_painterengine_platform_o := $(patsubst %.c,%.o,$(painterengine_build_platform_c)) $(patsubst %.cpp,%.o,$(painterengine_build_platform_cpp)) $(patsubst %.c,%.o,$(painterengine_build_platform_alsa_c))

libcore := core.a
libkernel := kernel.a
libruntime := runtime.a
libplatform := platform.a

all:$(project_build_o) $(libruntime) $(libplatform) $(libkernel) $(libcore) 
	$(CC) -O3 $(project_build_o) $(libruntime) $(libplatform) $(libkernel) $(libcore)   \
	-o $(project_path)/$(target) \
	-I "$(painterengine_path)" \
	-I "$(project_path)" \
	-I "$(painterengine_path)/platform/linux_embedded" \
	-L. -lpthread -ldl	
	make clean
	
$(libcore): $(painterengine_build_painterengine_core_o)
	$(AR) rcs $@ $^

$(libkernel): $(painterengine_build_painterengine_kernel_o)
	$(AR) rcs $@ $^

$(libruntime): $(painterengine_build_painterengine_runtime_o)
	$(AR) rcs $@ $^

$(libplatform): $(painterengine_build_painterengine_platform_o)
	$(AR) rcs $@ $^
	
$(project_path)/%.o:$(project_path)/%.c
	$(CC) -O3 -c $^ -o $@ -I "$(painterengine_path)" -I "$(painterengine_path)/platform/linux_embedded" -I "$(painterengine_path)/runtime"

$(painterengine_path)/runtime/%.o:$(painterengine_path)/runtime/%.c 
	$(CC) -O3 -c $^ -o $@

$(painterengine_path)/kernel/%.o:$(painterengine_path)/kernel/%.c
	$(CC) -O3 -c $^ -o $@

$(painterengine_path)/core/%.o:$(painterengine_path)/core/%.c
	$(CC) -O3 -c $^ -o $@

$(painterengine_path)/platform/linux_embedded/%.o:$(painterengine_path)/platform/linux_embedded/%.c
	$(CC) -O3 -c $^ -o $@ -I "$(project_path)" -I "$(painterengine_path)" -I "$(painterengine_path)/platform/linux_embedded" -I "$(painterengine_path)/runtime"

$(painterengine_path)/platform/linux_embedded/%.o:$(painterengine_path)/platform/linux_embedded/%.cpp
	$(CC) -O3 -c $^ -o $@ -I "$(project_path)" -I "$(painterengine_path)" -I "$(painterengine_path)/platform/linux_embedded" -I "$(painterengine_path)/runtime"

.PHONY:clean
clean:
	-del  "$(painterengine_path)\core\*.o"
	-del  "$(painterengine_path)\kernel\*.o"
	-del  "$(painterengine_path)\runtime\*.o"
	-del  "$(painterengine_path)\platform\linux_embedded\*.o"
	-del  "$(painterengine_path)\platform\linux_embedded\tinyalsa\*.o"
	-del  $(libruntime) $(libplatform) $(libkernel) $(libcore) 
	-del  "$(project_path)\*.o"