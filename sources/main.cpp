#include <iostream>

#include <vector>
#include <string>

#include "ppm.h"
#include "pattern.h"

int main(int argc, char* argv[]) {
    if (argc > 1 && (std::string(argv[1]) == "help" || std::string(argv[1]) == "--help")) {
        std::cout << "Usage: " << argv[0] << " [width] [height] [aa_type] [aa_level] [pattern_type] [output_file]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  width:        Output image width (default: 1920)" << std::endl;
        std::cout << "  height:       Output image height (default: 1080)" << std::endl;
        std::cout << "  aa_type:      ssaa, msaa, fxaa (default: msaa)" << std::endl;
        std::cout << "  aa_level:     1-8 (default: 2)" << std::endl;
        std::cout << "  pattern_type: uv, checkerboard, circle, voronoi (default: voronoi)" << std::endl;
        std::cout << "  output_file:  Optional output file name" << std::endl;
        return 0;
    }
    
    vec2i OutputSize = {1920, 1080};
    if (argc > 2) {
        OutputSize.x = std::atoi(argv[1]);
        OutputSize.y = std::atoi(argv[2]);
    }

    EAAType AAType = EAAType::MSAA;
    if (argc > 3) {
        std::string AATypeStr = argv[3];
        if (AATypeStr == "ssaa") {
            AAType = EAAType::SSAA;
        } else if (AATypeStr == "msaa") {
            AAType = EAAType::MSAA;
        } else if (AATypeStr == "fxaa") {
            AAType = EAAType::FXAA;
        }
    }

    int AALevel = 2;
    if (argc > 4) {
        AALevel = std::atoi(argv[4]);
        if (AALevel < 1) AALevel = 1;
        if (AALevel > 8) AALevel = 8;
    }

    EPatternType patternType = EPatternType::VORONOI;
    if (argc > 5) {
        std::string patternTypeStr = argv[5];
        if (patternTypeStr == "uv") {
            patternType = EPatternType::UV;
        } else if (patternTypeStr == "checkerboard") {
            patternType = EPatternType::CHECKERBOARD;
        } else if (patternTypeStr == "circle") {
            patternType = EPatternType::CIRCLE;
        } else if (patternTypeStr == "voronoi") {
            patternType = EPatternType::VORONOI;
        }
    }

    std::string FileName = "output";
    FileName += "_" + std::to_string(OutputSize.x) + "x" + std::to_string(OutputSize.y);
    
    switch (AAType) {
        case EAAType::SSAA: FileName += "_SSAA"; break;
        case EAAType::MSAA: FileName += "_MSAA"; break;
        case EAAType::FXAA: FileName += "_FXAA"; break;
        default: break;
    }

    FileName += "_" + std::to_string(AALevel);
    FileName += ".ppm";

    std::string outputFile = FileName;
    if (argc > 6) {
        outputFile = argv[6];
    }


    std::vector<vec3f> pixels;
    switch (patternType) {
        case EPatternType::UV:
            pixels = generate_uv_pattern_data(OutputSize, AAType, AALevel);
            break;
        case EPatternType::CHECKERBOARD:
            pixels = generate_checkerboard_pattern_data(OutputSize, AAType, AALevel, 40.0f, {0.5f, 0.5f}, 50.f);
            break;
        case EPatternType::CIRCLE:
            pixels = generate_circle_pattern_data(OutputSize, AAType, AALevel, 12.f, 7.f);
            break;
        case EPatternType::VORONOI:
            pixels = generate_voronoi_pattern_data(OutputSize, AAType, AALevel, 100);
            break;
    }

    if (AAType == EAAType::FXAA) {
        // Apply FXAA
        pixels = apply_fxaa(OutputSize, pixels);
    }

    unsigned char* data = new unsigned char[OutputSize.x * OutputSize.y * 3];
    for (int i = 0; i < OutputSize.x * OutputSize.y; ++i) {
        data[i * 3 + 0] = static_cast<unsigned char>(pixels[i].x * 255);    
        data[i * 3 + 1] = static_cast<unsigned char>(pixels[i].y * 255);
        data[i * 3 + 2] = static_cast<unsigned char>(pixels[i].z * 255);
    }

    ExportPPM(outputFile.c_str(), EPPMFormat::P3_BINARY, OutputSize.x, OutputSize.y, data);

    delete[] data;

    return 0;
}