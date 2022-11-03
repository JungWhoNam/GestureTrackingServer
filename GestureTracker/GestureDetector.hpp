#pragma once

#include <string>
#include <limits.h>

#include <k4a/k4a.h>
#include <k4abt.h>

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

    std::string createMessage(k4abt_frame_t body_frame) {
        int idx = getClosestBodyIndex(body_frame);

        if (idx < 0) return "NONE";

        std::string msg;
        k4abt_skeleton_t skeleton;
        k4a_result_t skeleton_result = k4abt_frame_get_body_skeleton(body_frame, idx, &skeleton);
        if (skeleton_result == K4A_RESULT_SUCCEEDED) {
            {
                k4abt_joint_id_t jointID = K4ABT_JOINT_HAND_LEFT;
                k4a_float3_t pos = skeleton.joints[jointID].position;
                msg += std::to_string(pos.v[0]) + "," +std::to_string(pos.v[1]) + "," +std::to_string(pos.v[2]);
            }
            msg += ",";
            {
                k4abt_joint_id_t jointID = K4ABT_JOINT_HAND_RIGHT;
                k4a_float3_t pos = skeleton.joints[jointID].position;
                msg += std::to_string(pos.v[0]) + "," +std::to_string(pos.v[1]) + "," +std::to_string(pos.v[2]);
            }
        }

        return msg;
    }
}