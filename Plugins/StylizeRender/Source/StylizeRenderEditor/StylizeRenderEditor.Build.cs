using UnrealBuildTool;

public class StylizeRenderEditor : ModuleRules
{
	public StylizeRenderEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"StylizeRender",
				"UnrealEd"
			}
		);
	}
}
