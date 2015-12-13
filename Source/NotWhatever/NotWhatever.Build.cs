// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class NotWhatever : ModuleRules
{
	public NotWhatever(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
        
        LoadSpheroLibrary(Target);

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
		// {
		//		if (UEBuildConfiguration.bCompileSteamOSS == true)
		//		{
		//			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		//		}
		// }
	}
    
    private string ModulePath
    {
        get { return Path.GetDirectoryName( RulesCompiler.GetModuleFilename( this.GetType().Name ) ); }
    }
 
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath( Path.Combine( ModulePath, "../../ThirdParty/" ) ); }
    }
    
    public bool LoadSpheroLibrary(TargetInfo Target)
    {
        bool isLibrarySupported = false;
 
        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            isLibrarySupported = true;
 
            //string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            string LibrariesPath = Path.Combine(ThirdPartyPath, "SpheroLibrary", "Libraries");
 
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "SpheroLibrary.lib")); 
        }
 
        if (isLibrarySupported)
        {
            // Include path
            PublicIncludePaths.Add( Path.Combine( ThirdPartyPath, "SpheroLibrary", "Includes" ) );
        }
 
        Definitions.Add(string.Format( "WITH_SPHERO_LIBRARY_BINDING={0}", isLibrarySupported ? 1 : 0 ) );
 
        return isLibrarySupported;
    }
}
