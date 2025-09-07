#pragma once

#include "math.h"

enum class EAAType {
    NONE,
    SSAA,
    MSAA,
    FXAA
};

enum class EPatternType {
    UV,
    CHECKERBOARD,
    CIRCLE,
    VORONOI
};

const vec2f MSAA_SAMPLES[8] = {
    {-0.3125f, -0.4375f}, { 0.1875f, -0.3125f},
    { 0.4375f, -0.0625f}, { 0.0625f,  0.1875f},
    {-0.4375f,  0.0625f}, {-0.0625f,  0.3125f},
    {-0.1875f,  0.4375f}, { 0.3125f,  0.0625f}
};

static vec3f pattern_uv(const vec2f& uv)
{
    return {uv.x, uv.y, 0.0f};
}

static vec3f pattern_checkerboard(const vec2f& uv, const vec2i& size, const float tileSize)
{
    // tileSize: 타일 한 칸의 픽셀 크기(<=0 방지)
    const float step = (tileSize > 0.0f) ? tileSize : 10.0f;

    // 픽셀 좌표
    const vec2f pos = uv * size;

    // 셀 인덱스
    const int cx = static_cast<int>(std::floor(pos.x / step));
    const int cy = static_cast<int>(std::floor(pos.y / step));

    // 패리티로 0/1 결정
    const int parity = (cx + cy) & 1;
    const float c = parity ? 1.0f : 0.0f;

    return {c, c, c};
}

static vec3f pattern_circle(const vec2f& uv, const vec2i& size, const float thickness, const float gap)
{
    const vec2f center = vec2f{0.5f, 0.5f} * size;
    const vec2f pos = uv * size;

    const float dist = length(pos - center);
    const float remainder = std::fmod(dist, (thickness + gap));
    const float parity = remainder > thickness ? 1.f : 0.f;
    return vec3f::One * parity;
}

static vec3f pattern_voronoi(const vec2f& uv, const vec2i& size, const std::vector<vec2f>& points)
{
    const vec2f pos = uv * size;
    
    vec2f closestPoint = points[0];
    float minDist = std::numeric_limits<float>::max();
    for (const vec2f& point : points) {
        float dist = length(pos - point);
        if (dist < minDist) {
            minDist = dist;
            closestPoint = point;
        }
    }
    
    return {closestPoint.x / size.x, closestPoint.y / size.y, 0.f};
    //const float maxDist = 7.f / (length(size));
    //return {0.f, 0.f, 1.f - (minDist * maxDist)};
}

static std::vector<vec3f> generate_uv_pattern_data(const vec2i& outputSize, const EAAType AAType, const int AALevel)
{
    const float AspectRatio = static_cast<float>(outputSize.y) / static_cast<float>(outputSize.x);
   
    std::vector<vec3f> pixels(outputSize.x * outputSize.y);
    const vec2f UVOffset = {0.5f, 0.5f};

    for (int y = 0; y < outputSize.y; y++) {
        for (int x = 0; x < outputSize.x; x++) {
            vec3f accumulatedColor = vec3f::Zero;
            int sampleCount = 1;

            if (AAType == EAAType::SSAA) {
                sampleCount = AALevel * AALevel;
                const float InvAALevel = 1.0f / static_cast<float>(AALevel);
                for (int subY = 0; subY < AALevel; subY++) {
                    for (int subX = 0; subX < AALevel; subX++) {
                        const int superSampleX = x * AALevel + subX;
                        const int superSampleY = y * AALevel + subY;
                        vec2f uv = {
                            (static_cast<float>(superSampleX) + UVOffset.x) * InvAALevel / outputSize.x,
                            (static_cast<float>(superSampleY) + UVOffset.y) * InvAALevel / outputSize.y
                        };
                        uv.y *= AspectRatio;
                        accumulatedColor += pattern_uv(uv);
                    }
                }
            } else { // MSAA or NONE
                sampleCount = (AAType == EAAType::MSAA) ? AALevel : 1;
                for (int i = 0; i < sampleCount; i++) {
                    vec2f offset = (AAType == EAAType::MSAA) ? MSAA_SAMPLES[i] : vec2f();
                    vec2f uv = {
                        (static_cast<float>(x) + UVOffset.x + offset.x) / outputSize.x,
                        (static_cast<float>(y) + UVOffset.y + offset.y) / outputSize.y
                    };
                    uv.y *= AspectRatio;
                    accumulatedColor += pattern_uv(uv);
                }
            }
            pixels[y * outputSize.x + x] = accumulatedColor / static_cast<float>(sampleCount);
        }
    }
    return pixels;
}

static std::vector<vec3f> generate_checkerboard_pattern_data(const vec2i& outputSize, const EAAType AAType, const int AALevel, float angleDegrees, const vec2f& pivot, float tileSize)
{
    const float AspectRatio = static_cast<float>(outputSize.y) / static_cast<float>(outputSize.x);
    const float angle = DegreeToRadian(angleDegrees);
    const mat2f rotation = {std::cos(angle), -std::sin(angle), std::sin(angle), std::cos(angle)};
   
    std::vector<vec3f> pixels(outputSize.x * outputSize.y);
    const vec2f UVOffset = {0.5f, 0.5f};

    vec2i patternSize = outputSize;
    float patternTileSize = tileSize;

    if (AAType == EAAType::SSAA) {
        patternSize = outputSize * AALevel;
        patternTileSize = tileSize * AALevel;
    }
    const vec2i SuperSamplePatternSize = {patternSize.x, patternSize.x};


    for (int y = 0; y < outputSize.y; y++) {
        for (int x = 0; x < outputSize.x; x++) {
            vec3f accumulatedColor = vec3f::Zero;
            int sampleCount = 1;

            if (AAType == EAAType::SSAA) {
                sampleCount = AALevel * AALevel;
                const float InvAALevel = 1.0f / static_cast<float>(AALevel);
                for (int subY = 0; subY < AALevel; subY++) {
                    for (int subX = 0; subX < AALevel; subX++) {
                        const int superSampleX = x * AALevel + subX;
                        const int superSampleY = y * AALevel + subY;
                        vec2f uv = {
                            (static_cast<float>(superSampleX) + UVOffset.x) * InvAALevel / outputSize.x,
                            (static_cast<float>(superSampleY) + UVOffset.y) * InvAALevel / outputSize.y
                        };
                        uv.y *= AspectRatio;
                        uv = uv - vec2f{pivot.x, pivot.y * AspectRatio};
                        uv = rotation * uv;
                        uv = uv + pivot;
                        accumulatedColor += pattern_checkerboard(uv, SuperSamplePatternSize, patternTileSize);
                    }
                }
            } else { // MSAA or NONE
                sampleCount = (AAType == EAAType::MSAA) ? AALevel : 1;
                for (int i = 0; i < sampleCount; i++) {
                    vec2f offset = (AAType == EAAType::MSAA) ? MSAA_SAMPLES[i] : vec2f();
                    vec2f uv = {
                        (static_cast<float>(x) + UVOffset.x + offset.x) / outputSize.x,
                        (static_cast<float>(y) + UVOffset.y + offset.y) / outputSize.y
                    };
                    uv.y *= AspectRatio;
                    uv = uv - vec2f{pivot.x, pivot.y * AspectRatio};
                    uv = rotation * uv;
                    uv = uv + pivot;
                    accumulatedColor += pattern_checkerboard(uv, SuperSamplePatternSize, patternTileSize);
                }
            }
            pixels[y * outputSize.x + x] = accumulatedColor / static_cast<float>(sampleCount);
        }
    }
    return pixels;
}

static std::vector<vec3f> generate_circle_pattern_data(const vec2i& outputSize, const EAAType AAType, const int AALevel, const float thickness, const float gap)
{
    std::vector<vec3f> pixels(outputSize.x * outputSize.y);
    const vec2f UVOffset = {0.5f, 0.5f};

    for (int y = 0; y < outputSize.y; y++) {
        for (int x = 0; x < outputSize.x; x++) {
            vec3f accumulatedColor = vec3f::Zero;
            int sampleCount = 1;

            if (AAType == EAAType::SSAA) {
                sampleCount = AALevel * AALevel;
                const float InvAALevel = 1.0f / static_cast<float>(AALevel);
                const vec2i SuperSampleSize = outputSize * AALevel;
                const float SuperSampleThickness = thickness * AALevel;
                const float SuperSampleGap = gap * AALevel;

                for (int subY = 0; subY < AALevel; subY++) {
                    for (int subX = 0; subX < AALevel; subX++) {
                        const int superSampleX = x * AALevel + subX;
                        const int superSampleY = y * AALevel + subY;
                        const vec2f uv = {
                            (static_cast<float>(superSampleX) + UVOffset.x) * InvAALevel / outputSize.x,
                            (static_cast<float>(superSampleY) + UVOffset.y) * InvAALevel / outputSize.y
                        };
                        accumulatedColor += pattern_circle(uv, SuperSampleSize, SuperSampleThickness, SuperSampleGap);
                    }
                }
            } else if (AAType == EAAType::MSAA) {
                sampleCount = AALevel;
                for (int i = 0; i < sampleCount; i++) {
                    vec2f uv = {
                        (static_cast<float>(x) + UVOffset.x + MSAA_SAMPLES[i].x) / outputSize.x,
                        (static_cast<float>(y) + UVOffset.y + MSAA_SAMPLES[i].y) / outputSize.y
                    };
                    accumulatedColor += pattern_circle(uv, outputSize, thickness, gap);
                }
            } else { // NONE
                vec2f uv = {
                    (static_cast<float>(x) + UVOffset.x) / outputSize.x,
                    (static_cast<float>(y) + UVOffset.y) / outputSize.y
                };
                accumulatedColor = pattern_circle(uv, outputSize, thickness, gap);
            }
            
            pixels[y * outputSize.x + x] = accumulatedColor / static_cast<float>(sampleCount);
        }
    }
    return pixels;
}

static std::vector<vec3f> generate_voronoi_pattern_data(const vec2i& outputSize, const EAAType AAType, const int AALevel, int numPoints)
{
    std::vector<vec3f> pixels(outputSize.x * outputSize.y);
    const vec2f UVOffset = {0.5f, 0.5f};

    vec2i patternSize = outputSize;
    if (AAType == EAAType::SSAA) {
        patternSize = outputSize * AALevel;
    }

    std::vector<vec2f> points;
    points.reserve(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        points.push_back({static_cast<float>(std::rand() % patternSize.x), static_cast<float>(std::rand() % patternSize.y)});
    }

    for (int y = 0; y < outputSize.y; y++) {
        for (int x = 0; x < outputSize.x; x++) {
            vec3f accumulatedColor = vec3f::Zero;
            int sampleCount = 1;

            if (AAType == EAAType::SSAA) {
                sampleCount = AALevel * AALevel;
                const float InvAALevel = 1.0f / static_cast<float>(AALevel);
                for (int subY = 0; subY < AALevel; subY++) {
                    for (int subX = 0; subX < AALevel; subX++) {
                        const int superSampleX = x * AALevel + subX;
                        const int superSampleY = y * AALevel + subY;
                        const vec2f uv = {
                            (static_cast<float>(superSampleX) + UVOffset.x) * InvAALevel / outputSize.x,
                            (static_cast<float>(superSampleY) + UVOffset.y) * InvAALevel / outputSize.y
                        };
                        accumulatedColor += pattern_voronoi(uv, patternSize, points);
                    }
                }
            } else { // MSAA or NONE
                sampleCount = (AAType == EAAType::MSAA) ? AALevel : 1;
                for (int i = 0; i < sampleCount; i++) {
                    vec2f offset = (AAType == EAAType::MSAA) ? MSAA_SAMPLES[i] : vec2f();
                    vec2f uv = {
                        (static_cast<float>(x) + UVOffset.x + offset.x) / outputSize.x,
                        (static_cast<float>(y) + UVOffset.y + offset.y) / outputSize.y
                    };
                    accumulatedColor += pattern_voronoi(uv, patternSize, points);
                }
            }
            pixels[y * outputSize.x + x] = accumulatedColor / static_cast<float>(sampleCount);
        }
    }
    return pixels;
}


static std::vector<vec3f> apply_fxaa(const vec2i& outputSize, const std::vector<vec3f>& pixels)
{
    const vec3f LUMA_COEFF = {0.299f, 0.587f, 0.114f};
    std::vector<float> luma(outputSize.x * outputSize.y);
    for (int i = 0; i < pixels.size(); ++i) {
        luma[i] = dot(pixels[i], LUMA_COEFF);
    }

    std::vector<vec3f> edgePixels(outputSize.x * outputSize.y);
    constexpr float edgeThreshold = 0.001f;

    for (int y = 1; y < outputSize.y - 1; ++y) {
        for (int x = 1; x < outputSize.x - 1; ++x) {
            const int index = y * outputSize.x + x;

            const float lumaCenter = luma[index];
            const float lumaNorth = luma[index - outputSize.x];
            const float lumaSouth = luma[index + outputSize.x];
            const float lumaWest = luma[index - 1];
            const float lumaEast = luma[index + 1];

            const float lumaMin = std::min({lumaCenter, lumaNorth, lumaSouth, lumaWest, lumaEast});
            const float lumaMax = std::max({lumaCenter, lumaNorth, lumaSouth, lumaWest, lumaEast});

            const float deltaLuma = lumaMax - lumaMin;

            if (deltaLuma > edgeThreshold) {
                const float lumaNW = luma[index - outputSize.x - 1];
                const float lumaNE = luma[index - outputSize.x + 1];
                const float lumaSW = luma[index + outputSize.x - 1];
                const float lumaSE = luma[index + outputSize.x + 1];

                const float deltaX = std::abs((lumaNW + lumaSW) - (lumaNE + lumaSE));
                const float deltaY = std::abs((lumaNW + lumaNE) - (lumaSW + lumaSE));

                const bool isHorizontal = deltaX > deltaY;
                const vec2i dir = isHorizontal ? vec2i(1, 0) : vec2i(0, 1);

                float distForward = 0.f;
                float distBackward = 0.f;

                for (int i = 1; i < 10; ++i) {
                    const int nextIndex = index + (dir.y * outputSize.x + dir.x) * i;
                    if (nextIndex < 0 || nextIndex >= luma.size()) break;
                    if (std::abs(luma[nextIndex] - lumaCenter) > edgeThreshold) {
                        distForward = static_cast<float>(i);
                        break;
                    }
                }

                for (int i = 1; i < 10; ++i) {
                    const int prevIndex = index - (dir.y * outputSize.x + dir.x) * i;
                    if (prevIndex < 0 || prevIndex >= luma.size()) break;
                    if (std::abs(luma[prevIndex] - lumaCenter) > edgeThreshold) {
                        distBackward = static_cast<float>(i);
                        break;
                    }
                }

                const float totalDist = distForward + distBackward;
                const float pixelOffset = (distForward - distBackward) / (2.f * totalDist) - 0.5f;

                const int blendIndex = index + (dir.y * outputSize.x + dir.x) * static_cast<int>(pixelOffset);
                if (blendIndex >= 0 && blendIndex < pixels.size()) {
                    edgePixels[index] = (pixels[index] + pixels[blendIndex]) * 0.5f;
                } else {
                    edgePixels[index] = pixels[index];
                }

            } else {
                edgePixels[index] = pixels[index];
            }
        }
    }
    return edgePixels;
}