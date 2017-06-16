#include "MoveCenterTabController.h"
#include <QQuickWindow>
#include "../overlaycontroller.h"

// application namespace
namespace advsettings {

void MoveCenterTabController::initStage1() {
	setTrackingUniverse(vr::VRCompositor()->GetTrackingSpace());
	auto settings = OverlayController::appSettings();
	settings->beginGroup("playspaceSettings");
	auto value = settings->value("adjustChaperone", m_adjustChaperone);
	settings->endGroup();
	if (value.isValid() && !value.isNull()) {
		m_adjustChaperone = value.toBool();
	}
}

void MoveCenterTabController::initStage2(OverlayController * parent, QQuickWindow * widget) {
	this->parent = parent;
	this->widget = widget;
}


int MoveCenterTabController::trackingUniverse() const {
	return (int)m_trackingUniverse;
}

void MoveCenterTabController::setTrackingUniverse(int value, bool notify) {
	if (m_trackingUniverse != value) {
		reset();
		m_trackingUniverse = value;
		if (notify) {
			emit trackingUniverseChanged(m_trackingUniverse);
		}
	}
}

float MoveCenterTabController::offsetX() const {
	return m_offsetX;
}

void MoveCenterTabController::setOffsetX(float value, bool notify) {
	if (m_offsetX != value) {
		modOffsetX(value - m_offsetX, notify);
	}
}

float MoveCenterTabController::offsetY() const {
	return m_offsetY;
}

void MoveCenterTabController::setOffsetY(float value, bool notify) {
	if (m_offsetY != value) {
		modOffsetY(value - m_offsetY, notify);
	}
}

float MoveCenterTabController::offsetZ() const {
	return m_offsetZ;
}

void MoveCenterTabController::setOffsetZ(float value, bool notify) {
	if (m_offsetZ != value) {
		modOffsetZ(value - m_offsetZ, notify);
	}
}

int MoveCenterTabController::rotation() const {
	return m_rotation;
}

void MoveCenterTabController::setRotation(int value, bool notify) {
	if (m_rotation != value) {
		float angle = (value - m_rotation) * 2 * M_PI / 360.0;
		//parent->RotateUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, angle, m_adjustChaperone);
		parent->RotateHMD((vr::TrackingUniverseOrigin)m_trackingUniverse, angle, m_adjustChaperone);
		m_rotation = value;
		if (notify) {
			emit rotationChanged(m_rotation);
		}
	}
}

bool MoveCenterTabController::adjustChaperone() const {
	return m_adjustChaperone;
}

void MoveCenterTabController::setAdjustChaperone(bool value, bool notify) {
	if (m_adjustChaperone != value) {
		m_adjustChaperone = value;
		if (m_trackingUniverse == vr::TrackingUniverseStanding) {
			vr::VRChaperoneSetup()->RevertWorkingCopy();
			float offsetdir;
			if (m_adjustChaperone) {
				offsetdir = -1.0;
			} else {
				offsetdir = 1.0;
			}

			if (m_offsetX != 0.0f) {
				if (m_adjustChaperone || m_rotation == 0) {
					parent->AddOffsetToCollisionBounds(0, offsetdir * m_offsetX, false);
				} else {
					auto angle = m_rotation * 2 * M_PI / 360.0;
					parent->AddOffsetToCollisionBounds(0, offsetdir * m_offsetX * std::cos(angle), false);
					parent->AddOffsetToCollisionBounds(2, offsetdir * m_offsetX * std::sin(angle), false);
				}
			}
			if (m_offsetY != 0.0f) {
				parent->AddOffsetToCollisionBounds(1, offsetdir * m_offsetY, false);
			}
			if (m_offsetZ != 0.0f) {
				if (m_adjustChaperone || m_rotation == 0) {
					parent->AddOffsetToCollisionBounds(2, offsetdir * m_offsetZ, false);
				} else {
					auto angle = m_rotation * 2 * M_PI / 360.0;
					parent->AddOffsetToCollisionBounds(2, offsetdir * m_offsetZ * std::cos(angle), false);
					parent->AddOffsetToCollisionBounds(0, -offsetdir * m_offsetZ * std::sin(angle), false);
				}
			}
			if (m_rotation != 0) {
				float angle = m_rotation * 2 * M_PI / 360.0;
				parent->RotateCollisionBounds(offsetdir * angle, false);
			}
			vr::VRChaperoneSetup()->CommitWorkingCopy(vr::EChaperoneConfigFile_Live);
		}
		auto settings = OverlayController::appSettings();
		settings->beginGroup("playspaceSettings");
		settings->setValue("adjustChaperone", m_adjustChaperone);
		settings->endGroup();
		settings->sync();
		if (notify) {
			emit adjustChaperoneChanged(m_adjustChaperone);
		}
	}
}

void MoveCenterTabController::modOffsetX(float value, bool notify) {
	if (m_rotation == 0) {
		parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 0, value, m_adjustChaperone);
	} else {
		auto angle = m_rotation * 2 * M_PI / 360.0;
		vr::VRChaperoneSetup()->RevertWorkingCopy();
		parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 0, value * std::cos(angle), m_adjustChaperone, false);
		parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 2, value * std::sin(angle), m_adjustChaperone, false);
		vr::VRChaperoneSetup()->CommitWorkingCopy(vr::EChaperoneConfigFile_Live);
	}
	m_offsetX += value;
	if (notify) {
		emit offsetXChanged(m_offsetX);
	}
}

void MoveCenterTabController::modOffsetY(float value, bool notify) {
	parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 1, value, m_adjustChaperone);
	m_offsetY += value;
	if (notify) {
		emit offsetYChanged(m_offsetY);
	}
}

void MoveCenterTabController::modOffsetZ(float value, bool notify) {
	if (m_rotation == 0) {
		parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 2, value, m_adjustChaperone);
	} else {
		auto angle = m_rotation * 2 * M_PI / 360.0;
		vr::VRChaperoneSetup()->RevertWorkingCopy();
		parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 2, value * std::cos(angle), m_adjustChaperone, false);
		parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 0, -value * std::sin(angle), m_adjustChaperone, false);
		vr::VRChaperoneSetup()->CommitWorkingCopy(vr::EChaperoneConfigFile_Live);
	}
	m_offsetZ += value;
	if (notify) {
		emit offsetZChanged(m_offsetZ);
	}
}

void MoveCenterTabController::reset() {
	vr::VRChaperoneSetup()->RevertWorkingCopy();
	//parent->RotateUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, -m_rotation * 2 * M_PI / 360.0, m_adjustChaperone, false);
	parent->RotateHMD((vr::TrackingUniverseOrigin)m_trackingUniverse, -m_rotation * 2 * M_PI / 360.0, m_adjustChaperone, false);
	parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 0, -m_offsetX, m_adjustChaperone, false);
	parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 1, -m_offsetY, m_adjustChaperone, false);
	parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 2, -m_offsetZ, m_adjustChaperone, false);
	vr::VRChaperoneSetup()->CommitWorkingCopy(vr::EChaperoneConfigFile_Live);
	m_offsetX = 0.0f;
	m_offsetY = 0.0f;
	m_offsetZ = 0.0f;
	m_rotation = 0;
	emit offsetXChanged(m_offsetX);
	emit offsetYChanged(m_offsetY);
	emit offsetZChanged(m_offsetZ);
	emit rotationChanged(m_rotation);
}

void MoveCenterTabController::drag_workd() {
	auto leftId = vr::VRSystem()->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
	auto rightId = vr::VRSystem()->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);
	if (rightId != vr::k_unTrackedDeviceIndexInvalid) {
		vr::VRControllerState_t state;
		vr::ETrackingUniverseOrigin eOrigin = vr::ETrackingUniverseOrigin::TrackingUniverseRawAndUncalibrated;
		vr::TrackedDevicePose_t pose;
		//virtual bool GetControllerStateWithPose( ETrackingUniverseOrigin eOrigin, vr::TrackedDeviceIndex_t unControllerDeviceIndex, vr::VRControllerState_t *pControllerState, uint32_t unControllerStateSize, TrackedDevicePose_t *pTrackedDevicePose ) = 0;
		if (vr::VRSystem()->GetControllerStateWithPose(eOrigin, rightId, &state, sizeof(vr::VRControllerState_t), &pose))
		{
			//logControllerState(state, "Left ");
			//newState |= handleControllerState(state, m_pttControllerConfigs);
			if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger)
				|| state.ulButtonTouched & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger)) 
			//if (state.ulButtonPressed & vr::ButtonMaskFromId((vr::EVRButtonId)10)
			//	|| state.ulButtonTouched & vr::ButtonMaskFromId((vr::EVRButtonId)10)) 
			{
				if (!m_buttonwaspressed) {
					m_startpose = pose;
					m_buttonwaspressed = true;
				}
				else {
					//vr::TrackedDevicePose_t newpose = pose;
					//float valueX = (pose.mDeviceToAbsoluteTracking.m[0][3] - m_startpose.mDeviceToAbsoluteTracking.m[0][3]);
					//float valueY = drag_mult * (pose.mDeviceToAbsoluteTracking.m[1][3] - m_startpose.mDeviceToAbsoluteTracking.m[1][3]);
					//float valueZ = (pose.mDeviceToAbsoluteTracking.m[2][3] - m_startpose.mDeviceToAbsoluteTracking.m[2][3]);

					switch (m_grabfunction)
					{
					case GrabFunction::Translate:
					{
						float drag_mult = 10.f;
						float valueXab = drag_mult * (pose.mDeviceToAbsoluteTracking.m[0][3] - m_startpose.mDeviceToAbsoluteTracking.m[0][3]);
						float valueY = drag_mult * (pose.mDeviceToAbsoluteTracking.m[1][3] - m_startpose.mDeviceToAbsoluteTracking.m[1][3]);
						float valueZab = drag_mult * (pose.mDeviceToAbsoluteTracking.m[2][3] - m_startpose.mDeviceToAbsoluteTracking.m[2][3]);
						
						auto angle = m_rotation * 2 * M_PI / 360.0;

						float valueX = std::cos(angle) * valueXab - std::sin(angle) * valueZab;
						float valueZ = std::sin(angle) * valueXab + std::cos(angle) * valueZab;
						
						//valueX *= drag_mult;
						//valueZ *= drag_mult;
						//m_rotation = 0;

						parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 0, valueX, true);
						parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 1, valueY, true);
						parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 2, valueZ, true);

						//m_startpose = pose;
						m_offsetX += valueX;
						m_offsetY += valueY;
						m_offsetZ += valueZ;
						//*
						emit offsetXChanged(m_offsetX);
						emit offsetYChanged(m_offsetY);
						emit offsetZChanged(m_offsetZ);
						//*/
					} break;
					case GrabFunction::Rotate:
					{
						//vr::TrackedDevicePose_t currHmdPose = vr::Compositor_FrameTiming::m_HmdPose;
						//vr::TrackedDevicePose_t currHmdPose = vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(ETrackingUniverseOrigin eOrigin, float fPredictedSecondsToPhotonsFromNow, VR_ARRAY_COUNT(unTrackedDevicePoseArrayCount) TrackedDevicePose_t *pTrackedDevicePoseArray, uint32_t unTrackedDevicePoseArrayCount);

						vr::TrackedDevicePose_t HMDPose;
						//vr::TrackedDevicePose_t *devicePose = &trackedDevicePose;

						vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(
							eOrigin, 0, &HMDPose, 1);
						
						/*
						float angle_n0 = pose.mDeviceToAbsoluteTracking.m[0][3] * m_startpose.mDeviceToAbsoluteTracking.m[0][3];
						float angle_n2 = pose.mDeviceToAbsoluteTracking.m[2][3] * m_startpose.mDeviceToAbsoluteTracking.m[2][3];
						
						float angle_dp = (sqrt(pose.mDeviceToAbsoluteTracking.m[0][3] * pose.mDeviceToAbsoluteTracking.m[0][3]
							+ pose.mDeviceToAbsoluteTracking.m[2][3] * pose.mDeviceToAbsoluteTracking.m[2][3]));
						float angle_ds = sqrt(m_startpose.mDeviceToAbsoluteTracking.m[0][3] * m_startpose.mDeviceToAbsoluteTracking.m[0][3]
							+ m_startpose.mDeviceToAbsoluteTracking.m[2][3] * m_startpose.mDeviceToAbsoluteTracking.m[2][3]);
						
						float angle = (angle_n0 + angle_n2) / (angle_dp * angle_ds);
						/*

						float Ax = m_startpose.mDeviceToAbsoluteTracking.m[0][3];
						float Az = m_startpose.mDeviceToAbsoluteTracking.m[2][3];
						float Bx = pose.mDeviceToAbsoluteTracking.m[0][3];
						float Bz = pose.mDeviceToAbsoluteTracking.m[2][3];

						/*

						float Ax = m_startpose.mDeviceToAbsoluteTracking.m[0][3] - HMDPose.mDeviceToAbsoluteTracking.m[0][3];
						float Az = m_startpose.mDeviceToAbsoluteTracking.m[2][3] - HMDPose.mDeviceToAbsoluteTracking.m[2][3];
						float Bx = pose.mDeviceToAbsoluteTracking.m[0][3] - HMDPose.mDeviceToAbsoluteTracking.m[0][3];
						float Bz = pose.mDeviceToAbsoluteTracking.m[2][3] - HMDPose.mDeviceToAbsoluteTracking.m[2][3];

						float angle = (Bx*Bz + Ax*Az - Ax*Bx - Az*Bz - Ax*Bz - Az*Bx)
							/ (sqrt(Ax*Ax + Az*Az) * sqrt(Bx*Bx + Bz*Bz));
						//*/

						float startposeX = m_startpose.mDeviceToAbsoluteTracking.m[0][3] - HMDPose.mDeviceToAbsoluteTracking.m[0][3];
						float startposeZ = m_startpose.mDeviceToAbsoluteTracking.m[2][3] - HMDPose.mDeviceToAbsoluteTracking.m[2][3];
						float currposeX = pose.mDeviceToAbsoluteTracking.m[0][3] - HMDPose.mDeviceToAbsoluteTracking.m[0][3];
						float currposeZ = pose.mDeviceToAbsoluteTracking.m[2][3] - HMDPose.mDeviceToAbsoluteTracking.m[2][3];
						// cos(c) = A dot B  / ||A|| x ||B||
						float angle_nX = currposeX * startposeX;
						float angle_nZ = currposeZ * startposeZ;

						float angle_dC = (std::sqrt(currposeX * currposeX + currposeZ * currposeZ));
						float angle_dS = std::sqrt(startposeX * startposeX + startposeZ * startposeZ);

						float angle = (angle_nX + angle_nZ) / (angle_dC * angle_dS);

						angle = (angle > 1) ? 0 : std::acos(angle);

						float dposeX = currposeX - startposeX;
						float dposeZ = currposeZ - startposeZ;
						
						if (startposeZ < 0 && (startposeX > startposeZ || startposeX < -startposeZ))
						{
							if (dposeX > 0) angle *= -1;
						}
						else angle = 0;

						/*
						if (dposeX > 0)
						{
							if (dposeZ > 0)
							{
								if (startposeX < 0 && startposeZ < 0) angle *= 1;
								if (startposeX > 0 && startposeZ > 0) angle *= -1;
							}
							else
							{
								if (startposeX < 0 && startposeZ > 0) angle *= -1;
								if (startposeX > 0 && startposeZ < 0) angle *= 1;
							}
						}
						else
						{
							if (dposeZ > 0)
							{
								if (startposeX < 0 && startposeZ < 0) angle *= -1;
								if (startposeX > 0 && startposeZ > 0) angle *= 1;
							}
							else
							{
								if (startposeX < 0 && startposeZ > 0) angle *= 1;
								if (startposeX > 0 && startposeZ < 0) angle *= -1;
							}
						}
						*/
						//float angle_limit = 0.001f;
						//if (angle > angle_limit) angle = angle_limit;
						//else if (angle < -angle_limit) angle = -angle_limit;

						//angle = std::sqrt(startposeX * startposeX + startposeZ * startposeZ);

						//angle = startposeX - currposeX;

						//if (startposeZ > 0) angle = 0.001;
						//else angle = -0.001;
						
						//float angle = 1.f * (m_startpose.mDeviceToAbsoluteTracking.m[0][3] - pose.mDeviceToAbsoluteTracking.m[0][3]);
						//	 + pose.mDeviceToAbsoluteTracking.m[2][3] - m_startpose.mDeviceToAbsoluteTracking.m[2][3]);

						m_rotation += angle * 180.0 / M_PI;
						if (m_rotation > 180) 
						{
							m_rotation -= 360;
							angle -= 2 * M_PI;
						}
						else if (m_rotation < -180)
						{
							m_rotation += 360;
							angle += 2 * M_PI;
						}

						// * 2 * M_PI / 360.0
						//parent->RotateUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, angle, true);
						parent->RotateHMD((vr::TrackingUniverseOrigin)m_trackingUniverse, angle, true);
						emit rotationChanged(m_rotation);
					} break;
					}

					m_startpose = pose;
				}
			}
			else if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu)
				|| state.ulButtonTouched & vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu))
			{
				switch (m_grabfunction)
				{
				case GrabFunction::Translate:
				{
					parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 0, -m_offsetX, true);
					parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 1, -m_offsetY, true);
					parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 2, -m_offsetZ, true);
					m_offsetX = 0.0f;
					m_offsetY = 0.0f;
					m_offsetZ = 0.0f;
					emit offsetXChanged(m_offsetX);
					emit offsetYChanged(m_offsetY);
					emit offsetZChanged(m_offsetZ);
				} break;
				case GrabFunction::Rotate:
				{
					// * 2 * M_PI / 360.0
					//parent->RotateUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, -m_rotation * 2 * M_PI / 360.0, true);
					parent->RotateHMD((vr::TrackingUniverseOrigin)m_trackingUniverse, -m_rotation * 2 * M_PI / 360.0, true);
					m_rotation = 0;
					emit rotationChanged(m_rotation);
				} break;
				}
			}
			else if (state.ulButtonPressed & vr::ButtonMaskFromId((vr::EVRButtonId)15)
				|| state.ulButtonTouched & vr::ButtonMaskFromId((vr::EVRButtonId)15))
			{
				if (!m_buttonwaspressed) 
				{
					m_buttonwaspressed = true;
					float crouchvalue = 1.f;

					if (!m_crouching)
					{
						m_crouching = true;
						parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 1, crouchvalue, true);
						m_offsetY += crouchvalue;
						emit offsetYChanged(m_offsetY);
					}
					else
					{
						m_crouching = false;
						parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 1, -crouchvalue, true);
						m_offsetY -= crouchvalue;
						emit offsetYChanged(m_offsetY);
					}
				}
			}
			else if (state.ulButtonPressed & vr::ButtonMaskFromId((vr::EVRButtonId)8)
				|| state.ulButtonTouched & vr::ButtonMaskFromId((vr::EVRButtonId)8))
			{
				if (!m_buttonwaspressed) {
					m_changegrabfunctionpresstime = std::chrono::high_resolution_clock::now();
					m_buttonwaspressed = true;
					m_buttonwasheld = false;
				}
				else if (!m_buttonwasheld) {
					const float k_hold_duration_milli = 1000.f;
					std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
					std::chrono::duration<float, std::milli> pressDurationMilli = now - m_changegrabfunctionpresstime;

					if (pressDurationMilli.count() >= k_hold_duration_milli) {
						switch (m_grabfunction)
						{
						case GrabFunction::Translate:
						{
							m_grabfunction = GrabFunction::Rotate;
						} break;
						case GrabFunction::Rotate:
						{
							//m_grabfunction = GrabFunction::Translate;
							m_grabfunction = GrabFunction::None;
						} break;
						case GrabFunction::None:
						{
							m_grabfunction = GrabFunction::Translate;
						} break;
						}
						m_buttonwasheld = true;
					}
				}
			}
			else {
				m_buttonwaspressed = false;
			}
		}
	}
	if (leftId != vr::k_unTrackedDeviceIndexInvalid) {
		vr::VRControllerState_t state;
		vr::ETrackingUniverseOrigin eOrigin = vr::ETrackingUniverseOrigin::TrackingUniverseRawAndUncalibrated;
		vr::TrackedDevicePose_t pose;
		//virtual bool GetControllerStateWithPose( ETrackingUniverseOrigin eOrigin, vr::TrackedDeviceIndex_t unControllerDeviceIndex, vr::VRControllerState_t *pControllerState, uint32_t unControllerStateSize, TrackedDevicePose_t *pTrackedDevicePose ) = 0;
		if (vr::VRSystem()->GetControllerStateWithPose(eOrigin, leftId, &state, sizeof(vr::VRControllerState_t), &pose))
		{
			//logControllerState(state, "Left ");
			//newState |= handleControllerState(state, m_pttControllerConfigs);
			if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger)
				|| state.ulButtonTouched & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger))
				//if (state.ulButtonPressed & vr::ButtonMaskFromId((vr::EVRButtonId)10)
				//	|| state.ulButtonTouched & vr::ButtonMaskFromId((vr::EVRButtonId)10)) 
			{
				if (!m_leftbuttonwaspressed) {
					m_leftstartpose = pose;
					m_leftbuttonwaspressed = true;
				}
				else {
					//vr::TrackedDevicePose_t newpose = pose;
					//float valueX = (pose.mDeviceToAbsoluteTracking.m[0][3] - m_startpose.mDeviceToAbsoluteTracking.m[0][3]);
					//float valueY = drag_mult * (pose.mDeviceToAbsoluteTracking.m[1][3] - m_startpose.mDeviceToAbsoluteTracking.m[1][3]);
					//float valueZ = (pose.mDeviceToAbsoluteTracking.m[2][3] - m_startpose.mDeviceToAbsoluteTracking.m[2][3]);

					switch (m_leftgrabfunction)
					{
					case GrabFunction::Translate:
					{
						float drag_mult = 10.f;
						float valueXab = drag_mult * (pose.mDeviceToAbsoluteTracking.m[0][3] - m_startpose.mDeviceToAbsoluteTracking.m[0][3]);
						float valueY = drag_mult * (pose.mDeviceToAbsoluteTracking.m[1][3] - m_startpose.mDeviceToAbsoluteTracking.m[1][3]);
						float valueZab = drag_mult * (pose.mDeviceToAbsoluteTracking.m[2][3] - m_startpose.mDeviceToAbsoluteTracking.m[2][3]);

						auto angle = m_rotation * 2 * M_PI / 360.0;

						float valueX = std::cos(angle) * valueXab - std::sin(angle) * valueZab;
						float valueZ = std::sin(angle) * valueXab + std::cos(angle) * valueZab;

						parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 0, valueX, true);
						parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 1, valueY, true);
						parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 2, valueZ, true);

						m_offsetX += valueX;
						m_offsetY += valueY;
						m_offsetZ += valueZ;
						emit offsetXChanged(m_offsetX);
						emit offsetYChanged(m_offsetY);
						emit offsetZChanged(m_offsetZ);
					} break;
					case GrabFunction::Rotate:
					{
						
						vr::TrackedDevicePose_t HMDPose;

						vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(
							eOrigin, 0, &HMDPose, 1);

						
						float startposeX = m_startpose.mDeviceToAbsoluteTracking.m[0][3] - HMDPose.mDeviceToAbsoluteTracking.m[0][3];
						float startposeZ = m_startpose.mDeviceToAbsoluteTracking.m[2][3] - HMDPose.mDeviceToAbsoluteTracking.m[2][3];
						float currposeX = pose.mDeviceToAbsoluteTracking.m[0][3] - HMDPose.mDeviceToAbsoluteTracking.m[0][3];
						float currposeZ = pose.mDeviceToAbsoluteTracking.m[2][3] - HMDPose.mDeviceToAbsoluteTracking.m[2][3];
						// cos(c) = A dot B  / ||A|| x ||B||
						float angle_nX = currposeX * startposeX;
						float angle_nZ = currposeZ * startposeZ;

						float angle_dC = (std::sqrt(currposeX * currposeX + currposeZ * currposeZ));
						float angle_dS = std::sqrt(startposeX * startposeX + startposeZ * startposeZ);

						float angle = (angle_nX + angle_nZ) / (angle_dC * angle_dS);

						angle = (angle > 1) ? 0 : std::acos(angle);

						float dposeX = currposeX - startposeX;
						float dposeZ = currposeZ - startposeZ;

						if (startposeZ < 0 && (startposeX > startposeZ || startposeX < -startposeZ))
						{
							if (dposeX > 0) angle *= -1;
						}
						else angle = 0;

						m_rotation += angle * 180.0 / M_PI;
						if (m_rotation > 180)
						{
							m_rotation -= 360;
							angle -= 2 * M_PI;
						}
						else if (m_rotation < -180)
						{
							m_rotation += 360;
							angle += 2 * M_PI;
						}

						parent->RotateHMD((vr::TrackingUniverseOrigin)m_trackingUniverse, angle, true);
						emit rotationChanged(m_rotation);
					} break;
					}

					m_leftstartpose = pose;
				}
			}
			else if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu)
				|| state.ulButtonTouched & vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu))
			{
				switch (m_leftgrabfunction)
				{
				case GrabFunction::Translate:
				{
					parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 0, -m_offsetX, true);
					parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 1, -m_offsetY, true);
					parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 2, -m_offsetZ, true);
					m_offsetX = 0.0f;
					m_offsetY = 0.0f;
					m_offsetZ = 0.0f;
					emit offsetXChanged(m_offsetX);
					emit offsetYChanged(m_offsetY);
					emit offsetZChanged(m_offsetZ);
				} break;
				case GrabFunction::Rotate:
				{
					parent->RotateHMD((vr::TrackingUniverseOrigin)m_trackingUniverse, -m_rotation * 2 * M_PI / 360.0, true);
					m_rotation = 0;
					emit rotationChanged(m_rotation);
				} break;
				}
			}
			else if (state.ulButtonPressed & vr::ButtonMaskFromId((vr::EVRButtonId)15)
				|| state.ulButtonTouched & vr::ButtonMaskFromId((vr::EVRButtonId)15))
			{
				if (!m_leftbuttonwaspressed)
				{
					m_leftbuttonwaspressed = true;
					float crouchvalue = 1.f;

					if (!m_crouching)
					{
						m_crouching = true;
						parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 1, crouchvalue, true);
						m_offsetY += crouchvalue;
						emit offsetYChanged(m_offsetY);
					}
					else
					{
						m_crouching = false;
						parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 1, -crouchvalue, true);
						m_offsetY -= crouchvalue;
						emit offsetYChanged(m_offsetY);
					}
				}
			}
			else if (state.ulButtonPressed & vr::ButtonMaskFromId((vr::EVRButtonId)8)
				|| state.ulButtonTouched & vr::ButtonMaskFromId((vr::EVRButtonId)8))
			{
				if (!m_leftbuttonwaspressed) {
					m_leftchangegrabfunctionpresstime = std::chrono::high_resolution_clock::now();
					m_leftbuttonwaspressed = true;
					m_leftbuttonwasheld = false;
				}
				else if (!m_leftbuttonwasheld) {
					const float k_hold_duration_milli = 1000.f;
					std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
					std::chrono::duration<float, std::milli> pressDurationMilli = now - m_leftchangegrabfunctionpresstime;

					if (pressDurationMilli.count() >= k_hold_duration_milli) {
						switch (m_leftgrabfunction)
						{
						case GrabFunction::Translate:
						{
							m_leftgrabfunction = GrabFunction::Rotate;
						} break;
						case GrabFunction::Rotate:
						{
							//m_grabfunction = GrabFunction::Translate;
							m_leftgrabfunction = GrabFunction::None;
						} break;
						case GrabFunction::None:
						{
							m_leftgrabfunction = GrabFunction::Translate;
						} break;
						}
						m_leftbuttonwasheld = true;
					}
				}
			}
			else {
				m_leftbuttonwaspressed = false;
			}
		}
	}

}

void MoveCenterTabController::eventLoopTick(vr::ETrackingUniverseOrigin universe) {
	if (settingsUpdateCounter >= 50) {
		setTrackingUniverse((int)universe);
		settingsUpdateCounter = 0;
	} else {
		settingsUpdateCounter++;
	}
	drag_workd();
}

} // namespace advconfig
