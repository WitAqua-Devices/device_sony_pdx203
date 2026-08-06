/*
 * Copyright (C) 2026 The WitAqua Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string>

#include <android-base/logging.h>

#include "sku.h"

namespace {

/*
 * pdx203 ships as one image for every SKU, so build.prop describes the global
 * XQ-AT52. That is a model Sony never sold with FeliCa hardware, and anything
 * that identifies the device from its build - the Osaifu-Keitai provisioning
 * among others - sees a phone that should not have an eSE at all.
 *
 * LTALabel tells us which variant this actually is, so on the japanese ones
 * put back the identity of the stock firmware. Values are taken verbatim from
 * the stock builds:
 *
 *   SOG01   58.2.C.5.10  (KDDI)
 *   SO-51A  58.2.B.0.520 (docomo)
 *
 * The SoftBank variant is deliberately absent - there is no dump to copy the
 * fingerprint from, and a made up one is worse than the global one. (It is
 * also missing from the model list in sm8250-common's sku.cpp, which only
 * knows A002SO, the Xperia 5 II SoftBank model.) Anything not listed here
 * keeps the global identity and only picks up its ro.product.model from the
 * shared LTALabel detection.
 *
 * Note this also moves ro.product.device onto the japanese name. OTA packages
 * stay installable because BoardConfig.mk lists those names in
 * TARGET_OTA_ASSERT_DEVICE (and board-info.txt does the same for fastboot).
 */
struct Variant {
    const char* model;
    const char* brand;
    const char* name;
    const char* device;
    const char* id;
    const char* incremental;
};

constexpr Variant kVariants[] = {
        {
                .model = "SOG01",
                .brand = "KDDI",
                .name = "SOG01_jp_kdi",
                .device = "SOG01",
                .id = "58.2.C.5.10",
                .incremental = "058002C005001000415763700",
        },
        {
                .model = "SO-51A",
                .brand = "docomo",
                .name = "SO-51A",
                .device = "SO-51A",
                .id = "58.2.B.0.520",
                .incremental = "058002B000052002813269650",
        },
};

constexpr const char* kPartitions[] = {"odm", "product", "system", "system_ext", "vendor"};

/* The stock builds are all Android 12 user builds signed with release keys. */
constexpr char kRelease[] = "12";
constexpr char kTags[] = "release-keys";
constexpr char kType[] = "user";

void SetIdentity(const Variant& v) {
    const std::string fingerprint = std::string(v.brand) + "/" + v.name + "/" + v.device + ":" +
                                    kRelease + "/" + v.id + "/" + v.incremental + ":" + kType + "/" +
                                    kTags;
    const std::string description = std::string(v.name) + "-" + kType + " " + kRelease + " " + v.id +
                                    " " + v.incremental + " " + kTags;

    /*
     * property_derive_build_fingerprint() only composes ro.build.fingerprint
     * when it is still empty, so setting it here is enough to keep it. The
     * per-partition copies come straight out of build.prop and have to be
     * overwritten one by one.
     */
    sony::SetProperty("ro.build.fingerprint", fingerprint);
    sony::SetProperty("ro.bootimage.build.fingerprint", fingerprint);
    sony::SetProperty("ro.build.description", description);
    for (const char* partition : kPartitions) {
        sony::SetProperty(std::string("ro.") + partition + ".build.fingerprint", fingerprint);
    }

    /*
     * Same story as ro.product.model in the shared code: writing the bare
     * ro.product.* property makes property_initialize_ro_product_props() skip
     * deriving it, so the per-partition ones need setting too.
     */
    sony::SetProperty("ro.product.brand", v.brand);
    sony::SetProperty("ro.product.name", v.name);
    sony::SetProperty("ro.product.device", v.device);
    for (const char* partition : kPartitions) {
        const std::string prefix = std::string("ro.product.") + partition + ".";
        sony::SetProperty(prefix + "brand", v.brand);
        sony::SetProperty(prefix + "name", v.name);
        sony::SetProperty(prefix + "device", v.device);
    }
}

}  // namespace

void vendor_load_properties() {
    std::string model = sony::DetectLtaModel();
    if (model.empty()) {
        LOG(INFO) << "libinit: no japanese model in LTALabel, keeping the built-in identity";
        return;
    }

    LOG(INFO) << "libinit: detected " << model;
    sony::SetModelProperties(model);

    for (const Variant& v : kVariants) {
        if (model == v.model) {
            LOG(INFO) << "libinit: using the stock " << v.model << " build identity";
            SetIdentity(v);
            return;
        }
    }

    LOG(INFO) << "libinit: no stock build identity for " << model << ", keeping the built-in one";
}
