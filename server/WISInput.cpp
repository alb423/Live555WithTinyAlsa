/*
 * Copyright (C) 2005-2006 WIS Technologies International Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and the associated README documentation file (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
// An interface to the WIS GO7007 capture device.
// Implementation

#include "WISInput.hh"
#include "Options.hh"
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/poll.h>
#include <linux/soundcard.h>
////////// WISOpenFileSource definition //////////

// A common "FramedSource" subclass, used for reading from an open file:

class WISOpenFileSource: public FramedSource
{
protected:
    WISOpenFileSource(UsageEnvironment& env, WISInput& input, int fileNo);
    virtual ~WISOpenFileSource();

    virtual void readFromFile() = 0;

private: // redefined virtual functions:
    virtual void doGetNextFrame();

private:
    static void incomingDataHandler(WISOpenFileSource* source, int mask);
    void incomingDataHandler1();

protected:
    WISInput& fInput;
    int fFileNo;
};


////////// WISAudioOpenFileSource definition //////////

class WISAudioOpenFileSource: public WISOpenFileSource
{
public:
    WISAudioOpenFileSource(UsageEnvironment& env, WISInput& input);
    virtual ~WISAudioOpenFileSource();

protected: // redefined virtual functions:
    virtual void readFromFile();
};


////////// WISInput implementation //////////

WISInput* WISInput::createNew(UsageEnvironment& env)
{
    if (!fHaveInitialized) {
        if (!initialize(env)) {
            return NULL;
        }
        fHaveInitialized = True;
    }

    return new WISInput(env);
}

FramedSource* WISInput::audioSource()
{
    if (fOurAudioSource == NULL) {
        fOurAudioSource = new WISAudioOpenFileSource(envir(), *this);
    }
    return fOurAudioSource;
}

WISInput::WISInput(UsageEnvironment& env)
    : Medium(env)
{
}

WISInput::~WISInput()
{
    struct pcm *pPcm = fOurPCMDevice;
    pcm_close(pPcm);
}

Boolean WISInput::initialize(UsageEnvironment& env)
{
    do {
        if (!openFiles(env)) break;
        return True;
    } while (0);

    // An error occurred
    return False;
}

Boolean WISInput::openFiles(UsageEnvironment& env)
{
    do {

        struct pcm_config config;
        struct pcm *pPcm;

        memset(&config, 0, sizeof(config));
        config.channels = audioNumChannels; //2;
        config.rate = audioSamplingFrequency; //48000;
        config.period_size = 1024;
        config.period_count = 2;
        if (audioFormat == AFMT_PCM_RAW16) {
            // live555 audio format to tinyalsa format
            config.format = PCM_FORMAT_S16_LE;
        }
        config.start_threshold = 1024;
        config.stop_threshold = 1024 * 2;
        config.silence_threshold = 1024 * 2;

        pPcm = pcm_open(0, 0, PCM_IN, &config);
        if (!pPcm) {
            fprintf(stderr, "failed to allocate memory for pcm (%s)\n", pcm_get_error(pPcm));
            return 0;
        }
        if (!pcm_is_ready(pPcm)) {
            fprintf(stderr, "Unable to open PCM device (%s)\n", pcm_get_error(pPcm));
            pcm_close(pPcm);
            return 0;
        }

        fOurAudioFileNo = pcm_get_file_descriptor(pPcm);
        fOurPCMDevice = pPcm;
        //fcntl(fOurAudioFileNo, F_SETFL, O_NONBLOCK);

        return True;
    } while (0);

    // An error occurred:
    return False;
}


Boolean WISInput::fHaveInitialized = False;
int WISInput::fOurAudioFileNo = -1;
FramedSource* WISInput::fOurAudioSource = NULL;
struct pcm* WISInput::fOurPCMDevice = NULL;


////////// WISOpenFileSource implementation //////////

WISOpenFileSource
::WISOpenFileSource(UsageEnvironment& env, WISInput& input, int fileNo)
    : FramedSource(env),
      fInput(input), fFileNo(fileNo)
{
}

WISOpenFileSource::~WISOpenFileSource()
{
    envir().taskScheduler().turnOffBackgroundReadHandling(fFileNo);
}

void WISOpenFileSource::doGetNextFrame()
{
    // Await the next incoming data on our FID:
    envir().taskScheduler().turnOnBackgroundReadHandling(fFileNo,
            (TaskScheduler::BackgroundHandlerProc*)&incomingDataHandler, this);
}

void WISOpenFileSource
::incomingDataHandler(WISOpenFileSource* source, int /*mask*/)
{
    source->incomingDataHandler1();
}

void WISOpenFileSource::incomingDataHandler1()
{
    // Read the data from our file into the client's buffer:
    readFromFile();

    // Stop handling any more input, until we're ready again:
    envir().taskScheduler().turnOffBackgroundReadHandling(fFileNo);

    // Tell our client that we have new data:
    afterGetting(this);
}


////////// WISAudioOpenFileSource implementation //////////

WISAudioOpenFileSource
::WISAudioOpenFileSource(UsageEnvironment& env, WISInput& input)
    : WISOpenFileSource(env, input, input.fOurAudioFileNo)
{
}

WISAudioOpenFileSource::~WISAudioOpenFileSource()
{
    fInput.fOurAudioSource = NULL;
}

void WISAudioOpenFileSource::readFromFile()
{
    // Read available audio data:
    struct pcm *pPcm = fInput.fOurPCMDevice;
    int timeinc;

    //int read_count  = pcm_readi(pPcm, fTo, fMaxSize);   // fMaxSize=21828
    int read_count  = pcm_readi(pPcm, fTo, 4096);
    if (read_count  < 0) read_count  = 0;

    fFrameSize = pcm_frames_to_bytes(pPcm, read_count);
    gettimeofday(&fPresentationTime, NULL);

    /* PR#2665 fix from Robin
     * Assuming audio format = AFMT_S16_LE
     * Get the current time
     * Substract the time increment of the audio oss buffer, which is equal to
     * buffer_size / channel_number / sample_rate / sample_size ==> 400+ millisec
     */
    timeinc = fFrameSize * 1000 / audioNumChannels / (audioSamplingFrequency / 1000) / 2;
    while (fPresentationTime.tv_usec < timeinc) {
        fPresentationTime.tv_sec -= 1;
        timeinc -= 1000000;
    }
    fPresentationTime.tv_usec -= timeinc;
}

