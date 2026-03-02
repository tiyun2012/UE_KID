import unreal

MATERIAL_DIR = "/Game/StylizeRender/Materials"
SIM_MATERIAL_NAME = "M_WaterSim_Template"
SURFACE_MATERIAL_NAME = "M_WaterSurface_Template"


def log(message: str) -> None:
    unreal.log(f"[StylizeWaterAssets] {message}")


def ensure_directory(path: str) -> None:
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)
        log(f"Created directory: {path}")


def create_or_load_material(material_name: str):
    asset_path = f"{MATERIAL_DIR}/{material_name}"
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        existing = unreal.load_asset(asset_path)
        if existing:
            log(f"Using existing material: {asset_path}")
            return existing, False

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    factory = unreal.MaterialFactoryNew()
    created = asset_tools.create_asset(material_name, MATERIAL_DIR, unreal.Material, factory)
    if not created:
        raise RuntimeError(f"Failed to create material: {asset_path}")

    log(f"Created material: {asset_path}")
    return created, True


def create_sim_material_graph(material: unreal.Material) -> None:
    try:
        material.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
    except Exception:
        pass

    tex = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionTextureSampleParameter2D, -900, -50
    )
    tex.set_editor_property("parameter_name", "CurrentState")

    dt = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionScalarParameter, -900, 180
    )
    dt.set_editor_property("parameter_name", "Dt")
    dt.set_editor_property("default_value", 0.016)

    viscosity = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionScalarParameter, -900, 290
    )
    viscosity.set_editor_property("parameter_name", "Viscosity")
    viscosity.set_editor_property("default_value", 0.98)

    boat = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionVectorParameter, -900, 420
    )
    boat.set_editor_property("parameter_name", "BoatData")
    boat.set_editor_property("default_value", unreal.LinearColor(0.0, 0.0, 0.0, 0.0))

    obstacle0 = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionVectorParameter, -900, 530
    )
    obstacle0.set_editor_property("parameter_name", "Obstacle0")
    obstacle0.set_editor_property("default_value", unreal.LinearColor(0.0, 0.0, 0.0, 0.0))

    obstacle1 = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionVectorParameter, -900, 640
    )
    obstacle1.set_editor_property("parameter_name", "Obstacle1")
    obstacle1.set_editor_property("default_value", unreal.LinearColor(0.0, 0.0, 0.0, 0.0))

    obstacle2 = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionVectorParameter, -900, 750
    )
    obstacle2.set_editor_property("parameter_name", "Obstacle2")
    obstacle2.set_editor_property("default_value", unreal.LinearColor(0.0, 0.0, 0.0, 0.0))

    # Minimal bootstrap graph: pass current state through to emissive.
    unreal.MaterialEditingLibrary.connect_material_property(
        tex, "RGB", unreal.MaterialProperty.MP_EMISSIVE_COLOR
    )

    unreal.MaterialEditingLibrary.layout_material_expressions(material)
    unreal.MaterialEditingLibrary.recompile_material(material)


def create_surface_material_graph(material: unreal.Material) -> None:
    try:
        material.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_SINGLELAYERWATER)
    except Exception:
        log("MSM_SINGLELAYERWATER unavailable in python API; keeping default shading model.")

    tex = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionTextureSampleParameter2D, -950, -50
    )
    tex.set_editor_property("parameter_name", "SimulationTexture")

    wave_amplitude = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionScalarParameter, -950, 200
    )
    wave_amplitude.set_editor_property("parameter_name", "WaveAmplitude")
    wave_amplitude.set_editor_property("default_value", 24.0)

    multiply = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionMultiply, -650, 120
    )
    unreal.MaterialEditingLibrary.connect_material_expressions(tex, "R", multiply, "A")
    unreal.MaterialEditingLibrary.connect_material_expressions(wave_amplitude, "", multiply, "B")

    zero_a = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionConstant, -650, 260
    )
    zero_a.set_editor_property("r", 0.0)

    zero_b = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionConstant, -650, 340
    )
    zero_b.set_editor_property("r", 0.0)

    append_xy = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionAppendVector, -420, 280
    )
    unreal.MaterialEditingLibrary.connect_material_expressions(zero_a, "", append_xy, "A")
    unreal.MaterialEditingLibrary.connect_material_expressions(zero_b, "", append_xy, "B")

    append_xyz = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionAppendVector, -220, 180
    )
    unreal.MaterialEditingLibrary.connect_material_expressions(append_xy, "", append_xyz, "A")
    unreal.MaterialEditingLibrary.connect_material_expressions(multiply, "", append_xyz, "B")

    foam_intensity = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionScalarParameter, -950, 320
    )
    foam_intensity.set_editor_property("parameter_name", "FoamIntensity")
    foam_intensity.set_editor_property("default_value", 2.0)

    foam_mul = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionMultiply, -650, 380
    )
    unreal.MaterialEditingLibrary.connect_material_expressions(tex, "B", foam_mul, "A")
    unreal.MaterialEditingLibrary.connect_material_expressions(foam_intensity, "", foam_mul, "B")

    unreal.MaterialEditingLibrary.connect_material_property(
        tex, "RGB", unreal.MaterialProperty.MP_BASE_COLOR
    )
    unreal.MaterialEditingLibrary.connect_material_property(
        append_xyz, "", unreal.MaterialProperty.MP_WORLD_POSITION_OFFSET
    )
    unreal.MaterialEditingLibrary.connect_material_property(
        foam_mul, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR
    )

    unreal.MaterialEditingLibrary.layout_material_expressions(material)
    unreal.MaterialEditingLibrary.recompile_material(material)


def save_material(material_name: str) -> None:
    asset_path = f"{MATERIAL_DIR}/{material_name}"
    unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
    log(f"Saved asset: {asset_path}")


def main() -> None:
    ensure_directory("/Game/StylizeRender")
    ensure_directory(MATERIAL_DIR)

    sim_material, sim_created = create_or_load_material(SIM_MATERIAL_NAME)
    if sim_created:
        create_sim_material_graph(sim_material)
        save_material(SIM_MATERIAL_NAME)
    else:
        log("Simulation material already exists; skipping graph rebuild.")

    surface_material, surface_created = create_or_load_material(SURFACE_MATERIAL_NAME)
    if surface_created:
        create_surface_material_graph(surface_material)
        save_material(SURFACE_MATERIAL_NAME)
    else:
        log("Surface material already exists; skipping graph rebuild.")

    unreal.EditorAssetLibrary.save_directory(MATERIAL_DIR, only_if_is_dirty=False, recursive=True)
    log("Completed stylize water material asset creation.")


if __name__ == "__main__":
    main()
