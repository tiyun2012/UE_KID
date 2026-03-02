import unreal

LEVEL_PATH = "/Game/Tests/L_StylizeAutoTest"
TEST_FOLDER = "/Game/Tests"

# Update these paths to match your project assets.
DEFAULT_MPC_PATH = "/Game/StylizeRender/Materials/MPC_StylizeGlobal"
DEFAULT_PRESET_PATH = "/Game/StylizeRender/Presets/DA_StylizeDefault"
DEFAULT_WATER_SIM_MATERIAL_PATH = "/Game/StylizeRender/Materials/M_WaterSim_Template"
DEFAULT_WATER_SURFACE_MATERIAL_PATH = "/Game/StylizeRender/Materials/M_WaterSurface_Template"


def log(message: str) -> None:
    unreal.log(f"[StylizeAutoLevel] {message}")


def ensure_directory(path: str) -> None:
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)
        log(f"Created directory: {path}")


def create_new_blank_level():
    if hasattr(unreal.EditorLoadingAndSavingUtils, "new_blank_map"):
        world = unreal.EditorLoadingAndSavingUtils.new_blank_map(False)
        log("Created new blank map.")
        return world
    raise RuntimeError("EditorLoadingAndSavingUtils.new_blank_map is not available.")


def save_level(world, level_path: str) -> bool:
    if hasattr(unreal.EditorLevelLibrary, "save_current_level_as"):
        return bool(unreal.EditorLevelLibrary.save_current_level_as(level_path))

    if hasattr(unreal.EditorLoadingAndSavingUtils, "save_map"):
        return bool(unreal.EditorLoadingAndSavingUtils.save_map(world, level_path))

    raise RuntimeError("Could not find a supported map save API.")


def spawn_actor(actor_class, location, name, folder=""):
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(actor_class, location, unreal.Rotator(0.0, 0.0, 0.0))
    if actor:
        actor.set_actor_label(name)
        if folder:
            actor.set_folder_path(folder)
    return actor


def spawn_static_mesh(mesh_path: str, location: unreal.Vector, scale: unreal.Vector, label: str, folder: str):
    mesh = unreal.load_asset(mesh_path)
    if not mesh:
        log(f"Skipped mesh '{label}': not found at {mesh_path}")
        return None

    actor = spawn_actor(unreal.StaticMeshActor, location, label, folder)
    if not actor:
        log(f"Failed to spawn mesh actor '{label}'")
        return None

    actor.set_actor_scale3d(scale)
    sm_component = actor.get_editor_property("static_mesh_component")
    sm_component.set_editor_property("static_mesh", mesh)
    return actor


def spawn_environment():
    folder = "Environment"

    directional = spawn_actor(unreal.DirectionalLight, unreal.Vector(0, 0, 500), "Stylize_DirectionalLight", folder)
    if directional:
        directional.set_actor_rotation(unreal.Rotator(-45.0, 35.0, 0.0), False)
        directional.get_editor_property("directional_light_component").set_editor_property("intensity", 8.0)

    skylight = spawn_actor(unreal.SkyLight, unreal.Vector(0, 0, 200), "Stylize_SkyLight", folder)
    if skylight:
        skylight.get_editor_property("light_component").set_editor_property("intensity", 1.0)

    spawn_actor(unreal.ExponentialHeightFog, unreal.Vector(0, 0, 0), "Stylize_HeightFog", folder)

    post_process = spawn_actor(unreal.PostProcessVolume, unreal.Vector(0, 0, 0), "Stylize_PostProcess", folder)
    if post_process:
        post_process.set_editor_property("unbound", True)

    camera = spawn_actor(unreal.CineCameraActor, unreal.Vector(-650, 0, 220), "Stylize_TestCamera", folder)
    if camera:
        camera.set_actor_rotation(unreal.Rotator(-10.0, 0.0, 0.0), False)


def spawn_test_geometry():
    folder = "TestGeometry"
    spawn_static_mesh("/Engine/BasicShapes/Plane.Plane", unreal.Vector(0, 0, -20), unreal.Vector(10, 10, 1), "Stylize_Floor", folder)
    spawn_static_mesh("/Engine/BasicShapes/Cube.Cube", unreal.Vector(0, 0, 100), unreal.Vector(1.5, 1.5, 1.5), "Stylize_Cube", folder)
    spawn_static_mesh("/Engine/BasicShapes/Sphere.Sphere", unreal.Vector(250, -150, 100), unreal.Vector(1, 1, 1), "Stylize_Sphere", folder)
    spawn_static_mesh("/Engine/BasicShapes/Cylinder.Cylinder", unreal.Vector(-220, 170, 100), unreal.Vector(1, 1, 1.2), "Stylize_Cylinder", folder)


def spawn_depth_stress_test():
    """Spawns stepped geometry across depth to validate shallow/deep gradients and intersection foam."""
    folder = "TestGeometry/DepthStress"
    for index in range(10):
        z_position = -50.0 - (index * 20.0)
        x_position = 400.0 + (index * 80.0)
        spawn_static_mesh(
            "/Engine/BasicShapes/Cube.Cube",
            unreal.Vector(x_position, 0.0, z_position),
            unreal.Vector(0.8, 4.0, 1.0),
            f"Stylize_DepthStep_{index}",
            folder,
        )


def spawn_stylize_water_actor():
    water_class = unreal.load_class(None, "/Script/StylizeRender.StylizeWaterActor")
    if not water_class:
        log("Could not load StylizeWaterActor. Skipping.")
        return

    actor = spawn_actor(water_class, unreal.Vector(0, 0, 0), "Stylize_WaterActor", "StylizeSystems")
    if not actor:
        log("Failed to spawn Stylize_WaterActor.")
        return

    actor.set_editor_property("simulation_resolution", 256)
    actor.set_editor_property("viscosity", 0.98)

    sim_material = unreal.load_asset(DEFAULT_WATER_SIM_MATERIAL_PATH)
    if sim_material:
        actor.set_editor_property("simulation_material_template", sim_material)
    else:
        log(f"Warning: Water simulation material not found at {DEFAULT_WATER_SIM_MATERIAL_PATH}")

    surface_material = unreal.load_asset(DEFAULT_WATER_SURFACE_MATERIAL_PATH)
    if surface_material:
        actor.set_editor_property("water_surface_material_template", surface_material)
    else:
        log(f"Warning: Water surface material not found at {DEFAULT_WATER_SURFACE_MATERIAL_PATH}")

    log("Spawned Stylize_WaterActor.")


def spawn_stylize_test_actor():
    test_class = unreal.load_class(None, "/Script/StylizeRender.StylizeLevelTestActor")
    if not test_class:
        log("Could not load StylizeLevelTestActor. Skipping.")
        return

    actor = spawn_actor(test_class, unreal.Vector(0, -400, 100), "Stylize_LevelTestActor", "StylizeSystems")
    if not actor:
        log("Failed to spawn Stylize_LevelTestActor.")
        return

    actor.set_editor_property("capture_base_name", "StylizeAuto")
    actor.set_editor_property("delay_between_captures_seconds", 1.5)
    actor.set_editor_property("test_resolutions", [unreal.IntPoint(1920, 1080), unreal.IntPoint(3840, 2160)])

    mpc = unreal.load_asset(DEFAULT_MPC_PATH)
    preset = unreal.load_asset(DEFAULT_PRESET_PATH)

    if mpc:
        actor.set_editor_property("material_parameter_collection", mpc)
    else:
        log(f"Warning: MPC not found at {DEFAULT_MPC_PATH}")

    if preset:
        actor.set_editor_property("active_preset", preset)
        actor.set_editor_property("test_presets", [preset])
    else:
        log(f"Warning: Preset not found at {DEFAULT_PRESET_PATH}")

    log("Spawned and configured Stylize_LevelTestActor.")


def main():
    log("Starting automated stylize level creation.")
    ensure_directory(TEST_FOLDER)

    world = create_new_blank_level()
    spawn_environment()
    spawn_test_geometry()
    spawn_depth_stress_test()
    spawn_stylize_water_actor()
    spawn_stylize_test_actor()

    if save_level(world, LEVEL_PATH):
        unreal.EditorAssetLibrary.save_directory(TEST_FOLDER, only_if_is_dirty=False, recursive=True)
        log(f"Completed. Level saved to {LEVEL_PATH}")
    else:
        log("Failed to save level.")


if __name__ == "__main__":
    main()
