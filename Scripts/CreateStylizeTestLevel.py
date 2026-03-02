import unreal

LEVEL_PATH = "/Game/Tests/L_StylizeAutoTest"
TEST_FOLDER = "/Game/Tests"


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
        result = unreal.EditorLevelLibrary.save_current_level_as(level_path)
        log(f"save_current_level_as('{level_path}') => {result}")
        return bool(result)

    if hasattr(unreal.EditorLoadingAndSavingUtils, "save_map"):
        result = unreal.EditorLoadingAndSavingUtils.save_map(world, level_path)
        log(f"save_map('{level_path}') => {result}")
        return bool(result)

    raise RuntimeError("Could not find a supported map save API.")


def spawn_static_mesh(mesh_path: str, location: unreal.Vector, scale: unreal.Vector, label: str):
    mesh = unreal.load_asset(mesh_path)
    if not mesh:
        log(f"Skipped mesh actor '{label}': mesh not found at {mesh_path}")
        return None

    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.StaticMeshActor, location, unreal.Rotator(0.0, 0.0, 0.0))
    actor.set_actor_label(label)
    actor.set_actor_scale3d(scale)
    sm_component = actor.get_editor_property("static_mesh_component")
    sm_component.set_editor_property("static_mesh", mesh)
    return actor


def spawn_lighting_and_camera():
    directional = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.DirectionalLight, unreal.Vector(0.0, 0.0, 500.0), unreal.Rotator(-45.0, 35.0, 0.0)
    )
    directional.set_actor_label("Stylize_DirectionalLight")
    directional_component = directional.get_editor_property("directional_light_component")
    directional_component.set_editor_property("intensity", 8.0)

    skylight = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.SkyLight, unreal.Vector(0.0, 0.0, 200.0), unreal.Rotator(0.0, 0.0, 0.0)
    )
    skylight.set_actor_label("Stylize_SkyLight")
    skylight_component = skylight.get_editor_property("light_component")
    skylight_component.set_editor_property("intensity", 1.0)

    fog = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.ExponentialHeightFog, unreal.Vector(0.0, 0.0, 0.0), unreal.Rotator(0.0, 0.0, 0.0)
    )
    fog.set_actor_label("Stylize_HeightFog")

    post_process = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.PostProcessVolume, unreal.Vector(0.0, 0.0, 0.0), unreal.Rotator(0.0, 0.0, 0.0)
    )
    post_process.set_actor_label("Stylize_PostProcess")
    post_process.set_editor_property("unbound", True)

    camera = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.CineCameraActor, unreal.Vector(-650.0, 0.0, 220.0), unreal.Rotator(-10.0, 0.0, 0.0)
    )
    camera.set_actor_label("Stylize_TestCamera")


def spawn_test_geometry():
    spawn_static_mesh(
        "/Engine/BasicShapes/Plane.Plane",
        unreal.Vector(0.0, 0.0, 0.0),
        unreal.Vector(10.0, 10.0, 1.0),
        "Stylize_Floor",
    )
    spawn_static_mesh(
        "/Engine/BasicShapes/Cube.Cube",
        unreal.Vector(0.0, 0.0, 100.0),
        unreal.Vector(1.5, 1.5, 1.5),
        "Stylize_Cube",
    )
    spawn_static_mesh(
        "/Engine/BasicShapes/Sphere.Sphere",
        unreal.Vector(250.0, -150.0, 100.0),
        unreal.Vector(1.0, 1.0, 1.0),
        "Stylize_Sphere",
    )
    spawn_static_mesh(
        "/Engine/BasicShapes/Cylinder.Cylinder",
        unreal.Vector(-220.0, 170.0, 100.0),
        unreal.Vector(1.0, 1.0, 1.2),
        "Stylize_Cylinder",
    )
    spawn_static_mesh(
        "/Engine/BasicShapes/Cone.Cone",
        unreal.Vector(140.0, 220.0, 120.0),
        unreal.Vector(1.0, 1.0, 1.0),
        "Stylize_Cone",
    )


def spawn_stylize_test_actor():
    stylize_level_test_class = unreal.load_class(None, "/Script/StylizeRender.StylizeLevelTestActor")
    if not stylize_level_test_class:
        log("Could not load StylizeLevelTestActor class. Plugin may not be loaded; skipping actor spawn.")
        return None

    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        stylize_level_test_class, unreal.Vector(0.0, -400.0, 100.0), unreal.Rotator(0.0, 0.0, 0.0)
    )
    actor.set_actor_label("Stylize_LevelTestActor")
    actor.set_editor_property("capture_base_name", "StylizeAuto")
    actor.set_editor_property("delay_between_captures_seconds", 1.5)
    actor.set_editor_property("test_resolutions", [unreal.IntPoint(1920, 1080), unreal.IntPoint(3840, 2160)])
    log("Spawned Stylize_LevelTestActor.")
    return actor


def main():
    log("Starting automated stylize level creation.")
    ensure_directory(TEST_FOLDER)

    world = create_new_blank_level()
    spawn_lighting_and_camera()
    spawn_test_geometry()
    spawn_stylize_test_actor()

    if not save_level(world, LEVEL_PATH):
        raise RuntimeError(f"Failed to save level at {LEVEL_PATH}")

    unreal.EditorLevelLibrary.save_current_level()
    unreal.EditorAssetLibrary.save_directory(TEST_FOLDER, only_if_is_dirty=False, recursive=True)
    log(f"Completed. Level saved to {LEVEL_PATH}")


if __name__ == "__main__":
    main()
