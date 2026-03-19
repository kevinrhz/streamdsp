#pragma once
#include <memory>
#include <vector>
#include "processing_stage.hpp"

namespace stx {

class StageFactory {
public:
    enum class StageType { WINDOW, FFT, MAGNITUDE };
    static std::unique_ptr<ProcessingStage> create(StageType type);
    static std::vector<std::unique_ptr<ProcessingStage>> create_default_pipeline();
};

}  // namespace stx
