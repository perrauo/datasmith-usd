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
			// Does not compile with C++20 due to pxr usd
			CppStandard = CppStandardVersion.Cpp17;
			OptimizeCode = CodeOptimization.Never;
			// Replace with PCHUsageMode.UseExplicitOrSharedPCHs when this plugin can compile with cpp20
			PCHUsage = PCHUsageMode.NoPCHs;

			bUseRTTI = true;
			bUseUnity = true;
			bEnableUndefinedIdentifierWarnings = false;
			bEnableExceptions = true;
			bWarningsAsErrors = false;
			bEnableNonInlinedGenCppWarnings = false;

			PublicDefinitions.Add("USE_USD_SDK=1");
			PublicDefinitions.Add("NEW_DIRECTLINK_PLUGIN=1");
			PublicDefinitions.Add("USD_USES_SYSTEM_MALLOC=1");
			PublicDefinitions.Add("IS_PROGRAM=1");

			PublicDefinitions.Add("ALLOW_UDP_MESSAGING_SHIPPING=1"); // bypasses the 'if shipping' of UdpMessagingModule.cpp
			PublicDefinitions.Add("PLATFORM_SUPPORTS_MESSAGEBUS=1"); // required to enable the default MessageBus in MessagingModule.cpp		
			PublicDefinitions.Add("USE_PER_MODULE_UOBJECT_BOOTSTRAP=1");


			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"DatasmithExporter"
					, "DatasmithExporterUI"
					, "Networking"
					, "Sockets"
					, "Messaging"
					// // required for DirectLink networking
					, "UdpMessaging"
					, "UEOpenExr"
					, "IntelTBB"
					, "Slate"
					, "SlateCore"
				}
			);

			PrivateIncludePathModuleNames.Add("Launch");			

			PublicSystemLibraryPaths.Add("C:\\Users\\olivi\\AppData\\Local\\Programs\\Python\\Python39\\Lib");
			PublicSystemLibraryPaths.Add("C:\\Users\\olivi\\AppData\\Local\\Programs\\Python\\Python39\\libs");
			PrivateIncludePaths.Add("C:\\Users\\olivi\\AppData\\Local\\Programs\\Python\\Python39\\include");
			
			string PxrUsdLocation = "C:\\usd";

			if (Directory.Exists(PxrUsdLocation))
			{
				PrivateIncludePaths.Add(Path.Combine(PxrUsdLocation, "include"));

				string LibraryPaths = Path.Combine(PxrUsdLocation, "lib");
				foreach(string UsdLib in Directory.EnumerateFiles(LibraryPaths, "*.lib", SearchOption.AllDirectories))
				{

					PublicAdditionalLibraries.Add(UsdLib);
				}
			}


			string BoostLibDir = "C:\\usd\\lib";
			foreach(string UsdLib in Directory.EnumerateFiles(BoostLibDir, "*.lib", SearchOption.AllDirectories))
			{

				PublicAdditionalLibraries.Add(UsdLib);
			}
			string BoostIncludeDir = "C:\\usd\\include\\boost-1_78";
			if(!string.IsNullOrEmpty(BoostIncludeDir))
			{
				PrivateIncludePaths.Add(BoostIncludeDir);
			}

			PrivateIncludePaths.Add(ModuleDirectory);
		}
	}
}