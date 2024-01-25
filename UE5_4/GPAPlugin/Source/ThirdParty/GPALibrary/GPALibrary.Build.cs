// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class GPALibrary : ModuleRules
{
	public GPALibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Add the import library
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib", "igpa-shim-loader-x64.lib"));

			// Delay-load the DLLs, StartupModule() will load the them from the correct location
			PublicDelayLoadDLLs.Add("logger-x64.dll");
			PublicDelayLoadDLLs.Add("runtime-x64.dll");
			PublicDelayLoadDLLs.Add("igpa-shim-loader-x64.dll");
		}
	}
}
