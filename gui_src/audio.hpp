/* rev 59af3dc3dafbcb0bf801e70b2d74ce8e5e11a0fa */
#include <cstdint>
#include <portaudio.h>

namespace llm {
    class Audio {
    private:
        PaStream *stream = nullptr;
        uint64_t _tick = 0;

        static int callback( const void *inputBuffer, void *outputBuffer,
                                   unsigned long framesPerBuffer,
                                   const PaStreamCallbackTimeInfo* timeInfo,
                                   PaStreamCallbackFlags statusFlags,
                                   void *userData )
        {
            /* Cast data passed through stream to our structure. */
            Audio *data = (Audio*)userData;
            float *out = (float*)outputBuffer;
            unsigned int i;
            (void) inputBuffer; /* Prevent unused variable warning. */

            for( i=0; i<framesPerBuffer; i++ )
            {
                float left = 0.0f;
                float right = 0.0f;

                data->tick(left, right, data->_tick++);

                *out++ = left;
                *out++ = right;
            }
            return 0;
        }

    public:
        Audio() {
            Pa_Initialize();
            Pa_OpenDefaultStream( &stream,
                                0,          /* no input channels */
                                2,          /* stereo output */
                                paFloat32,  /* 32 bit floating point output */
                                48000,      /* sample rate */
                                512,        /* frames per buffer */
                                &Audio::callback,
                                this);
        }

        void start() {
            Pa_StartStream( stream );
        }
        void wait() {}

        ~Audio() {
            if (stream) {
                Pa_StopStream(stream);
                Pa_CloseStream(stream);
            }
            Pa_Terminate();
        }

        virtual void tick(float &left, float &right, uint64_t globalTick) { tick(left, right); }
        virtual void tick(float &left, float &right) { left = right = 0; }
    };
};
