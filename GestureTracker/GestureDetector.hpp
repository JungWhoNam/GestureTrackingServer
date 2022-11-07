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
            {
                k4a_float3_t pos = skeleton.joints[K4ABT_JOINT_HAND_LEFT].position;
                j["HAND_LEFT"] = { pos.v[0], pos.v[1], pos.v[2] };
            }
            {
                k4a_float3_t pos = skeleton.joints[K4ABT_JOINT_HAND_RIGHT].position;
                j["HAND_RIGHT"] = { pos.v[0], pos.v[1], pos.v[2] };
            }
            {
                k4a_float3_t pos = skeleton.joints[K4ABT_JOINT_SPINE_CHEST].position;
                j["SPINE_CHEST"] = { pos.v[0], pos.v[1], pos.v[2] };
            }
        }

        return j;
    }

    std::string createMessage(k4abt_frame_t body_frame) {
        nlohmann::json j = createMessageJson(body_frame);
        return j.dump();
    }
}