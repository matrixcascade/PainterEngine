#gcc freeglut makefile
#####################################################
target :=PainterEngine
project_path := "your project path"
painterengine_path := ../..
#####################################################
#mingw32-64 makefile
CC := gcc
AR := ar
#####################################################
project_build := $(wildcard $(project_path)/*.c)
project_build_o := $(patsubst %.c,%.o,$(project_build))

painterengine_build_core := $(wildcard $(painterengine_path)/core/*.c)
painterengine_build_painterengine_core_o := $(patsubst %.c,%.o,$(painterengine_build_core))

painterengine_build_kernel := $(wildcard $(painterengine_path)/kernel/*.c)
painterengine_build_painterengine_kernel_o := $(patsubst %.c,%.o,$(painterengine_build_kernel))

painterengine_build_runtime := $(wildcard $(painterengine_path)/runtime/*.c)
painterengine_build_painterengine_runtime_o := $(patsubst %.c,%.o,$(painterengine_build_runtime))

painterengine_build_platform_c := $(wildcard $(painterengine_path)/platform/linux/*.c)
painterengine_build_platform_cpp := $(wildcard $(painterengine_path)/platform/linux/*.cpp)
painterengine_build_platform_alsa_c := $(wildcard $(painterengine_path)/platform/linux/tinyalsa/*.c)
painterengine_build_painterengine_platform_o := $(patsubst %.c,%.o,$(painterengine_build_platform_c)) $(patsubst %.cpp,%.o,$(painterengine_build_platform_cpp)) $(patsubst %.c,%.o,$(painterengine_build_platform_alsa_c))

libcore := core.a
libkernel := kernel.a
libruntime := runtime.a
libplatform := platform.a

all:$(project_build_o) $(libruntime) $(libplatform) $(libkernel) $(libcore) 
	$(CC) -O3 $(project_build_o) $(libruntime) $(libplatform) $(libkernel) $(libcore)   \
	-o $(target) \
	-I "$(painterengine_path)" \
	-I "$(project_path)" \
	-I "$(painterengine_path)/platform/linux" \
	-I "$(painterengine_path)/runtime" \
	-L. -lGL -lglut -lpthread
	
$(libcore): $(painterengine_build_painterengine_core_o)
	$(AR) rcs $@ $^

$(libkernel): $(painterengine_build_painterengine_kernel_o)
	$(AR) rcs $@ $^

$(libruntime): $(painterengine_build_painterengine_runtime_o)
	$(AR) rcs $@ $^

$(libplatform): $(painterengine_build_painterengine_platform_o)
	$(AR) rcs $@ $^
	
$(project_path)/%.o:$(project_path)/%.c
	$(CC) -O3 -c $^ -o $@ -I "$(painterengine_path)" -I "$(painterengine_path)/platform/linux" -I "$(painterengine_path)/runtime"

$(painterengine_path)/runtime/%.o:$(painterengine_path)/runtime/%.c 
	$(CC) -O3 -c $^ -o $@

$(painterengine_path)/kernel/%.o:$(painterengine_path)/kernel/%.c
	$(CC) -O3 -c $^ -o $@

$(painterengine_path)/core/%.o:$(painterengine_path)/core/%.c
	$(CC) -O3 -c $^ -o $@

$(painterengine_path)/platform/linux/%.o:$(painterengine_path)/platform/linux/%.c
	$(CC) -O3 -c $^ -o $@ -I "$(project_path)" -I "$(painterengine_path)" -I "$(painterengine_path)/platform/linux" -I "$(painterengine_path)/runtime"

$(painterengine_path)/platform/linux/%.o:$(painterengine_path)/platform/linux/%.cpp
	$(CC) -O3 -c $^ -o $@ -I "$(project_path)" -I "$(painterengine_path)" -I "$(painterengine_path)/platform/linux" -I "$(painterengine_path)/runtime"

.PHONY:clean
clean:
	rm  -f "$(painterengine_path)\core\*.o"
	rm  -f "$(painterengine_path)\kernel\*.o"
	rm  -f "$(painterengine_path)\runtime\*.o"
	rm  -f "$(painterengine_path)\platform\linux_embedded\tinyalsa\*.o"
	rm  -f "$(painterengine_path)\platform\linux\*.o"
	rm  -f $(libruntime) $(libplatform) $(libkernel) $(libcore)
	rm  -f "$(project_path)\*.o"