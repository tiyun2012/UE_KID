import unreal

LEVEL_PATH = "/Game/Tests/L_StylizeAutoTest"
WATER_SIM_MATERIAL_PATH = "/Game/StylizeRender/Materials/M_WaterSim_Template"
WATER_SURFACE_MATERIAL_PATH = "/Game/StylizeRender/Materials/M_WaterSurface_Template"


def log(message: str) -> None:
    unreal.log(f"[StylizeLevelVerify] {message}")


def load_level(level_path: str) -> None:
    if hasattr(unreal.EditorLoadingAndSavingUtils, "load_map"):
        loaded_world = unreal.EditorLoadingAndSavingUtils.load_map(level_path)
        if loaded_world:
            return

    if hasattr(unreal.EditorLevelLibrary, "load_level"):
        result = unreal.EditorLevelLibrary.load_level(level_path)
        if bool(result):
            return

    raise RuntimeError(f"Could not load level: {level_path}")


def find_first_actor_by_class_name(actors, class_name: str):
    for actor in actors:
        if actor.get_class().get_name() == class_name:
            return actor
    return None


def get_all_level_actors():
    try:
        actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
        if actor_subsystem:
            return actor_subsystem.get_all_level_actors()
    except Exception:
        pass
    return unreal.EditorLevelLibrary.get_all_level_actors()


def get_asset_path_or_empty(asset_object) -> str:
    if not asset_object:
        return ""
    return asset_object.get_path_name()


def normalize_object_path(path: str) -> str:
    if "." in path:
        return path.split(".", 1)[0]
    return path


def ensure_equal(actual: str, expected: str, label: str) -> None:
    if normalize_object_path(actual) != normalize_object_path(expected):
        raise RuntimeError(f"{label} mismatch. Expected '{expected}', got '{actual}'")


def get_actor_label(actor) -> str:
    if not actor:
        return ""
    try:
        return actor.get_actor_label()
    except Exception:
        return ""


def main() -> None:
    if not unreal.EditorAssetLibrary.does_asset_exist(LEVEL_PATH):
        raise RuntimeError(f"Missing level asset: {LEVEL_PATH}")

    load_level(LEVEL_PATH)

    actors = get_all_level_actors()
    log(f"Loaded level and found {len(actors)} actors.")

    water_actor = find_first_actor_by_class_name(actors, "StylizeWaterActor")
    if not water_actor:
        raise RuntimeError("StylizeWaterActor not found in test level.")

    level_test_actor = find_first_actor_by_class_name(actors, "StylizeLevelTestActor")
    if not level_test_actor:
        raise RuntimeError("StylizeLevelTestActor not found in test level.")

    sim_material = water_actor.get_editor_property("simulation_material_template")
    surface_material = water_actor.get_editor_property("water_surface_material_template")

    sim_path = get_asset_path_or_empty(sim_material)
    surface_path = get_asset_path_or_empty(surface_material)

    ensure_equal(sim_path, WATER_SIM_MATERIAL_PATH, "Simulation material")
    ensure_equal(surface_path, WATER_SURFACE_MATERIAL_PATH, "Surface material")

    wave_amplitude = float(water_actor.get_editor_property("wave_amplitude"))
    foam_intensity = float(water_actor.get_editor_property("foam_intensity"))
    if wave_amplitude <= 0.0:
        raise RuntimeError(f"WaveAmplitude must be > 0. Current value: {wave_amplitude}")
    if foam_intensity < 0.0:
        raise RuntimeError(f"FoamIntensity must be >= 0. Current value: {foam_intensity}")

    depth_step_labels = {get_actor_label(actor) for actor in actors}
    expected_depth_steps = {f"Stylize_DepthStep_{index}" for index in range(10)}
    missing_depth_steps = sorted(expected_depth_steps - depth_step_labels)
    if missing_depth_steps:
        raise RuntimeError(f"Missing depth stress actors: {missing_depth_steps}")

    log(f"Water actor material assignment OK: sim={sim_path}, surface={surface_path}")
    log(f"Water visuals OK: WaveAmplitude={wave_amplitude}, FoamIntensity={foam_intensity}")
    log("Depth stress test actors OK (10/10).")
    log("VERIFY_OK")


if __name__ == "__main__":
    main()
