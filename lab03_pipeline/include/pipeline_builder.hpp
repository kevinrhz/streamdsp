#pragma once
#include <memory>
#include <string>
#include <vector>
#include "processing_stage.hpp"
#include "stage_config.hpp"

namespace stx {

class PipelineBuilder {
public:
    PipelineBuilder();
    ~PipelineBuilder();

    PipelineBuilder& with_config(const StageConfig& config);
    PipelineBuilder& add_window(StageConfig::WindowType type);
    PipelineBuilder& add_fft();
    PipelineBuilder& add_magnitude();

    bool validate(std::string& error_msg) const;
    std::vector<std::unique_ptr<ProcessingStage>> build();
    size_t num_stages() const;
    void clear();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

}  // namespace stx
