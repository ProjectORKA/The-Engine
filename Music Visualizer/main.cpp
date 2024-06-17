#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <iostream>
#include <vector>

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    float* pFloatOutput = (float*)pOutput;
    float* pBuffer = (float*)pDevice->pUserData;
    static size_t bufferOffset = 0;

    size_t totalFrames = *((size_t*)pDevice->pUserData + 1);
    size_t framesToRead = (bufferOffset + frameCount > totalFrames) ? totalFrames - bufferOffset : frameCount;

    memcpy(pFloatOutput, pBuffer + bufferOffset, framesToRead * sizeof(float) * 2); // 2 for stereo
    bufferOffset += framesToRead;

    if (framesToRead < frameCount) { // If end of buffer reached, loop or stop
        memset(pFloatOutput + framesToRead * 2, 0, (frameCount - framesToRead) * sizeof(float) * 2);
        ma_device_stop(pDevice); // Stop the device if you only want to play it once
    }
}

int main() {
    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

    // Initialize decoder
    result = ma_decoder_init_file("sound.mp3", NULL, &decoder);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to load MP3 file." << std::endl;
        return -1;
    }

    // Prepare device config
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate        = decoder.outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = NULL;

    // Initialize device
    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        std::cerr << "Failed to open playback device." << std::endl;
        ma_decoder_uninit(&decoder);
        return -2;
    }

    // Decode entire MP3 to a float buffer
    std::vector<float> buffer(decoder.outputFrameCount * decoder.outputChannels);
    ma_decoder_read_pcm_frames(&decoder, buffer.data(), decoder.outputFrameCount);

    // Set buffer and totalFrames in user data for callback
    size_t totalFrames = decoder.outputFrameCount;
    float* userData = new float[2]; // Allocate user data
    userData[0] = *((float*)&buffer[0]); // Cast buffer data to float*
    userData[1] = *((float*)&totalFrames); // Cast totalFrames to float*
    device.pUserData = userData;

    // Start playback
    ma_device_start(&device);

    std::cout << "Playback started. Press Enter to exit..." << std::endl;
    std::cin.ignore();

    // Clean up
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
    delete[] userData;

    return 0;
}