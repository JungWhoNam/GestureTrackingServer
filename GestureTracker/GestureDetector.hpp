#pragma once

#include <string>
#include <limits.h>

#include <k4a/k4a.h>
#include <k4abt.h>

#include "json.hpp"

namespace GestureTracker {
    int getClosestBodyIndex(k4abt_frame_t body_frame) {
        size_t num_bodies = k4abt_frame_get_num_bodies(body_frame);
        float minDist = FLT_MAX;
        int bodyIndex = -1;

        for (size_t i = 0; i < num_bodies; i++) {
            k4abt_skeleton_t skeleton;
            k4a_result_t skeleton_result = k4abt_frame_get_body_skeleton(body_frame, i, &skeleton);
            if (skeleton_result == K4A_RESULT_SUCCEEDED) {
                float dist = skeleton.joints[K4ABT_JOINT_HAND_LEFT].position.xyz.z;
                if (dist < minDist) {
                    minDist = dist;
                    bodyIndex = i;
                }
            }
        }

        return bodyIndex;
    }

    nlohmann::json createMessageJson(k4abt_frame_t body_frame) {
        int idx = getClosestBodyIndex(body_frame);
        if (idx < 0) return {};
        
        nlohmann::ordered_json j;
        k4abt_skeleton_t skeleton;
        k4a_result_t skeleton_result = k4abt_frame_get_body_skeleton(body_frame, idx, &skeleton);
        if (skeleton_result == K4A_RESULT_SUCCEEDED) {
            for (int i = 0; i < K4ABT_JOINT_COUNT; i++) {
                k4a_float3_t pos = skeleton.joints[i].position;
                j[i]["pos"] = { pos.v[0], pos.v[1], pos.v[2] };
                j[i]["conf"] = skeleton.joints[i].confidence_level;
            }
        }

        return j;
    }

    std::string createMessage(k4abt_frame_t body_frame) {
        nlohmann::json j = createMessageJson(body_frame);
        return j.dump();
    }
}