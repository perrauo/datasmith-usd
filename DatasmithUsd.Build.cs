// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.Diagnostics;
using System.IO;

namespace UnrealBuildTool.Rules
{
public class DatasmithUsd : ModuleRules
{
		public DatasmithUsd(ReadOnlyTargetRules Target)
			: base(Target)
		{
			// Does not compile with C++20:
			// error C2666: 'UE::FDummyWeakPtrType::operator ==': overloaded functions have similar conversions
			// error C2666: 'UE::FDummyRefPtrType::operator ==': overloaded functions have similar conversions
			CppStandard = CppStandardVersion.Cpp17;

			// Replace with PCHUsageMode.UseExplicitOrSharedPCHs when this plugin can compile with cpp20
			PCHUsage = PCHUsageMode.NoPCHs;

			bUseRTTI = true;

			// To avoid clashes with PxrUSD
			// todo: separate Slate code from 3ds max to a module
			bUseUnity = false;

			PublicDefinitions.Add("USE_USD_SDK=1");
			PublicDefinitions.Add("NEW_DIRECTLINK_PLUGIN=1");
			PublicDefinitions.Add("USD_USES_SYSTEM_MALLOC=1");

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"DatasmithExporter",
					"DatasmithExporterUI",
					"UdpMessaging", // required for DirectLink networking
					"UEOpenExr",
					"IntelTBB",
					"Slate",
					"SlateCore",
				}
			);

			PrivateIncludePathModuleNames.Add("Launch");
			bEnableUndefinedIdentifierWarnings = false;
			bEnableExceptions = true;
			bWarningsAsErrors = false;
			bEnableNonInlinedGenCppWarnings = false;
			//UEBuildConfiguration.bForceEnableExceptions = true

			PublicSystemLibraryPaths.Add("C:\\Users\\olivi\\AppData\\Local\\Programs\\Python\\Python39\\Lib");
			PublicSystemLibraryPaths.Add("C:\\Users\\olivi\\AppData\\Local\\Programs\\Python\\Python39\\libs");
			PrivateIncludePaths.Add("C:\\Users\\olivi\\AppData\\Local\\Programs\\Python\\Python39\\include");
			
			string PxrUsdLocation = "C:\\usd";

			// Make sure this version of Max is actually installed
			if (Directory.Exists(PxrUsdLocation))
			{
				PrivateIncludePaths.Add(Path.Combine(PxrUsdLocation, "include"));

				string LibraryPaths = Path.Combine(PxrUsdLocation, "lib");
				//PublicSystemLibraryPaths.Add(LibraryPaths);
				foreach(string UsdLib in Directory.EnumerateFiles(LibraryPaths, "*.lib", SearchOption.AllDirectories))
				{

					PublicAdditionalLibraries.Add(UsdLib);
				}
			}


			string BoostLibDir = "C:\\usd\\lib";
			//if(!string.IsNullOrEmpty(BoostLibDir))
			//{
			//	PublicSystemLibraryPaths.Add(BoostLibDir);
			//}
			foreach(string UsdLib in Directory.EnumerateFiles(BoostLibDir, "*.lib", SearchOption.AllDirectories))
			{

				PublicAdditionalLibraries.Add(UsdLib);
			}

			// Add the path to the Boost headers
			string BoostIncludeDir = "C:\\usd\\include\\boost-1_78";
			if(!string.IsNullOrEmpty(BoostIncludeDir))
			{
				PrivateIncludePaths.Add(BoostIncludeDir);
			}

			PrivateIncludePaths.Add(ModuleDirectory);
		}
	}
}