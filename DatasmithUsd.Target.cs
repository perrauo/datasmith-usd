// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

using static UnrealBuildTool.ModuleRules;

[SupportedPlatforms("Win64")]
[SupportedConfigurations(UnrealTargetConfiguration.Debug, UnrealTargetConfiguration.Development, UnrealTargetConfiguration.Shipping)]
public class DatasmithUsdTarget : TargetRules
{
	public DatasmithUsdTarget(TargetInfo Target)
		: base(Target)
	{
		Type = TargetType.Program;
		Platform = UnrealTargetPlatform.Win64;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		LinkType = TargetLinkType.Monolithic;
		CppStandard = CppStandardVersion.Cpp17;

		LaunchModuleName = "DatasmithUsd";
		ExeBinariesSubFolder = @"DatasmithUsdPlugin";
		SolutionDirectory = "Programs/Datasmith";

		GlobalDefinitions.Add("UE_EXTERNAL_PROFILING_ENABLED=0");
		AdditionalPlugins.AddRange(new string[]{
			"UdpMessaging"
			});
		EnablePlugins.AddRange(new string[]{
			"UdpMessaging"
			});

		bBuildInSolutionByDefault = false;
		bShouldCompileAsDLL = true;
		WindowsPlatform.bStrictConformanceMode = false;
		bCompileWithPluginSupport = true;
		bBuildDeveloperTools = true;
		bCompileAgainstEditor = false;
		bBuildWithEditorOnlyData = false;
		bCompileAgainstEngine = false;
		bCompileAgainstCoreUObject = true;
		bCompileAgainstApplicationCore = true;
		bCompileICU = false;
		bForceEnableExceptions = false;
		bUsesSlate = true;

		AddCopyPostBuildStep(Target);
	}

	protected void AddCopyPostBuildStep(TargetInfo Target)
	{
		string OutputName = "$(TargetName)";
		if (Target.Configuration != UnrealTargetConfiguration.Development)
		{
			OutputName = string.Format("{0}-{1}-{2}", OutputName, Target.Platform, Target.Configuration);
		}

		string SrcOutputFileName = string.Format(@"$(EngineDir)\Binaries\Win64\{0}\{1}.dll", ExeBinariesSubFolder, OutputName);
		string DstOutputFileName = string.Format(@"$(EngineDir)\Binaries\Win64\{0}\{1}.pyd", ExeBinariesSubFolder, OutputName);

		PostBuildSteps.Add(string.Format("echo Copying {0} to {1}...", SrcOutputFileName, DstOutputFileName));
		PostBuildSteps.Add(string.Format("copy /Y \"{0}\" \"{1}\" 1>nul", SrcOutputFileName, DstOutputFileName));
		PostBuildSteps.Add(string.Format("echo D|xcopy /Y /R /F /S \"{0}\" \"{1}\"",
			string.Format(@"$(EngineDir)/Source/Programs/Enterprise/Datasmith/DatasmithUsdPlugin/Resources"),
			string.Format(@"$(EngineDir)/Binaries/Win64/{0}", ExeBinariesSubFolder)
			));
	}
}

