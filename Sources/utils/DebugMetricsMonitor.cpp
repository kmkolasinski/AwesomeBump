/*
 * Copyright (C) 2014 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <QDebug>
#include "DebugMetricsMonitor.h"

#if defined(Q_OS_ANDROID)
#include "android/AndroidActivity.h"
#elif defined(Q_OS_MACX)
#include <mach/mach.h>
#elif defined(Q_OS_UNIX)
#include <QFile>
#elif defined(Q_OS_WIN)
#include <QFile>
#endif

DebugMetricsMonitor::DebugMetricsMonitor(QObject* parent) : QObject(parent) {
    mFpsRefreshTime.start();
    mMemoryUsageRefreshTime.start();
    mFpsCounterTimer.start();
    mInstantFpsCounterTimer.start();
}

void DebugMetricsMonitor::update() {
    // Engine::UPDATE_FREQUENCY should be in line with rendering frame rate

    mFpsCounterCount++;
    if (mFpsCounterTimer.elapsed() > mFpsAverageWindowSizeMs) {
        int intervalMillis = mFpsCounterTimer.restart();
        mFps = mFpsCounterCount * mFpsAverageWindowSizeMs / (float) intervalMillis;
        mFpsCounterCount = 0;
    }
    mInstantFps = 1000.0f / mInstantFpsCounterTimer.restart();

    if (mFpsRefreshTime.elapsed() >= mFpsRefreshIntervalMs) {
        updateFps();
        mFpsRefreshTime.restart();
    }
    if (mMemoryUsageRefreshTime.elapsed() >= mMemoryUsageRefreshIntervalMs) {
        updateMemoryUsage();
        mMemoryUsageRefreshTime.restart();
    }
}

void DebugMetricsMonitor::updateFps() {
    emit fpsChanged();
    // TODO: Decide if we want to log FPS details here
}

void DebugMetricsMonitor::updateMemoryUsage() {
#if defined(Q_OS_ANDROID)
    mMemoryUsage = AndroidActivity::getMemoryUsed();
#elif defined(Q_OS_MACX)
    struct task_basic_info taskInfo;
    mach_msg_type_number_t taskInfoCount = TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t) &taskInfo, &taskInfoCount)
            != KERN_SUCCESS)
    {
        qDebug() << "Kernel error when retrieving basic task info.";
        mMemoryUsage = -1;
    } else {
        // NOTE: On Mac OSX, the resident_size and virtual_size is in bytes, not pages,
        // which differs from the GNU Mach kernel.
        mMemoryUsage = taskInfo.resident_size;
    }
#elif defined(Q_OS_UNIX) // Guaranteed not to be Android or Mac

#else
    QFile procInfoFile("/proc/self/statm");
    procInfoFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString procInfo = procInfoFile.readAll();
    procInfoFile.close();
    // Memory info prints number of resident 4KB memory pages in the second column
    // See: http://man7.org/linux/man-pages/man5/proc.5.html
    // NOTE: This number is not accurate past the nearest 4KB.
    mMemoryUsage = procInfo.split("\\s")[1].toLong() * 4 * 1000;
#endif

    if (mLoggingMetrics) {
        qDebug() << "Memory Usage:" << (mMemoryUsage / 1000000.0) << "MB";
    }
    emit memoryUsageChanged();
}

void DebugMetricsMonitor::setFpsAverageWindowSizeMs(int value) {
    mFpsAverageWindowSizeMs = value;
    emit fpsAverageWindowSizeMsChanged();
}

void DebugMetricsMonitor::setFpsRefreshIntervalMs(int value) {
    mFpsRefreshIntervalMs = value;
    emit fpsRefreshIntervalMsChanged();
}

void DebugMetricsMonitor::setMemoryUsageRefreshIntervalMs(int value) {
    mMemoryUsageRefreshIntervalMs = value;
    emit memoryUsageRefreshIntervalMsChanged();
}

void DebugMetricsMonitor::setLoggingMetrics(bool value) {
    mLoggingMetrics = value;
    emit loggingMetricsChanged();
}
