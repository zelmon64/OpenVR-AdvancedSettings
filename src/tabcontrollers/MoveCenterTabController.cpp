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

int MoveCenterTabController::pitch() const {
	return m_pitch;
}

int MoveCenterTabController::roll() const {
	return m_roll;
}

void MoveCenterTabController::setRotation(int value, bool notify) {
	if (m_rotation != value) {
		float angle = (value - m_rotation) * 2 * M_PI / 360.0;
		parent->RotateUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 1, angle, m_adjustChaperone);
		m_rotation = value;
		if (notify) {
			emit rotationChanged(m_rotation);
		}
	}
}

void MoveCenterTabController::setPitch(int value, bool notify) {
	if (m_pitch != value) {
		float angle = (value - m_pitch) * 2 * M_PI / 360.0;
		parent->RotateUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 0, angle, m_adjustChaperone);
		m_pitch = value;
		if (notify) {
			emit pitchChanged(m_pitch);
		}
	}
}

void MoveCenterTabController::setRoll(int value, bool notify) {
	if (m_roll != value) {
		float angle = (value - m_roll) * 2 * M_PI / 360.0;
		parent->RotateUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 2, angle, m_adjustChaperone);
		m_roll = value;
		if (notify) {
			emit rollChanged(m_roll);
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
	parent->RotateUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 1, -m_rotation * 2 * M_PI / 360.0, m_adjustChaperone, false);
	parent->RotateUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 0, -m_pitch * 2 * M_PI / 360.0, m_adjustChaperone, false);
	parent->RotateUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 2, -m_roll * 2 * M_PI / 360.0, m_adjustChaperone, false);
	parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 0, -m_offsetX, m_adjustChaperone, false);
	parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 1, -m_offsetY, m_adjustChaperone, false);
	parent->AddOffsetToUniverseCenter((vr::TrackingUniverseOrigin)m_trackingUniverse, 2, -m_offsetZ, m_adjustChaperone, false);
	vr::VRChaperoneSetup()->CommitWorkingCopy(vr::EChaperoneConfigFile_Live);
	m_offsetX = 0.0f;
	m_offsetY = 0.0f;
	m_offsetZ = 0.0f;
	m_rotation = 0;
	m_pitch = 0;
	m_roll = 0;
	emit offsetXChanged(m_offsetX);
	emit offsetYChanged(m_offsetY);
	emit offsetZChanged(m_offsetZ);
	emit rotationChanged(m_rotation);
	emit pitchChanged(m_pitch);
	emit rollChanged(m_roll);
}

void MoveCenterTabController::eventLoopTick(vr::ETrackingUniverseOrigin universe) {
	if (settingsUpdateCounter >= 50) {
		setTrackingUniverse((int)universe);
		settingsUpdateCounter = 0;
	} else {
		settingsUpdateCounter++;
	}
}

} // namespace advconfig
