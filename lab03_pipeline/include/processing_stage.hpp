/**
 * @file processing_stage.hpp
 * @brief Abstract interface for pipeline stages (Strategy pattern) + concrete declarations:
 * WindowStage, FFTStage, MagnitudeStage.
 *
 * The interface is given. The concrete stages are implemented in src/*.cpp using Pimpl.
 */
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "stage_config.hpp"

namespace stx {

class ProcessingStage {
public:
    virtual ~ProcessingStage() = default;
    virtual void initialize(const StageConfig& config) = 0;
    virtual void process(const float* input, float* output, size_t num_elements) = 0;
    virtual std::string name() const = 0;
    virtual bool supports_inplace() const = 0;
};

class WindowStage : public ProcessingStage {
public:
    WindowStage();
    ~WindowStage();
    void initialize(const StageConfig& config) override;
    void process(const float* input, float* output, size_t num_elements) override;
    std::string name() const override { return "WindowStage"; }
    bool supports_inplace() const override { return true; }
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

class FFTStage : public ProcessingStage {
public:
    FFTStage();
    ~FFTStage();
    void initialize(const StageConfig& config) override;
    // Output is interleaved complex pairs: [re0, im0, re1, im1, ...]
    // Output size = num_output_bins * channels * 2 floats
    void process(const float* input, float* output, size_t num_elements) override;
    std::string name() const override { return "FFTStage"; }
    bool supports_inplace() const override { return false; }
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

class MagnitudeStage : public ProcessingStage {
public:
    MagnitudeStage();
    ~MagnitudeStage();
    void initialize(const StageConfig& config) override;
    // Input is interleaved complex pairs, output is magnitudes
    void process(const float* input, float* output, size_t num_elements) override;
    std::string name() const override { return "MagnitudeStage"; }
    bool supports_inplace() const override { return false; }
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

}  // namespace stx
