//Morgen Hyde

#pragma once

#include <cmath>

const double PI = 4.0 * std::atan(1.0);
const float GAIN_EPSILON = 0.001f;

namespace Milk3D {

	struct Sound3DAttributes {

		void SetPanningAmount(float panningAmount) {

			double panAngle = (PI / 2.0) * panningAmount;

			// Calculate the panning gain amounts using the half power panning law
			// Check if the value is below some threshold and set it to zero below that
			leftGain = static_cast<float>(cos(panAngle));
			if (leftGain < GAIN_EPSILON) {
				leftGain = 0.0f;
			}

			rightGain = static_cast<float>(sin(panAngle));
			if (rightGain < GAIN_EPSILON) {
				rightGain = 0.0f;
			}
		}

		__inline float GetLeftGain() const { return leftGain; }
		__inline float GetRightGain() const { return rightGain; }

	private:
		float leftGain;
		float rightGain;

	};

} // namespace Milk3D
