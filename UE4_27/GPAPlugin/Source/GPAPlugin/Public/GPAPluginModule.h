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

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

THIRD_PARTY_INCLUDES_START
#include <igpa-shim-loader.h>
#include <igpa-config.h>
THIRD_PARTY_INCLUDES_END

DECLARE_LOG_CATEGORY_EXTERN(GPAPlugin, Log, All);

class FToolBarBuilder;
class FMenuBuilder;

class FGPAPluginModule : public IModuleInterface
{
public:
	FGPAPluginModule() : gpa(nullptr), bAllThirdPartyLibsLoaded(false), bStreamCaptureRunning(false) {};
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();
	
private:
	/** pointer to GPA interface, use GetGPAInterface to retrieve it**/
	IGPA* gpa;

	bool bAllThirdPartyLibsLoaded;
	bool bStreamCaptureRunning;

	/** Handles to the third party dlls that were set for delayed loading**/
	TArray<void*> ThirdPartyLibraryHandles;

	/** Plugin commands defined in FGPAPluginCommand**/
	TSharedPtr<class FUICommandList> PluginCommands;

	/** Loads all dlls required by the GPA API capture tool**/
	void LoadThirdPartyLibraries();
	/** Releases GPA related libraries**/
	void FreeThirdPartyLibraries();
	/** Callback for stream capture event**/
	void CaptureStream(const TArray<FString>& Args);
	/** Function handling on screen notification**/
	void ShowNotification(const FString& Info);
	/** Check if Graphics Monitor is running**/
	bool IsGraphicsMonitorProcessRunning(const FString& AppName);
	/** Start Graphics Monitor as a new process**/
	void StartGraphicsMonitorProcess();

	void RegisterMenus();
};
