/**
 * @file stage_factory.cpp
 * @brief Factory for creating processing stages.
 *
 * YOUR TASK: Implement create() and create_default_pipeline().
 * Mirrors: cpp/src/core/processing_stage.cpp StageFactory
 */

#include "stage_factory.hpp"
#include <stdexcept>

namespace stx {

std::unique_ptr<ProcessingStage> StageFactory::create(StageType type) {
    // TODO: switch on type, return make_unique<WindowStage/FFTStage/MagnitudeStage>
    throw std::runtime_error("StageFactory::create not implemented");
}

std::vector<std::unique_ptr<ProcessingStage>> StageFactory::create_default_pipeline() {
    // TODO: return [WindowStage, FFTStage, MagnitudeStage]
    throw std::runtime_error("StageFactory::create_default_pipeline not implemented");
}

}  // namespace stx
