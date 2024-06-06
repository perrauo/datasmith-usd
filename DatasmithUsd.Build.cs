// Copyright Epic Games, Inc. All Rights Reserved.

using System.Diagnostics;
using System.IO;

namespace UnrealBuildTool.Rules
{
public class DatasmithUsd : ModuleRules
{
		public DatasmithUsd(ReadOnlyTargetRules Target)
			: base(Target)
		{

			bUseRTTI = true;

			// To avoid clashes with PxrUSD
			// todo: separate Slate code from 3ds max to a module
			bUseUnity = false; 

			PublicDefinitions.Add("NEW_DIRECTLINK_PLUGIN=1");

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"DatasmithExporter",
					"DatasmithExporterUI",

					"UdpMessaging", // required for DirectLink networking
					"UEOpenExr",

					"Slate",
					"SlateCore",
				}
			);

			PrivateIncludePathModuleNames.Add("Launch");

			PublicSystemLibraryPaths.Add("C:\\Users\\olivi\\AppData\\Local\\Programs\\Python\\Python39\\Lib");
			PublicSystemLibraryPaths.Add("C:\\Users\\olivi\\AppData\\Local\\Programs\\Python\\Python39\\libs");
			PrivateIncludePaths.Add("C:\\Users\\olivi\\AppData\\Local\\Programs\\Python\\Python39\\include");

			// Max SDK setup
			{
				string PxrUsdLocation = "C:\\usd";

				// Make sure this version of Max is actually installed
				if (Directory.Exists(PxrUsdLocation))
				{
					PrivateIncludePaths.Add(Path.Combine(PxrUsdLocation, "include"));

					string LibraryPaths = Path.Combine(PxrUsdLocation, "lib");
					PublicSystemLibraryPaths.Add(LibraryPaths);
				}
			}

			PrivateIncludePaths.Add(ModuleDirectory);
		}
	}
}