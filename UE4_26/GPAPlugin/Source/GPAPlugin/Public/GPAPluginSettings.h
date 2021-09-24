/*******************************************************************************
 * Copyright 2021 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

#pragma once

#include "Engine/DeveloperSettings.h"
#include "GPAPluginSettings.generated.h"

UCLASS(config = Engine, defaultconfig, meta = (DisplayName = "GPA"))
class GPAPLUGIN_API UGPAPluginSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, Category = "General", meta = (
		ConsoleVariable = "gpa.BinaryLocation", DisplayName = "GPA binary location",
		ToolTip = "Path that will be used to locate GPA Framework binaries, typically C:\\Program Files\\IntelSWTools\\GPA Framework\\<version>\\bin\\Release",
		ConfigRestartRequired = true))
		FString GPABinaryPath;
	// TODO - Frame capture count is planned for future update
	/*
	UPROPERTY(config, EditAnywhere, Category = "Stream Capture Settings", meta = (
		ConsoleVariable = "gpa.FrameCaptureCount", DisplayName = "Number of frames to be captured",
		ToolTip = "If 0 than capture will run until explicitely stopped, otherwise it will autmatically stop after reaching specified number of frames",
		ClampMin = 0,
		ConfigRestartRequired = false))
		uint32 FrameCaptureCount;
	*/
	UPROPERTY(config, EditAnywhere, Category = "Stream Capture Settings", meta = (
		ConsoleVariable = "gpa.RunGPAAfterCapture", DisplayName = "Run GPA after capture complete",
		ToolTip = "If checked the GPA UI will automatically start after the capture is complete.",
		ConfigRestartRequired = false))
		bool bRunGPAAfterCapture;

public:
	virtual void PostInitProperties() override;
	virtual FName GetCategoryName() const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
