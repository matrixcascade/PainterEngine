
target := PainterEngineWorld
project_path := ../../project
painterengine_path := ../..

project_build := $(wildcard $(project_path)/*.c)
project_build_o := $(patsubst %.c,%.o,$(project_build))

painterengine_build_core := $(wildcard $(painterengine_path)/core/*.c)
painterengine_build_painterengine_o := $(patsubst %.c,%.o,$(painterengine_build_core))

painterengine_build_kernel := $(wildcard $(painterengine_path)/kernel/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_kernel))

painterengine_build_runtime := $(wildcard $(painterengine_path)/runtime/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_runtime))

painterengine_build_platform := $(wildcard $(painterengine_path)/platform/macos/*.c)
painterengine_build_painterengine_o += $(patsubst %.c,%.o,$(painterengine_build_platform))

painterengine_build_platform := $(wildcard $(painterengine_path)/platform/macos/*.m)
painterengine_build_painterengine_o += $(patsubst %.m,%.o,$(painterengine_build_platform))

all: $(project_build_o) $(painterengine_build_painterengine_o)
	clang $(project_build_o) $(painterengine_build_painterengine_o) \
	-o $(target) \
	-I "$(painterengine_path)" \
	-I "$(project_path)" \
	-I "$(painterengine_path)/platform/macos" \
	-I "$(painterengine_path)/runtime" \
	-L. -framework Cocoa -framework QuartzCore -framework Metal -framework MetalKit -framework CoreAudio -framework AudioUnit

clean:
	rm -f $(project_build_o) $(painterengine_build_painterengine_o)
	

$(project_path)/%.o: $(project_path)/%.c
	clang -c $^ -o $@ -I "$(painterengine_path)" -I "$(painterengine_path)/platform/macos" -I "$(painterengine_path)/runtime"

$(painterengine_path)/runtime/%.o: $(painterengine_path)/runtime/%.c
	clang -c $^ -o $@ -I "$(painterengine_path)"

$(painterengine_path)/kernel/%.o: $(painterengine_path)/kernel/%.c
	clang -c $^ -o $@

$(painterengine_path)/core/%.o: $(painterengine_path)/core/%.c
	clang -c $^ -o $@

$(painterengine_path)/platform/macos/%.o: $(painterengine_path)/platform/macos/%.c
	clang -c $^ -o $@ -I "$(project_path)" -I "$(painterengine_path)" -I "$(painterengine_path)/platform/macos" -I "$(painterengine_path)/runtime"

$(painterengine_path)/platform/macos/%.o: $(painterengine_path)/platform/macos/%.m
	clang -fobjc-arc -c $^ -o $@ -I "$(project_path)" -I "$(painterengine_path)" -I "$(painterengine_path)/platform/macos" -I "$(painterengine_path)/runtime"
