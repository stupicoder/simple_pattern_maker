#pragma once

#include <cstdio>

enum class EPPMFormat
{
    P3_ASCII,
    P3_BINARY
};

bool ExportPPM(const char* filename, EPPMFormat format, int width, int height, const unsigned char* data)
{
    if (filename == nullptr || data == nullptr || width <= 0 || height <= 0) 
    {
        return false;
    }

    FILE* file = nullptr;
    if (fopen_s(&file, filename, (format == EPPMFormat::P3_ASCII) ? "w" : "wb") != 0 || file == nullptr)
    {
        return false;
    }

    fprintf(file, (format == EPPMFormat::P3_ASCII) ? "P3\n%d %d\n255\n" : "P6\n%d %d\n255\n", width, height);
    if (format == EPPMFormat::P3_ASCII) 
    {
        for (int i = 0; i < width * height * 3; ++i) 
        {
            fprintf(file, "%d ", data[i]);
        }
    } 
    else 
    {
        // For binary format, we write the raw data directly
        fwrite(data, 1, width * height * 3, file);
    }
    fclose(file);
    return true;
}