#include <tinyalsa/limits.h>

const struct tinyalsa_unsigned_interval tinyalsa_channels_limit = {
    .max = TINYALSA_CHANNELS_MAX,
    .min = TINYALSA_CHANNELS_MIN
};

const struct tinyalsa_unsigned_interval tinyalsa_frames_limit = {
    .max = TINYALSA_FRAMES_MAX,
    .min = TINYALSA_FRAMES_MIN
};

