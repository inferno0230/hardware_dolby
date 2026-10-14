/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "vendor.dolby.media.c2@1.0-service"

#include <csignal>
#include <cstdlib>
#include <memory>

#include <android-base/logging.h>
#include <binder/ProcessState.h>
#include <codec2/hidl/1.0/ComponentStore.h>
#include <hidl/HidlTransportSupport.h>

namespace android {
// Provided by libcodec2_store_dolby
std::shared_ptr<C2ComponentStore> GetCodec2DolbyComponentStore();
}  // namespace android

int main() {
    using android::hardware::media::c2::V1_0::utils::ComponentStore;

    // The prebuilt service allocates ComponentStore with a stale size, so it
    // is constructed here against this tree's libcodec2_hidl instead.
    LOG(DEBUG) << "Dolby Codec2 service starting";

    signal(SIGPIPE, SIG_IGN);

    android::ProcessState::initWithDriver("/dev/vndbinder");
    android::ProcessState::self()->startThreadPool();
    android::hardware::configureRpcThreadpool(8, true);

    auto implementation = android::GetCodec2DolbyComponentStore();
    if (implementation == nullptr) {
        LOG(ERROR) << "Dolby component store factory returned null";
        return EXIT_FAILURE;
    }

    auto service = android::sp<ComponentStore>::make(implementation);
    if (service->status() != C2_OK) {
        LOG(ERROR) << "Cannot create Dolby Codec2 service: " << static_cast<int>(service->status());
        return EXIT_FAILURE;
    }

    android::status_t status = service->registerAsService("default1");
    if (status != android::OK) {
        LOG(ERROR) << "Cannot register Dolby Codec2 service: " << status;
        return EXIT_FAILURE;
    }

    android::hardware::joinRpcThreadpool();
    return EXIT_FAILURE;
}
