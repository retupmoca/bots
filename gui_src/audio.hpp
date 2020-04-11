/* rev 2ee34e61fb891e379c04fab7e363579f9c182585 */
#include <cstdint>
#include <portaudio.h>

namespace llm {
    class Sound {
    public:
        float volume = 1.0;

        virtual void sample(float &left, float &right) {
            tick(left, right);
            left *= volume;
            right *= volume;
        }

        virtual void tick(float &left, float &right) { left = right = 0; }
    };

    // TODO: SoundBus with an output volume?

    // TODO: exceptions on portaudio failures
    class SoundCard : public Sound {
    private:
        PaStream *stream = nullptr;

        static int callback( const void *inputBuffer, void *outputBuffer,
                                   unsigned long framesPerBuffer,
                                   const PaStreamCallbackTimeInfo* timeInfo,
                                   PaStreamCallbackFlags statusFlags,
                                   void *userData )
        {
            /* Cast data passed through stream to our structure. */
            SoundCard *data = (SoundCard*)userData;
            float *out = (float*)outputBuffer;
            unsigned int i;
            (void) inputBuffer; /* Prevent unused variable warning. */

            for( i=0; i<framesPerBuffer; i++ )
            {
                float left = 0.0f;
                float right = 0.0f;

                data->sample(left, right);

                *out++ = left;
                *out++ = right;
            }
            return 0;
        }

    public:
        SoundCard() {
            Pa_Initialize();
            Pa_OpenDefaultStream( &stream,
                                0,          /* no input channels */
                                2,          /* stereo output */
                                paFloat32,  /* 32 bit floating point output */
                                48000,      /* sample rate */
                                512,        /* frames per buffer */
                                &SoundCard::callback,
                                this);
        }

        void start() {
            Pa_StartStream( stream );
        }
        void wait() {}

        ~SoundCard() {
            if (stream) {
                Pa_StopStream(stream);
                Pa_CloseStream(stream);
            }
            Pa_Terminate();
        }
    };
};
