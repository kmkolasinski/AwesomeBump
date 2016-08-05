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

#ifndef DEBUGMETRICSMONITOR_H
#define DEBUGMETRICSMONITOR_H

#include <QObject>
#include <QTime>
#include <QVariant>

/**
 * @ingroup Engine
 * @brief Tracks and provides mechanisms for reporting various debug metrics.
 *
 * Current metrics that are supported include: frames rendered per second and application memory
 * usage.
 * @note In order for DebugMetricsMonitor to report correctly, update() must be called every frame.
 */
class DebugMetricsMonitor : public QObject {
    Q_OBJECT

    /**
     * @brief Average number of frames rendered per second, averaged over #fpsAverageWindowSizeMs
     * milliseconds and updated every #fpsRefreshIntervalMs milliseconds.
     */
    Q_PROPERTY(float fps READ getFps NOTIFY fpsChanged)
    /**
     * @brief Size of window, in milliseconds, over which to average #fps.
     */
    Q_PROPERTY(int fpsAverageWindowSizeMs READ getFpsAverageWindowSizeMs
            WRITE setFpsAverageWindowSizeMs NOTIFY fpsAverageWindowSizeMsChanged)
    /**
     * @brief Length of interval, in milliseconds, after which to refresh the #fps metric.
     */
    Q_PROPERTY(int fpsRefreshIntervalMs READ getFpsRefreshIntervalMs WRITE setFpsRefreshIntervalMs
               NOTIFY fpsRefreshIntervalMsChanged)
    /**
     * @brief The most immediate, non-averaged measurement of the number of frames rendered per
     * second that is updated each frame.
     */
    Q_PROPERTY(float instantFps READ getInstantFps)
    /**
     * @brief Amount of memory, in bytes, used by the application.
     */
#ifdef DOXYGEN
    Q_PROPERTY(long int memoryUsage READ getMemoryUsage NOTIFY memoryUsageChanged)
#else
    Q_PROPERTY(long memoryUsage READ getMemoryUsage NOTIFY memoryUsageChanged)
#endif
    /**
     * @brief Length of interval, in milliseconds, after which to refresh the #memoryUsage metric.
     */
    Q_PROPERTY(int memoryUsageRefreshIntervalMs READ getMemoryUsageRefreshIntervalMs
               WRITE setMemoryUsageRefreshIntervalMs NOTIFY memoryUsageRefreshIntervalMsChanged)
    /**
     * @brief Whether or not DebugMetricsMonitor logs debug information to @c qDebug.
     * @note Currently only the #fps metric is logged.
     */
    Q_PROPERTY(bool loggingMetrics READ isLoggingMetrics WRITE setLoggingMetrics
               NOTIFY loggingMetricsChanged)

public:
    /**
     * @brief Constructs a DebugMetricsMonitor.
     * @param parent Parent object
     */
    explicit DebugMetricsMonitor(QObject* parent = 0);

    /**
     * @brief Returns #fps.
     */
    float getFps() const { return mFps; }
    /**
     * @brief Returns #fpsAverageWindowSizeMs.
     */
    int getFpsAverageWindowSizeMs() const { return mFpsAverageWindowSizeMs; }
    /**
     * @brief Sets #fpsAverageWindowSizeMs.
     * @param value Integer to set #fpsAverageWindowSizeMs to
     */
    void setFpsAverageWindowSizeMs(int value);
    /**
     * @brief Returns #fpsRefreshIntervalMs.
     */
    int getFpsRefreshIntervalMs() const { return mFpsRefreshIntervalMs; }
    /**
     * @brief Sets #fpsRefreshIntervalMs.
     * @param value Integer to set #fpsRefreshIntervalMs to
     */
    void setFpsRefreshIntervalMs(int value);
    /**
     * @brief Returns #instantFps.
     */
    float getInstantFps() const { return mInstantFps; }
    /**
     * @brief Returns #memoryUsage.
     */
    long getMemoryUsage() const { return mMemoryUsage; }
    /**
     * @brief Returns #memoryUsageRefreshIntervalMs.
     */
    int getMemoryUsageRefreshIntervalMs() const { return mMemoryUsageRefreshIntervalMs; }
    /**
     * @brief Sets #memoryUsageRefreshIntervalMs.
     * @param value Integer to set #memoryUsageRefreshIntervalMs to
     */
    void setMemoryUsageRefreshIntervalMs(int value);
    /**
     * @brief Returns #loggingMetrics.
     */
    bool isLoggingMetrics() const { return mLoggingMetrics; }
    /**
     * @brief Sets #loggingMetrics.
     * @param value Boolean to set #loggingMetrics to
     */
    void setLoggingMetrics(bool value);

public slots:
    /**
     * @brief Updates the DebugMetricsMonitor metrics if more than their refresh interval times have
     * passed since they were last refreshed.
     * @note This function should be called each frame to ensure accurate metrics.
     */
    void update();

signals:
    /**
     * @brief Emitted when #fps changes.
     */
    void fpsChanged();
    /**
     * @brief Emitted when #fpsAverageWindowSizeMs changes.
     */
    void fpsAverageWindowSizeMsChanged();
    /**
     * @brief Emitted when #fpsRefreshIntervalMs changes.
     */
    void fpsRefreshIntervalMsChanged();
    /**
     * @brief Emitted when #memoryUsage changes.
     */
    void memoryUsageChanged();
    /**
     * @brief Emitted when #memoryUsageRefreshIntervalMs changes.
     */
    void memoryUsageRefreshIntervalMsChanged();
    /**
     * @brief Emitted when #loggingMetrics changes.
     */
    void loggingMetricsChanged();

private:
    void updateFps();
    void updateMemoryUsage();

    float mFps = 0.0f;
    int mFpsAverageWindowSizeMs = 1000;
    int mFpsRefreshIntervalMs = 100;
    float mInstantFps = 0.0f;
    QTime mFpsRefreshTime;
    int mFpsCounterCount = 0;
    QTime mFpsCounterTimer;
    QTime mInstantFpsCounterTimer;

    long mMemoryUsage = 0;
    int mMemoryUsageRefreshIntervalMs = 1000;
    QTime mMemoryUsageRefreshTime;

    bool mLoggingMetrics = true;
};

#endif // DEBUGMETRICSMONITOR_H
