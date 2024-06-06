// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

[SupportedPlatforms("Win64")]
public class DatasmithUsdTarget : TargetRules
{
	public DatasmithUsdTarget(TargetInfo Target)
		: base(Target)
	{
		LaunchModuleName = "DatasmithUsd";
		ExeBinariesSubFolder = @"DatasmithUsdPlugin";

		AddCopyPostBuildStep(Target);

		Type = TargetType.Program;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		SolutionDirectory = "Programs/Datasmith";
		bBuildInSolutionByDefault = false;
		bLegalToDistributeBinary = true;

		bShouldCompileAsDLL = true;
		LinkType = TargetLinkType.Monolithic;

		// WindowsPlatform.ModuleDefinitionFile = "Programs/Enterprise/Datasmith/DatasmithUsdExporter/DatasmithUsdExporterWithDirectLink.def";

		WindowsPlatform.bStrictConformanceMode = false;

		bBuildDeveloperTools = false;
		bBuildWithEditorOnlyData = true;
		bCompileAgainstEngine = false;
		bCompileAgainstCoreUObject = true;
		bCompileICU = false;
		bUsesSlate = false;

		bHasExports = true;
		bForceEnableExceptions = true;

		GlobalDefinitions.Add("UE_EXTERNAL_PROFILING_ENABLED=0"); // For DirectLinkUI (see FDatasmithExporterManager::FInitOptions)

		CppStandard = CppStandardVersion.Cpp17;
	}

	protected void AddCopyPostBuildStep(TargetInfo Target)
	{
		string OutputName = "$(TargetName)";
		if (Target.Configuration != UnrealTargetConfiguration.Development)
		{
			OutputName = string.Format("{0}-{1}-{2}", OutputName, Target.Platform, Target.Configuration);
		}

		string SrcOutputFileName = string.Format(@"$(EngineDir)\Binaries\Win64\{0}\{1}.dll", ExeBinariesSubFolder, OutputName);

		string DstOutputFileName;

		DstOutputFileName = string.Format(@"$(EngineDir)\Binaries\Win64\{0}\{1}.pyd", ExeBinariesSubFolder, OutputName);

		PostBuildSteps.Add(string.Format("echo Copying {0} to {1}...", SrcOutputFileName, DstOutputFileName));
		PostBuildSteps.Add(string.Format("copy /Y \"{0}\" \"{1}\" 1>nul", SrcOutputFileName, DstOutputFileName));

		// Copy resources
		// TODO: For some reason this only occurs when clicking "Rebuild"
		PostBuildSteps.Add(string.Format("echo D|xcopy /Y /R /F /S \"{0}\" \"{1}\"",
			string.Format(@"$(EngineDir)/Source/Programs/Enterprise/Datasmith/DatasmithUsdPlugin/Resources"),
			string.Format(@"$(EngineDir)/Binaries/Win64/{0}", ExeBinariesSubFolder)
			));
	}
}

