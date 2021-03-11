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
 
#include "GPAPluginModule.h"
#include "GPAPluginStyle.h"
#include "GPAPluginCommands.h"
#include "Misc/MessageDialog.h"
#include "Interfaces/IPluginManager.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "ToolMenus.h"

#include "Windows/AllowWindowsPlatformTypes.h"
// needed for starting Graphics Monitor process
#include <shellapi.h>
#include <psapi.h>

static const FName GPAPluginTabName("GPAPlugin");

#define LOCTEXT_NAMESPACE "FGPAPluginModule"

DEFINE_LOG_CATEGORY(GPAPlugin);

// register console variables that are exposed and can be modifiedin the project settings
static TAutoConsoleVariable<FString> CVarGPABinaryLocation(
	TEXT("gpa.BinaryLocation"),
	TEXT(""),
	TEXT("Path that will be used to locate GPA Framework binaries."));

// TODO - Frame capture count is planned for future update
/*
static TAutoConsoleVariable<int32> CVarGPAFrameCaptureCount(
	TEXT("gpa.FrameCaptureCount"),
	0,
	TEXT("	0: capture will run until stopped.")
	TEXT("	>0: capture automatically stop after reaching specified number of frames."));
*/
static TAutoConsoleVariable<int32> CVarGPARunGPAAfterCapture(
	TEXT("gpa.RunGPAAfterCapture"),
	0,
	TEXT("	0: GPA UI will not be run after capture is.")
	TEXT("	1: GPA UI will automatically start after the capture is complete."));

void FGPAPluginModule::LoadThirdPartyLibraries()
{
	FString LibraryPath = CVarGPABinaryLocation.GetValueOnAnyThread();
	// order is important, igpa-shim-loader-x64.dll depends on previous dlls
	TArray<FString> ThirdPartyDlls = { "logger-x64.dll" ,
									   "runtime-x64.dll",
									   "igpa-shim-loader-x64.dll"
									 };

	// Verify that location in ini file is correct, if not try to use path from registry entry
	if (!FPaths::FileExists(FPaths::Combine(LibraryPath, ThirdPartyDlls[0])))
	{
		UE_LOG(GPAPlugin, Warning, TEXT("Directory \"%s\" from ini configuration file is not a valid GPA directory. Will try using registry entry."), *LibraryPath);

		// try path from registry entry
		FString RegSubKey = TEXT("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment");
		FWindowsPlatformMisc::QueryRegKey(HKEY_LOCAL_MACHINE, *RegSubKey, TEXT("INTEL_GPA_FRAMEWORK"), LibraryPath);
		LibraryPath = FPaths::Combine(LibraryPath, TEXT("bin\\Release"));

		if (!FPaths::FileExists(FPaths::Combine(LibraryPath, ThirdPartyDlls[0])))
		{
			UE_LOG(GPAPlugin, Warning, TEXT("Could not find a valid Intel(R) Graphics Performance Analyzers tool location, please verify installation."));
			// all attempts failed, quit the dll load
			return;
		}
		else
		{
			UE_LOG(GPAPlugin, Log, TEXT("Found valid GPA directory: %s."), *LibraryPath);
		}
	}

	// update console variable to the correct path
	CVarGPABinaryLocation.AsVariable()->Set(*LibraryPath, ECVF_SetByProjectSetting);

	for (FString& DllName : ThirdPartyDlls)
	{
		FString DllPath = FPaths::Combine(LibraryPath, DllName);
		void* DllHandle = FPlatformProcess::GetDllHandle(*DllPath);
		if (DllHandle == nullptr)
		{
			UE_LOG(GPAPlugin, Warning, TEXT("Failed to load GPA capture library %s. Install latest GPA version."), *DllPath);
			return;
		}

		ThirdPartyLibraryHandles.Add(DllHandle);
	}

	// if we made it this far indicate all libs have loaded
	bAllThirdPartyLibsLoaded = true;
}

void FGPAPluginModule::FreeThirdPartyLibraries()
{
	for (void* Handle : ThirdPartyLibraryHandles)
	{
		if (Handle)
		{
			FPlatformProcess::FreeDllHandle(Handle);
			Handle = nullptr;
		}
	}
}

void FGPAPluginModule::ShowNotification(const FString& Info)
{
	const FText notificationText = FText::Format(LOCTEXT("Notifications", "{0}"), FText::FromString(Info));
	//const FText notificationText = FText::FromString(Info);
	FNotificationInfo info(notificationText);
	info.bFireAndForget = true;
	info.FadeInDuration = 0.5f;
	info.FadeOutDuration = 1.0f;
	info.ExpireDuration = 4.0f;

	FSlateNotificationManager::Get().AddNotification(info);
}

bool FGPAPluginModule::IsGraphicsMonitorProcessRunning(const FString& AppName)
{
	DWORD ProcessIds[1024], ProcessArraySize;

	// if not possible to check running processesassume GM is not running
	if (!EnumProcesses(ProcessIds, sizeof(ProcessIds), &ProcessArraySize))
	{
		return false;
	}

	// Calculate how many process identifiers were returned.
	DWORD NumProcesses = ProcessArraySize / sizeof(DWORD);

	// Scan all running processes and get basic info for each
	// compare name with Graphics Monitor binary
	for (UINT i = 0; i < NumProcesses; i++)
	{
		if (ProcessIds[i] != 0)
		{
			HANDLE ProcessHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, ProcessIds[i]);
			if (ProcessHandle)
			{
				const DWORD ProcessNameSize = 4096;
				TCHAR ProcessName[ProcessNameSize];
				if (QueryFullProcessImageName(ProcessHandle, 0, ProcessName, (PDWORD)(&ProcessNameSize)))
				{
					if (AppName.Compare(ProcessName) == 0)
					{
						return true;
					}
				}
				CloseHandle(ProcessHandle);
			}
		}
	}

	return false;
}

void FGPAPluginModule::StartGraphicsMonitorProcess()
{
	FString GMPath = "";
	FString GMBinary = "GpaMonitor.exe";

	// look for Graphics Monitor registry entry
	FString RegSubKey = TEXT("SOFTWARE\\Intel\\Intel(R) Graphics Performance Analyzers");
	FWindowsPlatformMisc::QueryRegKey(HKEY_LOCAL_MACHINE, *RegSubKey, TEXT("Location"), GMPath);
	GMBinary = FPaths::Combine(GMPath, GMBinary);

	if (FPaths::FileExists(GMBinary))
	{
		// if Graphics Monitor already running do nothing
		if (IsGraphicsMonitorProcessRunning(GMBinary))
		{
			return;
		}

		ShowNotification("Starting Graphics Monitor in new window.");

		SHELLEXECUTEINFO shExInfo = { 0 };
		shExInfo.cbSize = sizeof(shExInfo);
		shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		shExInfo.hwnd = 0;
		shExInfo.lpVerb = L"runas";                
		shExInfo.lpFile = *GMBinary;       // Application to start    
		shExInfo.lpParameters = L"";
		shExInfo.lpDirectory = 0;
		shExInfo.nShow = SW_SHOW;
		shExInfo.hInstApp = 0;

		if (!ShellExecuteEx(&shExInfo))
		{
			UE_LOG(GPAPlugin, Warning, TEXT("Failed to start Graphics Monitor application."));
		}
	}
	else
	{
		UE_LOG(GPAPlugin, Warning, TEXT("Could not find valid Graphics Monitor location. Please verify GPA installation."));
	}
}

void FGPAPluginModule::CaptureStream(const TArray<FString>& Args)
{	
	//expecting exactly 1 argument, ignore all other cases
	if (Args.Num() != 1)
	{
		return;
	}

	// only DX12 capture fully supported at this point
	static const bool bIsDx12 = FCString::Strcmp(GDynamicRHI->GetName(), TEXT("D3D12")) == 0;
	if (!bIsDx12)
	{
		ShowNotification("Currently only DX12 stream capture is supported.\nPlease change RHI do DX12 and restart editor.");
		return;
	}

	// ignore all argumnets other than 'start'/'stop'
	if (Args[0] == "start")
	{
		// notify user if a capture session already running and quit
		// otherwise start capture
		if (bStreamCaptureRunning)
		{
			ShowNotification("GPA capture session already running.");
			return;
		}
		bStreamCaptureRunning = true;
		ShowNotification("Starting GPA stream capture.");

		// enable RHI ideal capture conditions trigger steam capture start
		GDynamicRHI->EnableIdealGPUCaptureOptions(true);
		gpa->TriggerStreamCapture();
	}
	else if (Args[0] == "stop")
	{
		if (!bStreamCaptureRunning)
		{
			ShowNotification("No GPA capture session running. Start new session to capture stream.");
			return;
		}
		bStreamCaptureRunning = false;
		ShowNotification("Stopped GPA stream capture.");

		// run Graphics Monitor application if enable in settings
		if (CVarGPARunGPAAfterCapture.GetValueOnAnyThread())
		{
			StartGraphicsMonitorProcess();
		}

		// trigger steam capture stop event and disable RHI ideal capture conditions
		gpa->TriggerStreamCapture();
		GDynamicRHI->EnableIdealGPUCaptureOptions(false);
	}
}

void FGPAPluginModule::StartupModule()
{
	// Apply settings from ini file, this will update the console variables and project settings
	ApplyCVarSettingsFromIni(TEXT("/Script/GPAPlugin.GPAPluginSettings"), *GEngineIni, ECVF_SetByProjectSetting);

	// Load all 3rd party libraries that have seen delay loaded
	LoadThirdPartyLibraries();

	// if GPA dll found and sucessfully loaded intialize capture process
	if (bAllThirdPartyLibsLoaded)
	{
		std::string Path = std::string(TCHAR_TO_UTF8(*CVarGPABinaryLocation.GetValueOnAnyThread()));
		gpa = GetGPAInterface(Path);
		if (gpa != nullptr && (gpa->Initialize() != IGPA::Result::Ok)) 
		{
			UE_LOG(GPAPlugin, Warning, TEXT("Failed to initialize GPA capture library!"));
		}
	}
	else 
	{
		return;
	}

	// register console variables that tie into the capture start/stop UI button
	static FAutoConsoleCommand CCmdRenderDocCapturePIE = FAutoConsoleCommand(
		TEXT("gpa.StreamCapture"),
		TEXT("	start: starts GPA stream capture")
		TEXT("	stop: stops GPA stream capture"),
		FConsoleCommandWithArgsDelegate::CreateRaw(this, &FGPAPluginModule::CaptureStream)
	);
	
	FGPAPluginStyle::Initialize();
	FGPAPluginStyle::ReloadTextures();

	FGPAPluginCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FGPAPluginCommands::Get().StreamCaptureAction,
		FExecuteAction::CreateRaw(this, &FGPAPluginModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FGPAPluginModule::RegisterMenus));
}

void FGPAPluginModule::ShutdownModule()
{
	// Shutdown GPA capture process
	if (gpa != nullptr)
	{
		gpa->Release();
		gpa = nullptr;
	}

	FreeThirdPartyLibraries();

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);
	FGPAPluginStyle::Shutdown();

	FGPAPluginCommands::Unregister();
}

void FGPAPluginModule::PluginButtonClicked()
{
	// this is the same action as if the gpa.CaptureStream cmd was called
	TArray<FString> CommandArgs = { };
	if (bStreamCaptureRunning)
	{
		CommandArgs.Add("stop");
	}
	else
	{
		CommandArgs.Add("start");
	}
	CaptureStream(CommandArgs);
}

void FGPAPluginModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FGPAPluginCommands::Get().StreamCaptureAction));

				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

#include "Windows/HideWindowsPlatformTypes.h"

IMPLEMENT_MODULE(FGPAPluginModule, GPAPlugin)

