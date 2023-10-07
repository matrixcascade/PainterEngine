#include "px_display.h"
#include "poll.h"
#include "tinyalsa/asoundlib.h"
#include "platform/modules/px_time.h"
#include "PainterEngine_Application.h"

px_int px_main_screen_width = 0, px_main_screen_height = 0;
px_int px_main_touch_fd = -1;
PX_SoundPlay* px_main_soundPlay;



int  PX_AudioInitialize(PX_SoundPlay* soundPlay)
{
    px_main_soundPlay = soundPlay;
    return 1;
}

void PX_AudioSetVolume(unsigned int volume)
{
    char content[64];
    PX_sprintf1(content, sizeof(content), "amixer - c 0 sset 'Headphone', 0 %1% unmute", PX_STRINGFORMAT_INT(volume));
    system(content);
}


int main()
{
    px_int fbfd, flags;
    px_color* gram32;
    px_int t_x = 0, t_y = 0;
    px_dword gram_size;
    px_dword timelasttime;
    px_dword pcm_delay = 0;
    struct pcm* pcm;
    struct input_event ts;

    if ((fbfd =px_main_initializedisplay(&px_main_screen_width,&px_main_screen_height))==0)
        return 0;

    px_main_touch_fd = open("/dev/input/event1", O_RDONLY);
    if (px_main_touch_fd == -1)
        return 0;

    if (!PX_ApplicationInitialize(&App, px_main_screen_width, px_main_screen_height))
        return 0;


    if (px_main_soundPlay)
    {
        unsigned int card = 0;
        unsigned int device = 0;
        int flags = PCM_OUT;

        const struct pcm_config config = {
            .channels = 2,
            .rate = 44100,
            .format = PCM_FORMAT_S16_LE,
            .period_size = 1024,
            .period_count = 2,
            .start_threshold = 0,
            .silence_threshold = 0,
            .stop_threshold = 0
        };

        pcm = pcm_open(card, device, flags, &config);
        if (pcm ==PX_NULL) {
            printf( "failed to allocate memory for PCM\n");
            return -1;
        }
        else if (!pcm_is_ready(pcm)) {
            pcm_close(pcm);
            printf( "failed to open PCM\n");
            return -1;
        }
    }


    gram_size = px_main_screen_width * px_main_screen_height * 4;
    timelasttime = PX_TimeGetTime();
    while (PX_TRUE)
    {
       //time
        px_dword elapsed;
        
        elapsed = PX_TimeGetTime() - timelasttime;
        timelasttime = PX_TimeGetTime();


        //touch
        do
        {
            struct pollfd fds[1];
            int ready;
            fds[0].fd = px_main_touch_fd;
            fds[0].events = POLLIN;

            ready = poll(fds, 1, 0); // Non-blocking

            if (ready > 0 && (fds[0].revents & POLLIN))
            {
                if (read(px_main_touch_fd, &ts, sizeof(ts)) > 0)
                {
                    PX_Object_Event e = { 0 };

                    if (ts.type == EV_ABS)
                    {
                        if (ts.code == ABS_X)
                            t_x = ts.value;

                        if (ts.code == ABS_Y)
                        {
                            t_y = ts.value;
                            e.Event = PX_OBJECT_EVENT_CURSORDRAG;
                            PX_Object_Event_SetCursorX(&e, t_x/4000.f*px_main_screen_width);
                            PX_Object_Event_SetCursorY(&e, t_y/4000.f*px_main_screen_height);
                            PX_ApplicationPostEvent(&App, e);
                        }
                    }

                    if (ts.type == EV_KEY && ts.code == BTN_TOUCH)
                    {
                        PX_Object_Event_SetCursorX(&e, t_x/4000.f*px_main_screen_width);
                        PX_Object_Event_SetCursorY(&e, t_y/4000.f*px_main_screen_height);
                        if (ts.value == 0)
                        {
                            e.Event = PX_OBJECT_EVENT_CURSORUP;

                        }
                        else
                        {
                            e.Event = PX_OBJECT_EVENT_CURSORDOWN;
                        }
                        PX_ApplicationPostEvent(&App, e);
                    }
                }
            }
        } while (0);

        //audio
        if(px_main_soundPlay)
        {
            px_short pcm_stream[4410];//4410sample 2channel 50ms
            pcm_delay += elapsed;
            if (pcm_delay>1000)
            {
                pcm_delay = 1000;
            }
            while (pcm_delay>=50)
            {
               px_int frame_count;
               frame_count = pcm_bytes_to_frames(pcm, sizeof(pcm_stream));
               PX_SoundPlayRead(&App.soundplay, (px_byte *)pcm_stream, sizeof(pcm_stream));
                if (pcm_writei(pcm, pcm_stream, frame_count) < 0)
                {
                    break;
                }
                pcm_delay -= 50;
            }
           
        } 
       
        PX_ApplicationUpdate(&App, elapsed);
        PX_ApplicationRender(&App, elapsed);

        px_main_displaygram(fbfd, App.runtime.RenderSurface.surfaceBuffer, gram_size);

        PX_Sleep(0);
    }
  
    return 0;
}
